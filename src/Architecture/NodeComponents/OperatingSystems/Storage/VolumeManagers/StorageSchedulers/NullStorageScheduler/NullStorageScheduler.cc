#include "NullStorageScheduler.h"

Define_Module(NullStorageScheduler);

NullStorageScheduler::~NullStorageScheduler(){
}


void NullStorageScheduler::initialize(){

	std::ostringstream osStream;


		// Set the moduleIdName
		osStream << "NullStorageScheduler." << getId();
		moduleIdName = osStream.str();

		// Init the super-class
		icancloud_Base::initialize();

		// Init the gates
	    toInputGate = gate ("toInput");
	    fromInputGate = gate ("fromInput");
	    toOutputGate = gate ("toOutput");
	    fromOutputGate = gate ("fromOutput");
}


void NullStorageScheduler::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* NullStorageScheduler::getOutGate (cMessage *msg){

		// If msg arrive from Input
		if (msg->getArrivalGate()==fromInputGate){
			if (toInputGate->getNextGate()->isConnected()){
				return (toInputGate);
			}
		}

		// If msg arrive from Output
		if (msg->getArrivalGate()==fromOutputGate){
			if (toOutputGate->getNextGate()->isConnected()){
				return (toOutputGate);
			}
		}

	// If gate not found!
	return NULL;
}


void NullStorageScheduler::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void NullStorageScheduler::processRequestMessage (icancloud_Message *sm){
	sendRequestMessage (sm, toOutputGate);
}


void NullStorageScheduler::processResponseMessage (icancloud_Message *sm){
	sendResponseMessage (sm);
}

