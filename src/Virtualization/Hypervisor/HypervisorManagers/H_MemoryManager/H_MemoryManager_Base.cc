//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "H_MemoryManager_Base.h"

H_MemoryManager_Base::~H_MemoryManager_Base() {
}

void H_MemoryManager_Base::initialize() {

	std::ostringstream osStream;

    nodeGate = 0;

    blockSize_KB = par("blockSize_KB").longValue();
    memorySize_MB = par("memorySize_MB").longValue();

    // Init the gate IDs to/from CPU
        fromVMMemoryI = NULL;
        fromVMMemoryO = NULL;
        toVMMemoryI   = NULL;
        toVMMemoryO   = NULL;

    // Init the gates IDs to/from Vms
        fromNodeMemoryI = NULL;
        toNodeMemoryI   = NULL;
        fromNodeMemoryO = NULL;
        toNodeMemoryO   = NULL;

    // Init the gate IDs to/from CPU

		fromVMMemoryI = new cGateManager(this);
		fromVMMemoryO = new cGateManager(this);
		toVMMemoryI   = new cGateManager(this);
		toVMMemoryO   = new cGateManager(this);

        fromVMMemoryI ->linkGate("fromVMMemoryI",nodeGate);
        fromVMMemoryO ->linkGate("fromVMMemoryO",nodeGate);
        toVMMemoryI   ->linkGate("toVMMemoryI",nodeGate);
        toVMMemoryO   ->linkGate("toVMMemoryO",nodeGate);

		// Init the gates IDs to/from Vms
        fromNodeMemoryI = gate ("fromNodeMemoryI");
        toNodeMemoryI  = gate ("toNodeMemoryI");

		fromNodeMemoryO = gate ("fromNodeMemoryO");
		toNodeMemoryO   = gate ("toNodeMemoryO");

	    vms.clear();

	    memory_overhead_MB = par("memoryOverhead_MB");
	    nodeOn = false;

	    icancloud_Base::initialize();
}

void H_MemoryManager_Base::finish(){
    vms.clear();
	icancloud_Base::finish();
}

void H_MemoryManager_Base::processSelfMessage (cMessage *msg){

    std::ostringstream msgLine;
    msgLine << "Unknown self message [" << msg->getName() << "]";
    throw cRuntimeError(msgLine.str().c_str());

    delete (msg);

}

cGate* H_MemoryManager_Base::getOutGate (cMessage *msg){

	// If msg arrive from VM
    if (msg->arrivedOn("fromVMMemoryI"))
        return (gate("toVMMemoryI", msg->getArrivalGate()->getIndex()));

    // If msg arrive from Node
    else if (msg->arrivedOn("fromNodeMemoryI"))
                return (gate("toNodeMemoryI"));

    // If msg arrive from VM
    else if (msg->arrivedOn("fromVMMemoryO"))
        return (gate("toVMMemoryO", msg->getArrivalGate()->getIndex()));

    // If msg arrive from Node
    else if (msg->arrivedOn("fromNodeMemoryO"))
        return (gate("toNodeMemoryO"));

    else
        // If gate not found!
        return NULL;
}

