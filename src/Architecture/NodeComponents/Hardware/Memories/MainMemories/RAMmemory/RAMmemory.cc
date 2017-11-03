#include "RAMmemory.h"

Define_Module (RAMmemory);


RAMmemory::~RAMmemory(){
    delete(operationMessage);
}


void RAMmemory::initialize(){

	HWEnergyInterface::initialize();

	std::ostringstream osStream;

	waitingForOperation = false;

	    // Set the moduleIdName
		osStream << "BasicMainMemory." << getId();
		moduleIdName = osStream.str();

	    // Init the super-class
	    icancloud_Base::initialize();

		// Module parameters		
		size_MB = par ("size_MB");
		blockSize_KB = par ("blockSize_KB");
		readLatencyTime_s = par ("readLatencyTime_s");
		writeLatencyTime_s = par ("writeLatencyTime_s");
		searchLatencyTime_s = par ("searchLatencyTime_s");
	    numDRAMChips = par ("numDRAMChips");
		numModules = par ("numModules");

		// Check sizes...
		if ((blockSize_KB<=0) || (size_MB<=0))
			throw cRuntimeError("BasicMainMemory, wrong memory size!!!");

		// Init variables
		totalBlocks = (size_MB*1000) / blockSize_KB;
		totalAppBlocks = 0;
		freeAppBlocks = totalBlocks;

	    // Init the gate IDs to/from Input gates...
		toInputGates = gate ("toInput");
		fromInputGates = gate ("fromInput");


	    // Init the gate IDs to/from Output
	    toOutputGate = gate ("toOutput");
	    fromOutputGate = gate ("fromOutput");

	    // Create the latency message and flush message
	    operationMessage = new cMessage (SM_CHANGE_STATE_MESSAGE.c_str());


	    pendingMessage = NULL;
	    
	    nodeState = MACHINE_STATE_OFF;

	    showStartedModule ("Init Memory: Size:%d bytes.  BlockSize:%d bytes. %d app. blocks",
			    			size_MB*1000*1000,
			    			blockSize_KB*KB,
			    			totalAppBlocks);

}


void RAMmemory::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* RAMmemory::getOutGate (cMessage *msg){

		// If msg arrive from Output
		if (msg->getArrivalGate()==fromOutputGate){
			if (toOutputGate->getNextGate()->isConnected()){
				return (toOutputGate);
			}
		}

		// If msg arrive from Inputs
		else if (msg->arrivedOn("fromInput")){
					return (toInputGates);
		}

	// If gate not found!
	return NULL;
}


void RAMmemory::processCurrentRequestMessage (){

	icancloud_Message *sm;
	cMessage *unqueuedMessage;

		// If exists enqueued requests
		if (!queue.empty()){

			// Pop!
			unqueuedMessage = (cMessage *) queue.pop();

			// Dynamic cast!
			sm = check_and_cast<icancloud_Message *>(unqueuedMessage);
						
			// Process
			processRequestMessage (sm);
		}
}


void RAMmemory::processSelfMessage (cMessage *msg){

	icancloud_App_MEM_Message *sm_mem;
	icancloud_App_IO_Message *sm_io;

	sm_io = dynamic_cast <icancloud_App_IO_Message*> (msg);
	sm_mem = dynamic_cast <icancloud_App_MEM_Message*> (msg);

		// Is a pending message?
		if (!strcmp (msg->getName(), SM_LATENCY_MESSAGE.c_str())){

			// There is a pending message...
			if (pendingMessage!=NULL){

				if ((DEBUG_DETAILED_Basic_Main_Memory) && (DEBUG_Basic_Main_Memory))
					showDebugMessage ("End of memory processing! Pending request:%s ", pendingMessage->contentsToString(DEBUG_MSG_Basic_Main_Memory).c_str());

				// Send current request
				sendRequest (pendingMessage);
			}

			// No pending message waiting to be sent...
			else{

				if ((DEBUG_DETAILED_Basic_Main_Memory) && (DEBUG_Basic_Main_Memory))
					showDebugMessage ("End of memory processing! No pending request.");
				
				// Process next request
				processCurrentRequestMessage ();
			}
			cancelEvent(msg);
		}
		else if (!strcmp (msg->getName(), SM_CHANGE_STATE_MESSAGE.c_str())){
			cancelAndDelete (msg);
			changeState (MEMORY_STATE_IDLE);

		}
		else if (sm_io != NULL) {
		    cancelEvent(msg);
		    changeState (MEMORY_STATE_IDLE);
			sendRequest (sm_io);
		}
		else if (sm_mem != NULL){
		    cancelEvent(msg);
            changeState (MEMORY_STATE_IDLE);
            sendResponseMessage(sm_mem);

		}

		else
			showErrorMessage ("Unknown self message [%s]", msg->getName());


}


