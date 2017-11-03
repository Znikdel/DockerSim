#include "NullStorageManager.h"

Define_Module(NullStorageManager);


NullStorageManager::~NullStorageManager(){
}


void NullStorageManager::initialize(){

	unsigned int i;
	std::ostringstream osStream;


		// Set the moduleIdName
		osStream << "NullStorageSystems." << getId();
		moduleIdName = osStream.str();
		
		numStorageSystem = par ("numStorageSystems");
		
		if (numStorageSystem > 1)
			showErrorMessage ("NullStorageManager cannot be used with more than one disk!!!");

		// Init the super-class
		icancloud_Base::initialize();	

		// Init the gate IDs to/from Scheduler
	    toSchedulerGate = gate ("toScheduler");
	    fromSchedulerGate = gate ("fromScheduler");
	    
	    // Init the gates IDs to/from StorageServers
	    toStorageSystemGates = new cGate* [numStorageSystem];
	    fromStorageSystemGates = new cGate* [numStorageSystem];  

	    for (i=0; i<numStorageSystem; i++){
			toStorageSystemGates[i] = gate ("toStorageSystem", i);
			fromStorageSystemGates[i] = gate ("fromStorageSystem", i);
	    }
}


void NullStorageManager::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* NullStorageManager::getOutGate (cMessage *msg){

	unsigned int i;

		// If msg arrive from scheduler
		if (msg->getArrivalGate()==fromSchedulerGate){
			if (toSchedulerGate->getNextGate()->isConnected()){
				return (toSchedulerGate);
			}
		}

		// If msg arrive from StorageServers
		else if (msg->arrivedOn("fromStorageSystem")){
			for (i=0; i<numStorageSystem; i++)
				if (msg->arrivedOn ("fromStorageSystem", i))
					return (toStorageSystemGates[i]);
		}

	// If gate not found!
	return NULL;
}


void NullStorageManager::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void NullStorageManager::processRequestMessage (icancloud_Message *sm){
	sendRequestMessage (sm, toStorageSystemGates[0]);
}


void NullStorageManager::processResponseMessage (icancloud_Message *sm){
	sendResponseMessage (sm);	
}

