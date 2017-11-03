#include "SimpleDisk.h"

Define_Module (SimpleDisk);


SimpleDisk::~SimpleDisk(){
    cancelAndDelete(latencyMessage);
}


void SimpleDisk::initialize(int stage){

	if (stage != 2){
        IStorageDevice::initialize(stage);
	}
    else
    {
        std::ostringstream osStream;

		// Set the moduleIdName
		osStream << "SimpleDisk." << getId();
		moduleIdName = osStream.str();

		// Init the super-class
		icancloud_Base::initialize();

		// Assign ID to module's gates
		inGate = gate ("in");
		outGate = gate ("out");

		// Get the blockSize
		readBandwidth = par ("readBandwidth");
		writeBandwidth = par ("writeBandwidth");

		// Init variables
		requestTime = 0;

		// Latency message
		latencyMessage = new cMessage (SM_LATENCY_MESSAGE.c_str());
		pendingMessage = NULL;

		nodeState = MACHINE_STATE_OFF;
    }
}


void SimpleDisk::finish(){   

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* SimpleDisk::getOutGate (cMessage *msg){

		// If msg arrive from scheduler
		if (msg->getArrivalGate()==inGate){
			if (outGate->getNextGate()->isConnected()){
				return (outGate);
			}
		}

	// If gate not found!
	return NULL;
}


void SimpleDisk::processSelfMessage (cMessage *msg){

	icancloud_BlockList_Message *sm_bl;


		// Latency message...
		if (!strcmp (msg->getName(), SM_LATENCY_MESSAGE.c_str())){

			// Cast!
			sm_bl = check_and_cast<icancloud_BlockList_Message *>(pendingMessage);

			// Init pending message...
			pendingMessage=NULL;

			// Change disk state to idle
			changeState (DISK_IDLE);

			// Send message back!
			sendResponseMessage (sm_bl);
		}

		else
			showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void SimpleDisk::processRequestMessage (icancloud_Message *sm){

    int operation;


        operation= sm->getOperation();

        if (operation == SM_CHANGE_DISK_STATE){
            changeDeviceState(sm->getChangingState().c_str());
            delete(sm);
        }
        else {

            // Link pending message
            pendingMessage = sm;

            // Process the current IO request
            requestTime = service (sm);
            changeState (DISK_ACTIVE);

            // Schedule a selft message to wait the request time...
            scheduleAt (simTime()+requestTime, latencyMessage);
        }
}


void SimpleDisk::processResponseMessage (icancloud_Message *sm){
	showErrorMessage ("There is no response messages in Disk Modules!!!");
}


simtime_t SimpleDisk::service(icancloud_Message *sm){

	simtime_t totalDelay;			// Time to perform the IO operation
	simtime_t transferTime;

	size_blockList_t totalBlocks = 0;		// Total Number of blocks
	icancloud_BlockList_Message *sm_bl;	// Block list message

    	// Init
    	//totalDelay = 0;

    	// Cast to Standard message
	    sm_bl = dynamic_cast<icancloud_BlockList_Message *>(sm);

	    // Get the number of blocks
    	//totalBlocks =  sm_bl->getFile().getTotalSectors ();
	    if (sm_bl == NULL) throw cRuntimeError("Maybe the file that has been requested has not properly pwd setted..\n");

	    if (sm_bl->getOperation() == 'r')
            transferTime = ((double)sm_bl->getSize()/(double)(readBandwidth*MB));
        else
            transferTime = ((double)sm_bl->getSize()/(double)(writeBandwidth*MB));

	    // Calculate total time!
	    //totalDelay = storageDeviceTime (totalBlocks, operation);
	    
	    if (DEBUG_Disk)
	    	showDebugMessage ("Processing request. Time:%f  - Requesting %lu bytes",
	    	                    transferTime.dbl(),
	    						totalBlocks);	

	    // Update message length
	    sm_bl->updateLength();

	    // Transfer data
		sm_bl->setIsResponse(true);

	return transferTime;
}


simtime_t SimpleDisk::storageDeviceTime (size_blockList_t numBlocks, char operation){

	simtime_t transferTime;					// Transfer time
	unsigned long long int totalBytes;		// Total Number of bytes

		// Init...
		transferTime = 0.0;
		totalBytes = numBlocks*BYTES_PER_SECTOR;

		if (operation == 'r')
			transferTime = (simtime_t) ((simtime_t)totalBytes/(readBandwidth*MB));
		else
			transferTime = (simtime_t) ((simtime_t)totalBytes/(writeBandwidth*MB));

	return (transferTime);
}


void SimpleDisk::changeDeviceState (string state, unsigned componentIndex){

	if (strcmp (state.c_str(),MACHINE_STATE_IDLE ) == 0) {

		nodeState = MACHINE_STATE_IDLE;
		changeState (DISK_IDLE);

	} else if (strcmp (state.c_str(),MACHINE_STATE_RUNNING ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		changeState (DISK_IDLE);

	} else if (strcmp (state.c_str(),MACHINE_STATE_OFF ) == 0) {

		nodeState = MACHINE_STATE_OFF;
		changeState (DISK_OFF);
	}
}


void SimpleDisk::changeState (string energyState, unsigned componentIndex){

//	if (strcmp (nodeState.c_str(),MACHINE_STATE_OFF ) == 0) {
//		energyState = DISK_OFF;
//	}

//	if (strcmp (energyState.c_str(), DISK_OFF) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), DISK_IDLE) == 0) NULL;
//
//	else if (strcmp (energyState.c_str(), DISK_ACTIVE) == 0) NULL;
//
// 	else NULL;

	e_changeState (energyState);
}
