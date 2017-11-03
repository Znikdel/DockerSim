#include "StorageManager.h"

Define_Module(StorageManager);


StorageManager::~StorageManager(){
	
	unsigned int i;
	
		SMS_raid_0->clear();
		delete (SMS_raid_0);
		
		// Clear all queues...
		for (i=0; i<numStorageSystems; i++){
			requestsQueue[i].clear();
		}
}


void StorageManager::initialize(){

	unsigned int i;
	std::ostringstream osStream;


		// Set the moduleIdName
		osStream << "StorageSystem." << getId();
		moduleIdName = osStream.str();

		// Init the super-class
		icancloud_Base::initialize();

		// Get the number of Storage servers
		numStorageSystems = par ("numStorageSystems");
		strideSize_b = par ("strideSize_b");

		// State of StorageServers
		isStorageSystemIdle = new bool [numStorageSystems];

		// Init state to idle!
		for (i=0; i<numStorageSystems; i++)
		    isStorageSystemIdle[i] = false;

		// Create requestQueues
		requestsQueue = new cQueue [numStorageSystems];

		// Init the queues
		for (i=0; i<numStorageSystems; i++)
			requestsQueue[i].clear();				
		
		// Check the Volume Manager stride size
		if ((strideSize_b < BYTES_PER_SECTOR) || ((strideSize_b%BYTES_PER_SECTOR) != 0)){
			showErrorMessage ("Wrong Volume Manager strideSize %d. Must be multiple of %d",
								strideSize_b,
								BYTES_PER_SECTOR);
		}	

		// Creates the SMS object
		SMS_raid_0 = new SMS_RAID_0 (strideSize_b, numStorageSystems);

		// Init the gate IDs to/from Scheduler
	    toSchedulerGate = gate ("toScheduler");
	    fromSchedulerGate = gate ("fromScheduler");

	    // Init the gates IDs to/from StorageServers
	    toStorageSystemGates = new cGate* [numStorageSystems];
	    fromStorageSystemGates = new cGate* [numStorageSystems];

	    for (i=0; i<numStorageSystems; i++){
			toStorageSystemGates[i] = gate ("toStorageSystem", i);
			fromStorageSystemGates[i] = gate ("fromStorageSystem", i);
	    }
}


void StorageManager::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* StorageManager::getOutGate (cMessage *msg){

    unsigned int i;

		// If msg arrive from scheduler
		if (msg->getArrivalGate()==fromSchedulerGate){
			if (toSchedulerGate->getNextGate()->isConnected()){
				return (toSchedulerGate);
			}
		}

		// If msg arrive from StorageServers
		else if (msg->arrivedOn("fromStorageSystem")){
			for (i=0; i<numStorageSystems; i++)
				if (msg->arrivedOn ("fromStorageSystem", i))
					return (toStorageSystemGates[i]);
		}

	// If gate not found!
	return NULL;
}


void StorageManager::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void StorageManager::processRequestMessage (icancloud_Message *sm){

	int i;
	unsigned int diskIndex;
	icancloud_BlockList_Message *strippedRequest;
	icancloud_BlockList_Message *sm_bl;

    // Casting to debug!
    sm_bl = check_and_cast<icancloud_BlockList_Message *>(sm);

	if (sm->getOperation() == SM_CHANGE_DISK_STATE){

	    for (i = 0; i < sm_bl->get_component_to_change_size(); i++){

	        // Get the disk to operate
	            diskIndex = sm_bl->get_component_to_change(i);

	        // Creates the message
	            icancloud_BlockList_Message* sm_io = new icancloud_BlockList_Message ();
                sm_io->setOperation (SM_CHANGE_DISK_STATE);

            // Set the corresponding parameters
                string state = sm_bl->getChangingState();
                sm_io->setChangingState(state);
                sm_io->add_component_index_To_change_state(diskIndex);

            // Send the message
                sendRequestMessage (sm_io, toStorageSystemGates[diskIndex]);

            // Update the state
                if (strcmp (state.c_str(), DISK_OFF) == 0)
                    isStorageSystemIdle[diskIndex] = false;
                else
                    isStorageSystemIdle[diskIndex] = true;
	    }
	    delete (sm);
	}
	else{


		// Splits incomming request!
		SMS_raid_0->splitRequest (sm);

		// Debug
		if (DEBUG_Storage_Manager)
			showDebugMessage ("Processing original request:%s from message:%s", 
							SMS_raid_0->requestToString(sm, DEBUG_SMS_Storage_Manager).c_str(),
							sm_bl->contentsToString(DEBUG_BRANCHES_Storage_Manager, DEBUG_MSG_Storage_Manager).c_str());
					
					
		// Show SMS contents?
		showSMSContents(DEBUG_ALL_SMS_Storage_Manager);

		// Enqueue stripped requests!
		for (i=0; i<SMS_raid_0->getNumberOfSubRequest(sm); i++){

			// Get subRequest i
			strippedRequest = check_and_cast<icancloud_BlockList_Message *>(SMS_raid_0->popSubRequest (sm, i));
									
			// Get the StorageServer index!
			diskIndex = strippedRequest->getNextModuleIndex();			

			// Check the disk index. If correct then enqueue current request!
			if ((diskIndex >= numStorageSystems) || (diskIndex < 0))
			
				showErrorMessage ("(Request) Disk index error (%d). There are %d disks attached!\n",
									diskIndex,
									numStorageSystems);
			else
				requestsQueue[diskIndex].insert (strippedRequest);
			
			
			if (DEBUG_Storage_Manager)
				showDebugMessage ("Queuing stripped request to queue[%d] %s", 
									diskIndex,
									strippedRequest->contentsToString(DEBUG_BRANCHES_Storage_Manager, DEBUG_MSG_Storage_Manager).c_str());
		}

		// Check for idle buses...
		checkBuses ();
	}
}


