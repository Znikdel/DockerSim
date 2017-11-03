#include "BranchScheduler.h"

Define_Module(BranchScheduler);


BranchScheduler::~BranchScheduler(){
	
	SMS_branch->clear();
	delete (SMS_branch);	
}


void BranchScheduler::initialize(){

	std::ostringstream osStream;


		// Set the moduleIdName
		osStream << "BranchScheduler." << getId();
		moduleIdName = osStream.str();

		// Init the super-class
		icancloud_Base::initialize();

		// Init the gates
	    toInputGate = gate ("toInput");
	    fromInputGate = gate ("fromInput");
	    toOutputGate = gate ("toOutput");
	    fromOutputGate = gate ("fromOutput");

	    // New SMS object!
	    SMS_branch = new SMS_Branch();
}


void BranchScheduler::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* BranchScheduler::getOutGate (cMessage *msg){

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


void BranchScheduler::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void BranchScheduler::processRequestMessage (icancloud_Message *sm){

	icancloud_BlockList_Message *sm_bl;

		// Cast!
		sm_bl = check_and_cast <icancloud_BlockList_Message*> (sm);		
		
			// Split and process current request!
			SMS_branch->splitRequest(sm_bl);			
			
			// Debug
			if (DEBUG_Storage_Scheduler)
				showDebugMessage ("Processing original request:%s from message:%s", 
							SMS_branch->requestToString(sm, DEBUG_SMS_Storage_Scheduler).c_str(),
							sm_bl->contentsToString(DEBUG_BRANCHES_Storage_Scheduler, DEBUG_MSG_Storage_Scheduler).c_str());
			
			processBranches();
}


void BranchScheduler::processResponseMessage (icancloud_Message *sm){
	sendResponseMessage (sm);
}


void BranchScheduler::processBranches (){

	icancloud_Message *subRequest;

		// Send all enqueued subRequest!
		do{

			subRequest = (icancloud_Message*) SMS_branch->popSubRequest();

			// There is a subRequest!
			if (subRequest != NULL){
				sendRequestMessage (subRequest, toOutputGate);
			}

		}while (subRequest!=NULL);
}


void BranchScheduler::sendRequestMessage (icancloud_Message *sm, cGate* gate){

	// If trace is empty, add current hostName, module and request number
	if (sm->isTraceEmpty()){
		sm->addNodeToTrace (getHostName());
		updateMessageTrace (sm);
	}

	// Send the message!
	send (sm, gate);
}

