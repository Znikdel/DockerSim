#include "RAMMemory_BlockModel.h"

Define_Module (RAMMemory_BlockModel);


RAMMemory_BlockModel::~RAMMemory_BlockModel(){
	
	cMessage *message;	

    	// Cancel the flush message
		cancelAndDelete (flushMessage);
		
		// Removes all messages placed on SMS
		SMS_memory->clear();
		
		// Removes the SMS object
		delete (SMS_memory);
		
		while (!memoryBlockList.empty()){
			delete (memoryBlockList.front());
			memoryBlockList.pop_front();
		}
		
		// Renoves all messages in memory list
		memoryBlockList.clear();
		
		// Removes all messages in flushQueue
		while (!flushQueue.empty()){			
			message = (cMessage *) flushQueue.pop();
			delete (message);			
		}
		
		flushQueue.clear();	

}


void RAMMemory_BlockModel::initialize(){

    HWEnergyInterface::initialize();

	std::ostringstream osStream;

	    // Set the moduleIdName
		osStream << "MainMemory." << getId();
		moduleIdName = osStream.str();

        // Init the super-class
        icancloud_Base::initialize();

        waitingForOperation = false;

		// Module parameters		
		readAheadBlocks = par ("readAheadBlocks");
		size_KB = (par ("size_MB").longValue()) * 1024;
		sizeCache_KB = par ("sizeCache_KB");
		blockSize_KB = par ("blockSize_KB");
		readLatencyTime_s = par ("readLatencyTime_s");
		writeLatencyTime_s = par ("writeLatencyTime_s");
		flushTime_s = par ("flushTime_s");
        searchLatencyTime_s = par ("searchLatencyTime_s");

        numDRAMChips = par ("numDRAMChips");
        numModules = par ("numModules");

		// Check sizes...
		if ((blockSize_KB<=0) || (sizeCache_KB<=0) || (size_KB<=0) || (readAheadBlocks<0))
			throw new cRuntimeError("MainMemory, wrong memory size or blocks sizes!!!");		
			
		if ((size_KB%blockSize_KB) != 0)
			throw new cRuntimeError("MainMemory_CacheBlockLatencies, blockSize_KB must be multiple of size_KB!!!");		

		if (sizeCache_KB > size_KB)
			throw new cRuntimeError("Cache size cannot be larger than the total memory size!!!");	

		// Init variables
		totalBlocks = size_KB / blockSize_KB;
		totalCacheBlocks = sizeCache_KB / blockSize_KB;
		totalAppBlocks = totalBlocks - totalCacheBlocks;
		freeAppBlocks = totalAppBlocks;

	    // Init the gate IDs to/from Input gates...
			toInputGates = gate ("toInput");
			fromInputGates = gate ("fromInput");


	    // Init the gate IDs to/from Output
	    toOutputGate = gate ("toOutput");
	    fromOutputGate = gate ("fromOutput");

	    // Create the latency message and flush message
	    latencyMessage = new cMessage (SM_LATENCY_MESSAGE.c_str());
	    flushMessage = new cMessage ("flush-message");
	    pendingMessage = NULL;

	    // Creates the SMS object
	    SMS_memory = new SMS_MainMemory (size_KB*KB, readAheadBlocks, blockSize_KB*KB);

	    // Flush Queue
	    flushQueue.clear();
	    
        nodeState = MACHINE_STATE_OFF;

	    showStartedModule ("Init Memory: Size:%d bytes.  BlockSize:%d bytes.  Read-ahead:%d blocks - %d cache blocks - %d app. blocks",
			    			size_KB*KB,
			    			blockSize_KB*KB,
			    			readAheadBlocks,
			    			totalCacheBlocks,
			    			totalAppBlocks);
}