void H_MemoryManager_Base::processRequestMessage (icancloud_Message *msg){

	// Define ..
		icancloud_App_MEM_Message *sm_mem;
		icancloud_Message *sm_mem_base;
		icancloud_Migration_Message *sm_migration;
		int operation;

	// Begin ..

		// Check the message operation ..
        operation = msg->getOperation();

	// The operation is a device state change
    if (operation == SM_CHANGE_DISK_STATE)

        sendRequestMessage(msg, toVMMemoryO->getGate(msg->getArrivalGate()->getIndex()));

    else if (operation == SM_CHANGE_MEMORY_STATE){

        // the node is turning on
        if (strcmp(msg->getChangingState().c_str(),"off") != 0) {

            if ((!nodeOn) && (memory_overhead_MB != 0.0)){
                nodeOn = true;

                // Create a new message!
                sm_mem = new icancloud_App_MEM_Message();

                // Base parameters...
                sm_mem->setOperation (SM_MEM_ALLOCATE);
                sm_mem->setUid(0);
                sm_mem->setPid(this->getId());
                sm_mem->setMemSize(memory_overhead_MB * 1024);
                // Update message length
                sm_mem->updateLength ();

                // send the message allocating the overhead of the hypervisor
                sendRequestMessage(sm_mem, toNodeMemoryI);
            }
        }

        // send the request message to change the memory state
        sendRequestMessage(msg, toNodeMemoryI);

    }
    // The operation is a system operation
	else if (operation == ALLOCATE_MIGRATION_DATA)  {

        sm_migration = check_and_cast <icancloud_Migration_Message*> (msg);

        // Analyze the contents of the message.
        sm_mem = migrationToMemoryContents(sm_migration);

        // Return the message allocation. The message copy about perform operations on memory had been created
        sm_migration->setIsResponse(true);
        sendResponseMessage(sm_migration);

        // Perform the I/O Operation
        schedulingMemory(sm_mem);
    }
    else if ((operation == GET_MIGRATION_DATA) || ((operation == GET_MIGRATION_CONNECTIONS))){

        sm_migration = check_and_cast <icancloud_Migration_Message*> (msg);

        if (sm_migration->getMemorySizeKB() != 0){

            sm_mem = migrationToMemoryContents(sm_migration);
            sm_mem->setOperation(SM_MEM_RELEASE);
            sm_migration->setMemorySizeKB(sm_mem->getMemSize() / KB);
//            sm_migration->setMemorySizeKB((cell->getTotalVMMemory()-cell->getRemainingVMMemory()) * blockSize_KB * 1024);
//            cell->freeAllMemory();

            sm_mem_base = check_and_cast <icancloud_Message*> (sm_mem);

            //Send the message to the memory devices
               sendRequestMessage(sm_mem_base, toNodeMemoryI);
        }

        // The message has to be returned to its owner
            msg->setResult (RETURN_MESSAGE);
            msg->setIsResponse(true);

        // Send message back!
            sendResponseMessage (msg);

        // The remote storage app check this value to set the connections in the block or not ..
            msg->setResult(MEMORY_DATA);
    }
    else if ((operation == SM_SET_IOR) || (operation == SM_CREATE_FILE) ||
            (operation == SM_DELETE_FILE) || (operation == SM_OPEN_FILE) ||
            (operation == SM_CLOSE_FILE) || (operation == SM_DELETE_USER_FS)
            ){

        if (msg->getRemoteOperation()){
            sendRequestMessage(msg, toVMMemoryI->getGate(nodeGate));
        }
        else {
            int idx = getVMGateIdx(msg->getUid(), msg->getPid());

            if (idx == -1){
                sendRequestMessage(msg, toVMMemoryO->getGate(nodeGate));

            }
            else{
                sendRequestMessage(msg, toVMMemoryO->getGate(idx));
            }
        }
    }
    else
        schedulingMemory(msg);

}

void H_MemoryManager_Base::processResponseMessage (icancloud_Message *msg){

    if (msg->getResult() == MEMORY_DATA){

	    delete(msg);

	} else {
	    if ((msg->getUid() == 0) && (this->getId() == msg->getPid()))
	        delete (msg);
	    else
	        sendResponseMessage(msg);

	}
}


int H_MemoryManager_Base::setVM (cGate* oGateI, cGate* oGateO, cGate* iGateI, cGate* iGateO, int requestedMemory_KB, int uId, int pId){

    int idxToVMi;
    int idxToVMo;
    int idxFromVMi;
    int idxFromVMo;

    // Initialize control structure at node
        vmControl* control;
        control = new vmControl();
        control->gate = -1;
        control->uId = uId;
        control->pId = pId;

    // Connect to output gates
        idxToVMi = toVMMemoryI->newGate("toVMMemoryI");
        toVMMemoryI->connectOut(iGateI,idxToVMi);

        idxToVMo = toVMMemoryO->newGate("toVMMemoryO");
        toVMMemoryO->connectOut(iGateO,idxToVMo);

    // Connect to input gates
        idxFromVMo = fromVMMemoryO->newGate("fromVMMemoryO");
        fromVMMemoryO->connectIn(oGateO,idxFromVMo);

        idxFromVMi = fromVMMemoryI->newGate("fromVMMemoryI");
        fromVMMemoryI->connectIn(oGateI,idxFromVMi);

        control->gate = idxFromVMo;
        vms.push_back(control);

        return idxToVMo;

}

