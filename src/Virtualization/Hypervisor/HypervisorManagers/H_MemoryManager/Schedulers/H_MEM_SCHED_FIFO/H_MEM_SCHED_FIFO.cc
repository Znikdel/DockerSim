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

#include "H_MEM_SCHED_FIFO.h"

Define_Module(H_MEM_SCHED_FIFO);

H_MEM_SCHED_FIFO::~H_MEM_SCHED_FIFO() {
}

void H_MEM_SCHED_FIFO::initialize(){

    H_MemoryManager_Base::initialize();

    memoryCells.clear();
    totalMemory_Blocks = requestedSizeToBlocks((int)memorySize_MB * 1024);       // memory in KB
    memoryAvailable_Blocks = totalMemory_Blocks ;

    memoryCell* cell = new memoryCell();
    cell->uId = 0;
    cell->pId = 0;
    cell->vmGate = nodeGate;
    cell->vmTotalBlocks_KB = totalMemory_Blocks;
    cell->remainingBlocks_KB = totalMemory_Blocks;

    memoryCells.push_back(cell);

}

void H_MEM_SCHED_FIFO::finish(){
    H_MemoryManager_Base::finish();

}

void H_MEM_SCHED_FIFO::schedulingMemory(icancloud_Message *msg){

    memoryCell* cell;
    bool found = false;
    int operation = msg->getOperation();
    int requiredBlocks;
    icancloud_App_MEM_Message* sm_mem;

    int temporalBlocksQuantity = 0;

    // The operation come from the physical resources
        if (msg->arrivedOn("fromNodeMemoryO") || msg->arrivedOn("fromNodeMemoryI"))

            sendMemoryMessage(msg);

        // The operation is a remote operation. So it will go to the OS
        else if (msg->getRemoteOperation()){

            sendRequestMessage(msg, toVMMemoryI->getGate(nodeGate));

        }
        else if (operation == SM_MEM_ALLOCATE) {

            sm_mem = dynamic_cast <icancloud_App_MEM_Message*> (msg);

            // Memory account
            requiredBlocks = requestedSizeToBlocks(sm_mem->getMemSize());

            // Get the memory cell

            for (int i = 0; (i < (int)memoryCells.size()) && (!found); i++){
                cell = (*(memoryCells.begin() + i));
                if ((cell->uId == msg->getUid()) && (cell->pId == msg->getPid())){
                    found = true;
                    temporalBlocksQuantity = cell->remainingBlocks_KB - requiredBlocks;
                    if (cell->remainingBlocks_KB < 0) cell->remainingBlocks_KB = 0;
                }
            }

            if (!found) throw cRuntimeError ("H_MEM_SCHED_FIFO::schedulingMemory->the user:%i with vm id:%i not found..\n",msg->getUid(), msg->getPid());

            if (temporalBlocksQuantity <= 0){ // Not enough memory

                showDebugMessage ("Not enough memory!. Free memory blocks: %d - Requested blocks: %d",  cell->remainingBlocks_KB, requiredBlocks);
                // Cast!
                sm_mem->setResult (SM_NOT_ENOUGH_MEMORY);

                // Response message
                sm_mem->setIsResponse(true);

                // Send message back!
                sendResponseMessage (sm_mem);

            }else if (temporalBlocksQuantity >= 0){  // Decrement the memory in the Hypervisor

                cell->remainingBlocks_KB = temporalBlocksQuantity;
                sendMemoryMessage(sm_mem);

            } else { // The size is 0!

                sendRequestMessage(msg, toVMMemoryO->getGate(msg->getArrivalGate()->getIndex()));
            }

        } else if (operation == SM_MEM_RELEASE) {

            sm_mem = dynamic_cast <icancloud_App_MEM_Message*> (msg);

            requiredBlocks = requestedSizeToBlocks(sm_mem->getMemSize());

            for (int i = 0; (i < (int)memoryCells.size()) && (!found); i++){
                cell = (*(memoryCells.begin() + i));
                if ((cell->uId == msg->getUid()) && (cell->pId) == (cell->pId)){
                    found = true;
                    cell->remainingBlocks_KB += requiredBlocks;
                    if (cell->remainingBlocks_KB > cell->vmTotalBlocks_KB) cell->remainingBlocks_KB = cell->vmTotalBlocks_KB;
                    sendMemoryMessage(sm_mem);
                }
            }

            if (!found) throw cRuntimeError ("H_MEM_SCHED_FIFO::schedulingMemory->the user%i with vm id:%i not found..\n",msg->getUid(), msg->getPid());

        }

        else{

            //Get the gate and the index of the arrival msg
            sendMemoryMessage(msg);
        }

}


int H_MEM_SCHED_FIFO::setVM (cGate* oGateI, cGate* oGateO, cGate* iGateI, cGate* iGateO, int uId, int pId, int requestedMemory_KB){

    int gateidx = H_MemoryManager_Base::setVM(oGateI, oGateO, iGateI, iGateO, requestedMemory_KB, uId, pId);

    memoryCell* cell = new memoryCell();
    cell->uId = uId;
    cell->pId = pId;
    cell->vmGate = gateidx;
    cell->vmTotalBlocks_KB = requestedSizeToBlocks(requestedMemory_KB);
    cell->remainingBlocks_KB = cell->vmTotalBlocks_KB;
    memoryCells.push_back(cell);

    memoryAvailable_Blocks -= cell->vmTotalBlocks_KB;

    return 0;

}

void H_MEM_SCHED_FIFO::freeVM(int uId, int pId){

    memoryCell* cell;
    bool found = false;

    for (int i = 0; (i < (int)memoryCells.size()) && (!found); i++){
        cell = (*(memoryCells.begin() + i));
        if ((cell->uId == uId) && ((cell->pId) == pId)){
            found = true;
            memoryAvailable_Blocks += (cell->vmTotalBlocks_KB);
            memoryCells.erase(memoryCells.begin() + i);
        }
    }

    H_MemoryManager_Base::freeVM(uId, pId);

}

int H_MEM_SCHED_FIFO::getVMGateIdx(int uId, int pId){

    memoryCell* cell;
    bool found = false;
    int gateIdx = -1;

    for (int i = 0; (i < (int)memoryCells.size()) && (!found); i++){
        cell = (*(memoryCells.begin() + i));
        if ((cell->uId == uId) && ((cell->pId) == pId)){
            found = true;
            gateIdx = cell->vmGate;

        }
    }

    return gateIdx;
}

void H_MEM_SCHED_FIFO::printCells(string methodName){
    memoryCell* cell;

    for (int i = 0; (i < (int)memoryCells.size()); i++){
        cell = (*(memoryCells.begin() + i));

        printf("H_MEM_SCHED_FIFO::printCells [%s] -->cell[%i] - uId-%i pId-%i gate-%i TotalBlocks-%i remainingBlocks-%i\n",methodName.c_str(), i, cell->uId, cell->pId, cell->vmGate, cell->vmTotalBlocks_KB, cell->remainingBlocks_KB);

    }
}

double H_MEM_SCHED_FIFO::getVMMemoryOccupation_MB(int uId, int pId){

    memoryCell* cell;
    bool found = false;
    int size = -1;

    for (int i = 0; (i < (int)memoryCells.size()) && (!found); i++){
        cell = (*(memoryCells.begin() + i));
        if ((cell->uId == uId) && ((cell->pId) == pId)){
            found = true;
            size =  (((cell->vmTotalBlocks_KB - cell->remainingBlocks_KB) * blockSize_KB) / 1024);

        }
    }

    return size;
}