void RAMmemory::processRequestMessage (icancloud_Message *sm){

	icancloud_App_IO_Message *sm_io;
	icancloud_App_MEM_Message *sm_mem;
	unsigned int requiredBlocks;
	int operation;

	operation = sm->getOperation();

	// Changing memory state from the node state!
	if (operation == SM_CHANGE_MEMORY_STATE){

		// change the state of the memory
		changeDeviceState (sm->getChangingState().c_str());
		processCurrentRequestMessage();
		delete(sm);

	} else if (operation == SM_CHANGE_DISK_STATE){

		// Cast!
		sm_io = check_and_cast <icancloud_App_IO_Message*> (sm);

		// Send request
		sendRequest (sm_io);

    // Allocating memory for application space!

	}	else if (operation == SM_MEM_ALLOCATE){

	    changeState (MEMORY_STATE_WRITE);
		// Cast!
		sm_mem = check_and_cast <icancloud_App_MEM_Message*> (sm);
		
		// Memory account
		requiredBlocks = (unsigned int) ceil (sm_mem->getMemSize() / blockSize_KB);
				
		if (DEBUG_Basic_Main_Memory)
			showDebugMessage ("Memory Request. Free memory blocks: %d - Requested blocks: %d", freeAppBlocks, requiredBlocks);
		
		if (requiredBlocks <= freeAppBlocks)
			freeAppBlocks -= requiredBlocks;
		else{
			
			showDebugMessage ("Not enough memory!. Free memory blocks: %d - Requested blocks: %d", freeAppBlocks, requiredBlocks);			
			sm_mem->setResult (SM_NOT_ENOUGH_MEMORY);			
		}		
		
		// Response message
		sm_mem->setIsResponse(true);						

		// Time to perform the read operation
		scheduleAt (writeLatencyTime_s+simTime(), sm_mem);

	}
	
	// Releasing memory for application space!
	else if (operation == SM_MEM_RELEASE){

	    changeState (MEMORY_STATE_WRITE);
		// Cast!
		sm_mem = check_and_cast <icancloud_App_MEM_Message*> (sm);
		
		// Memory account
		requiredBlocks = (unsigned int) ceil (sm_mem->getMemSize() / blockSize_KB);		
		
		if (DEBUG_Basic_Main_Memory)
			showDebugMessage ("Memory Request. Free memory blocks: %d - Released blocks: %d", freeAppBlocks, requiredBlocks);
		
		
		// Update number of free blocks
		freeAppBlocks += requiredBlocks;
		if (freeAppBlocks > totalBlocks){
		    freeAppBlocks = totalBlocks;
		}
		
		// Response message
		sm_mem->setIsResponse(true);

		// Time to perform the write operation
		scheduleAt (writeLatencyTime_s+simTime(), sm_mem);

	}
	
	
	// Disk cache space!
	else{

		// Cast!
		sm_io = check_and_cast <icancloud_App_IO_Message*> (sm);

		// Read or write operation?
		if ((operation == SM_READ_FILE) ||
			(operation == SM_WRITE_FILE)){


			// Request came from Service Redirector... Split it and process subRequests!
			if (!sm_io->getRemoteOperation()){

				// Verbose mode? Show detailed request
				if (DEBUG_Basic_Main_Memory)
					showDebugMessage ("Processing request:%s",
									  sm_io->contentsToString(DEBUG_MSG_Basic_Main_Memory).c_str());
				// Search in the banks of the memory and request to the io server
				changeState(MEMORY_STATE_SEARCHING);
				scheduleAt (searchLatencyTime_s+simTime(), sm_io);

			}

			// Request cames from I/O Redirector... send to NFS!
			else
				sendRequest (sm_io);
		}

		// Control operation...
		else{
			sendRequest (sm_io);
		}
	}
}


void RAMmemory::processResponseMessage (icancloud_Message *sm){

	icancloud_App_IO_Message *sm_io;

	// Verbose mode? Show detailed request
	if (DEBUG_Basic_Main_Memory){

		sm_io = check_and_cast <icancloud_App_IO_Message*> (sm);

		showDebugMessage ("Sending response:%s",
					       sm_io->contentsToString(DEBUG_MSG_Basic_Main_Memory).c_str());
	}
	changeState (MEMORY_STATE_IDLE);
	sendResponseMessage (sm);
}


void RAMmemory::sendRequest (icancloud_Message *sm){

	// Send to destination! I/O Redirector...
	if (!sm->getRemoteOperation())
		sendRequestMessage (sm, toOutputGate);

	// Request cames from I/O Redirector... send to corresponding App!
	else{	

		send (sm, toInputGates);
	}
}

void RAMmemory::changeDeviceState (string state,unsigned componentIndex){

	if (strcmp (state.c_str(),MACHINE_STATE_IDLE ) == 0) {

		nodeState = MACHINE_STATE_IDLE;
		changeState (MEMORY_STATE_IDLE);

	} else if (strcmp (state.c_str(),MACHINE_STATE_RUNNING ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		changeState (MEMORY_STATE_IDLE);

	} else if (strcmp (state.c_str(),MACHINE_STATE_OFF ) == 0) {

		nodeState = MACHINE_STATE_OFF;
        totalAppBlocks = 0;
        freeAppBlocks = totalBlocks;
		changeState (MEMORY_STATE_OFF);
	}
}


void RAMmemory::changeState (string energyState,unsigned componentIndex ){

//	if (strcmp (nodeState.c_str(),MACHINE_STATE_OFF ) == 0) {
//		energyState = MEMORY_STATE_OFF;
//	}

//	if (strcmp (energyState.c_str(), MEMORY_STATE_READ) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), MEMORY_STATE_WRITE) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), MEMORY_STATE_IDLE) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), MEMORY_STATE_OFF) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), MEMORY_STATE_SEARCHING) == 0) NULL;
//
// 	else NULL;


	e_changeState (energyState);

}