void RAMMemory_BlockModel::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* RAMMemory_BlockModel::getOutGate (cMessage *msg){

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



void RAMMemory_BlockModel::processCurrentRequestMessage (){

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
		else
			processSubRequests();
}


void RAMMemory_BlockModel::processSelfMessage (cMessage *msg){
    icancloud_App_MEM_Message *sm_mem;
    icancloud_App_IO_Message *sm_io;

    sm_io = dynamic_cast <icancloud_App_IO_Message*> (msg);
    sm_mem = dynamic_cast <icancloud_App_MEM_Message*> (msg);

		// Is a pending message?
		if (!strcmp (msg->getName(), SM_LATENCY_MESSAGE.c_str())){

			// There is a pending message...
			if (pendingMessage!=NULL){

				if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
					showDebugMessage ("End of memory processing! Pendign request:%s ", pendingMessage->contentsToString(DEBUG_MSG_Main_Memory).c_str());

				cancelEvent (msg);

				sendRequest (pendingMessage);
			}

			// No pending message waiting to be sent...
			else{

				if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
					showDebugMessage ("End of memory processing! No pending request.");
				
				
				cancelEvent (msg);

				// Process next subRequest...
				processCurrentRequestMessage ();
				//processSubRequests ();
			}
		}

		// Is a flush message?
		else if (!strcmp (msg->getName(), "flush-message")){

			// There is no blocks on flush queue... stop the timer!
			if (!flushQueue.empty())
				flushMemory ();
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
            if (strcmp(e_getActualState().c_str(), MEMORY_STATE_WRITE) == 0){
                cancelEvent(msg);
                changeState (MEMORY_STATE_IDLE);
                sendResponseMessage(sm_mem);
            } else if (strcmp(e_getActualState().c_str(), MEMORY_STATE_SEARCHING) == 0){
                cancelEvent(msg);
                changeState (MEMORY_STATE_READ);
                // delete msg?
                processSubRequests();
            }



        }

        else
            showErrorMessage ("Unknown self message [%s]", msg->getName());



}


void RAMMemory_BlockModel::processRequestMessage (icancloud_Message *sm){

	icancloud_App_IO_Message *sm_io;
	icancloud_App_MEM_Message *sm_mem;
	unsigned int requiredBlocks;
	int operation = sm->getOperation();
	
	// Changing energy state operations came from OS
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
    } else if (sm->getOperation() == SM_MEM_ALLOCATE){

        changeState (MEMORY_STATE_WRITE);

		// Cast!
		sm_mem = check_and_cast <icancloud_App_MEM_Message*> (sm);
		
		// Memory account
		requiredBlocks = (unsigned int) ceil (sm_mem->getMemSize() / blockSize_KB);
				
		if (DEBUG_Main_Memory)
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
	else if (sm->getOperation() == SM_MEM_RELEASE){
				
	    changeState (MEMORY_STATE_WRITE);
		// Cast!
		sm_mem = check_and_cast <icancloud_App_MEM_Message*> (sm);
		
		// Memory account
		requiredBlocks = (unsigned int) ceil (sm_mem->getMemSize() / blockSize_KB);		
		
		if (DEBUG_Main_Memory)
			showDebugMessage ("Memory Request. Free memory blocks: %d - Released blocks: %d", freeAppBlocks, requiredBlocks);
		
		
		// Update number of free blocks
		freeAppBlocks += requiredBlocks;				
		
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
		if ((sm_io->getOperation() == SM_READ_FILE) ||
			(sm_io->getOperation() == SM_WRITE_FILE)){

			// Request cames from Service Redirector... Split it and process subRequests!
			if (!sm_io->getRemoteOperation()){
								
				// Split and process current request!
				SMS_memory->splitRequest(sm_io);

				// Verbose mode? Show detailed request
				if (DEBUG_Main_Memory)
					showDebugMessage ("Processing original request:%s from message:%s", 
									SMS_memory->requestToString(sm_io, DEBUG_SMS_Main_Memory).c_str(),
									sm_io->contentsToString(DEBUG_MSG_Main_Memory).c_str());												


				showSMSContents(DEBUG_ALL_SMS_Main_Memory);

                changeState(MEMORY_STATE_SEARCHING);
                scheduleAt (searchLatencyTime_s+simTime(), sm_io);

			}

			// Request cames from I/O Redirector... send to corresponding App (NFS)!
			else
				sendRequest (sm_io);
		}

		// Control operation...
		else{
			sendRequest (sm_io);
		}
	}
}


void RAMMemory_BlockModel::processResponseMessage (icancloud_Message *sm){

	icancloud_App_IO_Message *sm_io;
	icancloud_MemoryBlock* currentBlockCached;


		// Response cames from remote node...
		if (sm->arrivedOn("fromInput")){
			sendResponseMessage (sm);
		}

		// Response cames from I/O Redirector... (local)
		else{

			// Read or Write operation?
			if ((sm->getOperation() == SM_READ_FILE) ||
				(sm->getOperation() == SM_WRITE_FILE)){

				// Cast!
				sm_io = check_and_cast <icancloud_App_IO_Message*> (sm);

				// Update memory...
				currentBlockCached = searchMemoryBlock (sm_io->getFileName(), sm_io->getOffset());

				// If block in memory... set current block as NOT pending and re-insert on memory!
				if (currentBlockCached!=NULL){
					currentBlockCached->setIsPending (false);
					reInsertBlock (currentBlockCached);
				}

				// Arrives subRequest!
				arrivesSubRequest (sm_io);

				// Show memory contents?
				if ((DEBUG_SHOW_CONTENTS_Main_Memory) && (DEBUG_Main_Memory))
					showDebugMessage ("Arrives block response. Memory contents: %s ", memoryListToString().c_str());
			}

			// Control operation?
			else
				sendResponseMessage (sm);
		}
}


void RAMMemory_BlockModel::sendRequest (icancloud_Message *sm){

	// Send to destination! I/O Redirector...
	if (!sm->getRemoteOperation())
		sendRequestMessage (sm, toOutputGate);

	// Request cames from I/O Redirector... send to corresponding App!
	else{	
		sendRequestMessage (sm, toInputGates);
	}
}


void RAMMemory_BlockModel::sendRequestWithoutCheck (icancloud_Message *sm){

	// If trace is empty, add current hostName, module and request number
	if (sm->isTraceEmpty()){
		sm->addNodeToTrace (getHostName());
		updateMessageTrace (sm);
	}

	// Send to destination! Probably a file system...
	if (!sm->getRemoteOperation()){
		send (sm, toOutputGate);
	}

	// Request cames from IOR... send to corresponding App!
	else{	
		send (sm, toInputGates);
	}
}


void RAMMemory_BlockModel::processSubRequests (){

	icancloud_App_IO_Message *sm_io;				// Request message!
	icancloud_App_IO_Message *sm_io_copy;			// Request message! (copy)

	bool is_memoryFull;							// Is the memory full?
	bool all_blocksPending;						// Are all cached blocks pending?
	icancloud_MemoryBlock* currentBlockCached;		// Is current block in memory?
	string isCached;							// Is block cached? in string format
	bool isMemorySaturated;						// Is cache saturated?
	bool currentSubReqIsPending;
	
		// Currently processing memory?
		if (latencyMessage->isScheduled()){

			if (DEBUG_Main_Memory)
				showDebugMessage ("Currently processing memory... current subRequest must wait!");

			return;
		}

		pendingMessage=NULL;

		// Get the first subrequest!
		sm_io = SMS_memory->getFirstSubRequest();

		// There is, at least, one subRequest...
		if (sm_io != NULL){

			// Check if current block is in memory!
			currentBlockCached = searchMemoryBlock (sm_io->getFileName(),
												   sm_io->getOffset());

			// Is the block cached? parse to string...
			if (currentBlockCached==NULL)
				isCached="false";
			else
				isCached="true";

			// Calculates is memory is full!
			is_memoryFull = isMemoryFull();

			// All blocks are pending?
			all_blocksPending = allBlocksPending();

			// Conditions to saturate memory...
			isMemorySaturated = ((is_memoryFull) && (all_blocksPending) && (currentBlockCached==NULL));

			
			if (DEBUG_Main_Memory)
				showDebugMessage ("Processing subRequest: Op:%s File:%s Offset:%u Size:%u",
								sm_io->operationToString().c_str(),
								sm_io->getFileName(),
								sm_io->getOffset(),
								sm_io->getSize());
			
			if (DEBUG_Main_Memory)
				showDebugMessage ("Status: blockCached?:%s memoryFull?:%s allPending?:%s -> saturated?:%s",
								isCached.c_str(),
								boolToString(is_memoryFull).c_str(),
								boolToString(all_blocksPending).c_str(),
								boolToString(isMemorySaturated).c_str());


			// Memory is saturated!!! Send directly the request!!!
			if (isMemorySaturated){
					
				if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
					showDebugMessage ("Memory is saturated!!! Sending subRequest directly to destination...");

				// Pop subrequest!
				sm_io = SMS_memory->popSubRequest();

				// Send..
				sendRequest (sm_io);

		        changeState (MEMORY_STATE_IDLE);
			}

			// Memory not saturated... using memory!!!
			else{

				 	// Pop subrequest!
					sm_io = SMS_memory->popSubRequest();
								
					// Is a READ operation?
					if (sm_io->getOperation() == SM_READ_FILE){

						// Current block is cached!
						if (currentBlockCached!=NULL){

							// If not pending, re-insert the block!
							if (!currentBlockCached->getIsPending())
								reInsertBlock (currentBlockCached);

							if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
								showDebugMessage ("Processing memory... block cached! (latencyTime_s). Request:%s",
													sm_io->contentsToString(DEBUG_MSG_Main_Memory).c_str());

							// Latency time!
							scheduleAt (readLatencyTime_s+simTime(), latencyMessage);

							// Send back as solved request!
							sm_io->setIsResponse(true);
							arrivesSubRequest (sm_io);
						}

						// Current block is not cached!
						else{

							// If memory full, remove a block!
							if (is_memoryFull)
								memoryBlockList.pop_back();

							// Insert current block in memory!
							insertBlock (sm_io->getFileName(), sm_io->getOffset(), sm_io->getSize());

							// Send request...
							pendingMessage = sm_io;

							if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
								showDebugMessage ("Processing memory... block NOT cached! (latencyTime_s). Request:%s",
													sm_io->contentsToString(DEBUG_MSG_Main_Memory).c_str());

							scheduleAt (readLatencyTime_s+simTime(), latencyMessage);

						}
					}

					// Is a WRITE operation?
					else{
												
						// Current block is cached!
						if (currentBlockCached!=NULL){

							// Current block is NOT pending!
							if (!currentBlockCached->getIsPending()){
								
								if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
									showDebugMessage ("Processing memory... block cached! (latencyTime_s). Request:%s",
														sm_io->contentsToString(DEBUG_MSG_Main_Memory).c_str());

								// Mark current block as pending!
								currentBlockCached->setIsPending(true);

								// Re insert current block...
								reInsertBlock (currentBlockCached);

								// enQueue on Flush Queue!
								insertRequestOnFlushQueue (sm_io);
								
								currentSubReqIsPending = false;
							}
							
							// Current block is pending...
							else{
								currentSubReqIsPending = true;								
							}
						}

						// not in memory!
						else{
							
							if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
								showDebugMessage ("Processing memory... block NOT cached:%s",
													sm_io->contentsToString(DEBUG_MSG_Main_Memory).c_str());
							
							currentSubReqIsPending = false;

							// If memory full, remove a block!
							if (is_memoryFull)
								memoryBlockList.pop_back();

							// Insert on memory
							insertBlock (sm_io->getFileName(), sm_io->getOffset(), sm_io->getSize());

							// enQueue on Flush Queue!
							insertRequestOnFlushQueue (sm_io);
						}

							// Copy request to send response to SMS!
							sm_io_copy = (icancloud_App_IO_Message*) sm_io->dup();
							sm_io_copy->setIsResponse(true);
							
							scheduleAt (writeLatencyTime_s+simTime(), latencyMessage);
							arrivesSubRequest (sm_io_copy);
							
							if (currentSubReqIsPending)
								delete (sm_io);
								
								
							// Flush!
							if (flushTime_s == 0)
								flushMemory ();
								
					}

					// Show memory contents?
					if ((DEBUG_SHOW_CONTENTS_Main_Memory) && (DEBUG_Main_Memory))
						showDebugMessage (" %s ", memoryListToString().c_str());
			}
		}
}


void RAMMemory_BlockModel::arrivesSubRequest (icancloud_App_IO_Message *subRequest){

	cMessage *auxRequest;
	icancloud_App_IO_Message *parentRequest;
	bool isRequestHere;

		// Parent request		
		auxRequest = subRequest->getParentRequest();		

		// Search for the request on request vector!
		isRequestHere = (SMS_memory->searchRequest (auxRequest) != NOT_FOUND);

		// If request is not here... delete current subRequest!
		if (!isRequestHere){

			if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
				showDebugMessage ("Arrived subRequest has no parent.. deleting! %s", subRequest->contentsToString(DEBUG_MSG_Main_Memory).c_str());

			delete (subRequest);
		}

		else{

			// Casting!
			parentRequest = check_and_cast <icancloud_App_IO_Message*> (auxRequest);

			// SubRequest arrives...
			SMS_memory->arrivesSubRequest (subRequest, parentRequest);
			
			// Check for errors...
			if (subRequest->getResult() != icancloud_OK){
				parentRequest->setResult (subRequest->getResult());
			}			

			// Verbose mode? Show detailed request
			if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
				showDebugMessage ("Processing request response:%s from message:%s", 
									SMS_memory->requestToString(parentRequest, DEBUG_SMS_Main_Memory).c_str(),
									subRequest->contentsToString(DEBUG_MSG_Main_Memory).c_str());		
			
			
			// If all request have arrived...
			if ((SMS_memory->arrivesAllSubRequest(parentRequest)) ||
				(SMS_memory->arrivesRequiredBlocks(parentRequest, readAheadBlocks))){
							
				if ((DEBUG_DETAILED_Main_Memory) && (DEBUG_Main_Memory))
					showDebugMessage ("Arrives all subRequest!");

				// Removes the request object!
				SMS_memory->removeRequest (parentRequest);

				// Show complete SMS
				showSMSContents (DEBUG_SHOW_CONTENTS_Main_Memory);
				
				// Now is a Response Message
				parentRequest->setIsResponse (true);

				// Update the mesage length!
				parentRequest->updateLength();

				// Send response
				sendResponseMessage (parentRequest);
			}
		}
}


void RAMMemory_BlockModel::flushMemory (){

 cMessage *unqueuedMessage;

	if (simTime() >= 2999)
		endSimulation();

	// If there is elements inside the queue
	while (flushQueue.length()>0){

		// pop next element...
		unqueuedMessage = (cMessage *) flushQueue.pop();
		icancloud_App_IO_Message *sm_io = check_and_cast<icancloud_App_IO_Message *>(unqueuedMessage);
		sendRequestWithoutCheck (sm_io);
	}
}


void RAMMemory_BlockModel::insertRequestOnFlushQueue (icancloud_App_IO_Message *subRequest){

	// Insert a block on flush queue
	flushQueue.insert (subRequest);

	// If memory Full!! Must force the flush...
	if (isMemoryFull()){

		// If there was activated a previous timer, then cancel it!
		if (flushMessage->isScheduled())
			cancelEvent (flushMessage);

		// Forced flush!
		flushMemory();
	}

	// If timer is not active... activate it!
	else if (!flushMessage->isScheduled()){
		scheduleAt (flushTime_s+simTime(), flushMessage);
	}
}


bool RAMMemory_BlockModel::isMemoryFull (){

	if (memoryBlockList.size() >= totalCacheBlocks)
		return true;
	else
		return false;
}


bool RAMMemory_BlockModel::allBlocksPending (){

	list <icancloud_MemoryBlock*>::iterator listIterator;
	bool allPending;

		// Init
		allPending = true;

		// Is empty?
		if (memoryBlockList.empty())
			allPending = false;

		// Memory not empty!
		else{

			// Walk through the list searching the requested block!
			for (listIterator=memoryBlockList.begin(); (listIterator!=memoryBlockList.end() && (allPending)); ++listIterator){

				if (!(*listIterator)->getIsPending())
					allPending = false;
			}
		}

	return (allPending);
}


void RAMMemory_BlockModel::insertBlock (string fileName, unsigned int offset, unsigned int size){

	icancloud_MemoryBlock *block;
	int blockOffset;
	int blockNumber;


		// Bigger than memory block size...
		if (size > (blockSize_KB*KB))
			throw new cRuntimeError("[searchMemoryBlock] memory Block size too big!!!");

		// Block number and Block offset
		blockNumber = offset/(blockSize_KB*KB);
		blockOffset = blockNumber*(blockSize_KB*KB);

		if ((offset+size)>((blockNumber+1)*(blockSize_KB*KB))){
			throw new cRuntimeError("[searchMemoryBlock] Wrong memory Block size (out of bounds)!!!");
		}

		block = new icancloud_MemoryBlock();

		// Set corresponding values
		block->setFileName (fileName);
		block->setOffset (blockOffset);
		block->setBlockSize (blockSize_KB*KB);
		block->setIsPending (true);

		// Insert block on memory list!
		memoryBlockList.push_front (block);
}


void RAMMemory_BlockModel::reInsertBlock (icancloud_MemoryBlock *block){

	list <icancloud_MemoryBlock*>::iterator listIterator;
	icancloud_MemoryBlock* memoryBlock;
	bool found;


		// Init
		memoryBlock = NULL;
		found = false;

		// Walk through the list searching the requested block!
		for (listIterator=memoryBlockList.begin();
			(listIterator!=memoryBlockList.end() && (!found));
			++listIterator){

			// Found?
			if ((block->getFileName() == (*listIterator)->getFileName()) &&
				(block->getOffset() == (*listIterator)->getOffset() )){
				found = true;
				memoryBlock = *listIterator;
				memoryBlockList.erase (listIterator);
				memoryBlockList.push_front (memoryBlock);
			}
		}
}


icancloud_MemoryBlock* RAMMemory_BlockModel::searchMemoryBlock (const char* fileName, unsigned int offset){

	list <icancloud_MemoryBlock*>::iterator listIterator;
	icancloud_MemoryBlock* memoryBlock;
	bool found;
	int currentBlock, requestedBlock;


		// Init
		memoryBlock = NULL;
		found = false;
		requestedBlock = offset/(blockSize_KB*KB);

		// Walk through the list searching the requested block!
		for (listIterator=memoryBlockList.begin(); ((listIterator!=memoryBlockList.end()) && (!found)); listIterator++){

			currentBlock = 	((*listIterator)->getOffset())/(blockSize_KB*KB);

			if ((!strcmp (fileName, (*listIterator)->getFileName().c_str()) ) &&
				(requestedBlock==currentBlock)){
				found = true;
				memoryBlock = *listIterator;
			}
		}


	return (memoryBlock);
}


string RAMMemory_BlockModel::memoryListToString (){

	list <icancloud_MemoryBlock*>::iterator listIterator;
	std::ostringstream info;
	int currentBlock;


		// Init
		currentBlock = 0;
		info << "Memory list..." << endl;

			// Walk through the list searching the requested block!
			for (listIterator=memoryBlockList.begin();
				listIterator!=memoryBlockList.end();
				listIterator++){

					info << "Block[" << currentBlock << "]: " << (*listIterator)->memoryBlockToString() << endl;
					currentBlock++;
			}

	return info.str();
}


void RAMMemory_BlockModel::showSMSContents (bool showContents){

	int i;
	int numRequests;
	
		if (showContents){

			// Get the number of requests
			numRequests = SMS_memory->getNumberOfRequests();
			
			showDebugMessage ("Showing the complete SMS vector... (%d requests)", numRequests);
	
			// Get all requests
			for (i=0; i<numRequests; i++)			
				showDebugMessage (" Request[%d]:%s ", i, SMS_memory->requestToStringByIndex(i).c_str());
		}
}

void RAMMemory_BlockModel::changeDeviceState (string state,unsigned componentIndex){

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


void RAMMemory_BlockModel::changeState (string energyState,unsigned componentIndex ){

//  if (strcmp (nodeState.c_str(),MACHINE_STATE_OFF ) == 0) {
//      energyState = MEMORY_STATE_OFF;
//  }

//  if (strcmp (energyState.c_str(), MEMORY_STATE_READ) == 0) NULL;
//
//  else if (strcmp (energyState.c_str(), MEMORY_STATE_WRITE) == 0) NULL;
//
//  else if (strcmp (energyState.c_str(), MEMORY_STATE_IDLE) == 0) NULL;
//
//  else if (strcmp (energyState.c_str(), MEMORY_STATE_OFF) == 0) NULL;
//
//  else if (strcmp (energyState.c_str(), MEMORY_STATE_SEARCHING) == 0) NULL;
//
//  else NULL;


    e_changeState (energyState);

}