void H_MemoryManager_Base::freeVM (int uId, int pId){

    bool found = false;
    vmControl* control;

    for (int i = 0; (i < (int)vms.size()) && (!found); i++){

        control = (*(vms.begin() + i));

        if ((control->uId == uId) && (control->pId == pId)){

            toVMMemoryI ->freeGate (control->gate);
            toVMMemoryO ->freeGate (control->gate);

            fromVMMemoryI->freeGate (control->gate);
            fromVMMemoryO->freeGate (control->gate);

            vms.erase(vms.begin()+i);
            found = true;
        }
    }

    if (!found) throw cRuntimeError ("H_CPUManager_Base::freeVM--> vm id %i not exists at hypervisor\n", pId);
}


void H_MemoryManager_Base::sendMemoryMessage(icancloud_Message* msg){
    int gateIdx;
    //Get the gate and the index of the arrival msg
        if (msg->arrivedOn("fromVMMemoryI")){

            sendRequestMessage(msg, toNodeMemoryI);

        }else if (msg->arrivedOn("fromVMMemoryO")){

            sendRequestMessage(msg, toNodeMemoryO);

        }else if (msg->arrivedOn("fromNodeMemoryI")){

            gateIdx = getVMGateIdx(msg->getUid(), msg->getPid());

            if (gateIdx == -1){
                printf("H_MemoryManager_Base::sendMemoryMessage(toVMMemoryI[1]->uid - %i::::pid - %i\n",msg->getUid(), msg->getPid());
                delete(msg);
            }
            else
                sendRequestMessage(msg, toVMMemoryI->getGate(gateIdx));
        }else if (msg->arrivedOn("fromNodeMemoryO")){

           gateIdx = getVMGateIdx(msg->getUid(), msg->getPid());

            if (gateIdx == -1){
                sendRequestMessage(msg, toVMMemoryO->getGate(nodeGate));
            }
            else{
                sendRequestMessage(msg, toVMMemoryO->getGate(gateIdx));
            }

        }else{
            throw cRuntimeError("H_MemoryManager_Base::sendMemoryMessage->Error in the arrival of a message");
        }
}

icancloud_App_MEM_Message * H_MemoryManager_Base::migrationToMemoryContents(icancloud_Migration_Message* sm){

    icancloud_App_MEM_Message *newMessage;
    TCPCommand *controlNew;
    TCPCommand *controlOld;
    int i;

        // Create a new message!
        newMessage = new icancloud_App_MEM_Message();

        // Base parameters...
        newMessage->setOperation (sm->getOperation());
        newMessage->setIsResponse (sm->getIsResponse());
        newMessage->setRemoteOperation (sm->getRemoteOperation());
        newMessage->setConnectionId (sm->getConnectionId());
        newMessage->setCommId(sm->getCommId());
        newMessage->setSourceId (sm->getSourceId());
        newMessage->setNextModuleIndex (sm->getNextModuleIndex());
        newMessage->setResult (sm->getResult());
        newMessage->setUid(sm->getUid());
        newMessage->setPid(sm->getPid());
        newMessage->setParentRequest (sm->getParentRequest());

        if (sm->getMemorySizeKB() > 0){

            newMessage->setOperation(SM_MEM_ALLOCATE);
            newMessage->setByteLength (sm->getMemorySizeKB());
            newMessage->setMemSize (sm->getMemorySizeKB() / KB);

        } else if (sm->getMemorySizeKB() < 0){

            newMessage->setOperation(SM_MEM_RELEASE);
            newMessage->setByteLength (abs(sm->getMemorySizeKB()));
            newMessage->setMemSize (abs(sm->getMemorySizeKB()) / KB);

        } else {

            newMessage->setOperation(SM_MEM_SEARCH);

        }

        // Copy the control info, if exists!
        if (sm->getControlInfo() != NULL){
            controlOld = check_and_cast<TCPCommand *>(sm->getControlInfo());
            controlNew = new TCPCommand();
            controlNew = controlOld->dup();
            newMessage->setControlInfo (controlNew);
        }

        // Reserve memory to trace!
        newMessage->setTraceArraySize (sm->getTraceArraySize());

        // Copy trace!
        int size = sm->getTraceArraySize();
        for (i=0; i< size; i++){
            newMessage->addNodeTrace(sm->getHostName(i), sm->getNodeTrace(i));
        }

    return (newMessage);
}