void StorageManager::processResponseMessage (icancloud_Message *sm){

	unsigned int diskIndex;
	icancloud_BlockList_Message *requestMsg;
	icancloud_Message *nextSubRequest;
	cMessage *parentRequest;
	cMessage* unqueuedMessage;


		// Update bus state!
		diskIndex = sm->getNextModuleIndex();

		if ((diskIndex >= numStorageSystems) || (diskIndex < 0))
			showErrorMessage ("(Response) Disk index error (%d). There are %d disks attached!\n",
								diskIndex,
								numStorageSystems);
		else
		    isStorageSystemIdle[diskIndex] = true;

		// Parent request
		parentRequest = sm->getParentRequest();

		// SubRequest arrives...
		SMS_raid_0->arrivesSubRequest (sm, parentRequest);

		// Casting...
		requestMsg = check_and_cast <icancloud_BlockList_Message*> (parentRequest);

		// Debug
		if (DEBUG_Storage_Manager)
			showDebugMessage ("Processing request response:%s from message:%s", 
							SMS_raid_0->requestToString(requestMsg, DEBUG_SMS_Storage_Manager).c_str(),
							requestMsg->contentsToString(DEBUG_BRANCHES_Storage_Manager, DEBUG_MSG_Storage_Manager).c_str());
								
		
		// If all request have arrived... erase request and send back the response!
		if (SMS_raid_0->arrivesAllSubRequest (parentRequest)){
					
			if (DEBUG_Storage_Manager)
				showDebugMessage ("Arrives all stripped request. %s", SMS_raid_0->requestToString(parentRequest, DEBUG_SMS_Storage_Manager).c_str());

			// Removes the request object!
			SMS_raid_0->removeRequest(parentRequest);

			// Show SMS contents?
			showSMSContents(DEBUG_ALL_SMS_Storage_Manager);

			// Now is a Response Message
			requestMsg->setIsResponse (true);

			// Update the mesage length!
			requestMsg->updateLength();

			// Sends the message
			sendResponseMessage (requestMsg);
		}

		// Check if there is pending request on queue i
		if (!requestsQueue[diskIndex].empty()){

			// Pop!
			unqueuedMessage = (cMessage *) requestsQueue[diskIndex].pop();

			// Dynamic cast!
			nextSubRequest = check_and_cast<icancloud_Message *>(unqueuedMessage);

			// Update state!
			isStorageSystemIdle[diskIndex] = false;

			// Send!
			sendRequestMessage (nextSubRequest, toStorageSystemGates[diskIndex]);
		}
}


void StorageManager::checkBuses (){

	unsigned int i;
	icancloud_Message* sm;
	cMessage* unqueuedMessage;

		// Check for idle buses
		for (i=0; i<numStorageSystems; i++){

			// If bus[i] is idle, send next subRequest!
			if (isStorageSystemIdle[i]){

				// Check if there is pending request on queue i
				if (!requestsQueue[i].empty()){

					// Pop!
					unqueuedMessage = (cMessage *) requestsQueue[i].pop();

					// Dynamic cast!
					sm = check_and_cast<icancloud_Message *>(unqueuedMessage);

					// Update state!
					isStorageSystemIdle[i]=false;

					// Send!
					sendRequestMessage (sm, toStorageSystemGates[i]);
				}
			}
		}
}


void StorageManager::showSMSContents (bool showContents){

	int i;
	int numRequests;

		// Get the number of requests
		numRequests = SMS_raid_0->getNumberOfRequests();
				
		if (DEBUG_Storage_Manager)
			showDebugMessage ("Showing the complete SMS vector... (%d requests)", numRequests);

		// Get all requests
		for (i=0; i<numRequests; i++)			
				if (DEBUG_Storage_Manager)
					showDebugMessage (" Request[%d]:%s ", i, SMS_raid_0->requestToStringByIndex(i).c_str());
}


