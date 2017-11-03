#include "NetworkService.h"

Define_Module (NetworkService);

NetworkService::~NetworkService(){

}


void NetworkService::initialize(){

	std::ostringstream osStream;

    	// Set the moduleIdName
		osStream << "NetworkService." << getId();
		moduleIdName = osStream.str();

	    // Init the super-class
	    HWEnergyInterface::initialize();

	    // Module parameters
	    localIP = (const char*) par ("localIP");
	    
	    // Module gates
	    fromNetManagerGate = gate ("fromNetManager");
	    fromNetTCPGate = gate ("fromNetTCP");
	    toNetManagerGate = gate ("toNetManager");
	    toNetTCPGate = gate ("toNetTCP");	    
	    
	    // Service objects
	    clientTCP_Services = new TCP_ClientSideService (localIP, toNetTCPGate, this);
	    serverTCP_Services = new TCP_ServerSideService (localIP, toNetTCPGate, toNetManagerGate, this);

	    nodeState = MACHINE_STATE_OFF;

	    sm_vector.clear();
	    lastOp = 0;

}


void NetworkService::finish(){

    sm_vector.clear();
    lastOp = 0;

	// Finish the super-class
    HWEnergyInterface::finish();
}


void NetworkService::handleMessage(cMessage *msg){

	icancloud_Message *sm;

		// If msg is a Self Message...
		if (msg->isSelfMessage())
			processSelfMessage (msg);

		// Not a self message...
		else{			
			
			// Established connection message...
			if (!strcmp (msg->getName(), "ESTABLISHED")){
//			    delete(msg);
				receivedEstablishedConnection (msg);
			}
			
			// Closing connection message ..
			else if (!strcmp (msg->getName(), "PEER_CLOSED")){
				serverTCP_Services->closeConnectionReceived(msg);
			}
			// Finished connection message ..
			else if (!strcmp (msg->getName(), "CLOSED")){
				delete(msg);
			}

			// Not an ESTABLISHED message message...
			else{
										
				// Cast!
				sm = check_and_cast<icancloud_Message *>(msg);

				// Request!
				if (!sm->getIsResponse()){

					// Update message trace
					updateMessageTrace (sm);

					// Insert into queue
					queue.insert (sm);

					// If not processing any request...
					processCurrentRequestMessage ();
				}

				// Response message!
				else
					processResponseMessage (sm);
			}
		}	
}


cGate* NetworkService::getOutGate (cMessage *msg){
	
	
		// If msg arrive from Service Redirector
		if (msg->getArrivalGate()==fromNetManagerGate){
			if (gate("toNetManager")->getNextGate()->isConnected()){
				return (toNetManagerGate);
			}
		}
		
		// If msg arrive from Service Redirector
		else if (msg->getArrivalGate()==fromNetTCPGate){
			if (gate("toNetTCP")->getNextGate()->isConnected()){
				return (toNetTCPGate);
			}
		}	
		

	// If gate not found!
	return NULL;
}


void NetworkService::processSelfMessage (cMessage *msg){


    if ((!strcmp (msg->getName(), "enqueued")) && ((sm_vector.size() > 0))){

        cancelEvent(msg);

        icancloud_Message* sm;

        // Load the first message
        sm = (*(sm_vector.begin()));

        // send the message
        clientTCP_Services->sendPacketToServer (sm);

        // erase the message from queue
        sm_vector.erase(sm_vector.begin());

        if (sm_vector.size() > 0){
            cMessage* waitToExecuteMsg = new cMessage ("enqueued");
            scheduleAt (simTime() + 0.5, waitToExecuteMsg);
        }
        else
            // Clean the lock of last op
            lastOp = 0;

    }

    delete(msg);
}


void NetworkService::processRequestMessage (icancloud_Message *sm){
	
	TCPSocket *socket;
	int operation;
		// Msg cames from Hypervisor ...
		if (sm->getArrivalGate() == fromNetManagerGate){

			if (DEBUG_Network_Service)
				showDebugMessage ("[processRequestMessage] from Service Redirector. %s",sm->contentsToString(DEBUG_MSG_Network_Service).c_str());
			
			
			// Create a new connection... client-side
			operation = sm->getOperation();
			if (operation == SM_CREATE_CONNECTION){

				clientTCP_Services->createConnection (sm);
			}		
			
			// Create a listen connection... server-side
			else if (operation == SM_LISTEN_CONNECTION){
				serverTCP_Services->newListenConnection (sm);
			}
			
			// Send data...
			else if ((operation == SM_OPEN_FILE)   ||
					 (operation == SM_CLOSE_FILE)  ||
					 (operation == SM_READ_FILE)   ||
					 (operation == SM_WRITE_FILE)  ||
					 (operation == SM_CREATE_FILE) ||
					 (operation == SM_DELETE_FILE) ||
					 (operation == SM_SEND_DATA_NET)){
				
				clientTCP_Services->sendPacketToServer (sm);
			}
			
			// Remote Storage Calls
			else if ((operation == SM_VM_REQUEST_CONNECTION_TO_STORAGE) ||
					 (operation == SM_NODE_REQUEST_CONNECTION_TO_MIGRATE)){

				clientTCP_Services->createConnection (sm);
			}

			else if (operation == SM_MIGRATION_REQUEST_LISTEN){

				serverTCP_Services->newListenConnection (sm);

			}

			// Migration calls...
			else if ((operation == SM_ITERATIVE_PRECOPY) ||
					 (operation == SM_STOP_AND_DOWN_VM)  ||
					 (operation == SM_VM_ACTIVATION)){

				clientTCP_Services->sendPacketToServer (sm);

			}

			// Close connection...
			else if (operation == SM_CLOSE_CONNECTION){

				clientTCP_Services->closeConnection (sm);

			}


			// MPI Calls			
			else if ((operation == MPI_SEND) ||
					 (operation == MPI_RECV) ||
					 (operation == MPI_BARRIER_UP)   ||
					 (operation == MPI_BARRIER_DOWN) ||
					 (operation == MPI_BCAST)   ||
					 (operation == MPI_SCATTER) ||
					 (operation == MPI_GATHER)){
				
			        clientTCP_Services->sendPacketToServer (sm);

			}
			
			// Change state
			else if (operation == SM_CHANGE_NET_STATE){
				// change the state of the network
				changeDeviceState (sm->getChangingState().c_str());

				delete(sm);

			}
			// Wrong operation...
			else{
				showErrorMessage ("Wrong request operation... %s", sm->contentsToString(true).c_str());
			}			
		}		
		
		// Msg cames from TCP Network
		else if (sm->getArrivalGate() == fromNetTCPGate){
			
			if (DEBUG_Network_Service)
				showDebugMessage ("[processRequestMessage] from TCP Network. %s", sm->contentsToString(DEBUG_MSG_Network_Service).c_str());

			// Seach the involved socket... server...
			socket = serverTCP_Services->getInvolvedSocket (sm);
			
			// Receiving data...
			if (socket != NULL){
				socket->processMessage(sm);				
			}

			// No socket found!
			else				
				showErrorMessage ("[processRequestMessage] No socket found!. %s",sm->contentsToString(true).c_str());							
		}
}


void NetworkService::processResponseMessage (icancloud_Message *sm){

	TCPSocket *socket;

		// Msg cames from Service Redirector...
		if (sm->getArrivalGate() == fromNetManagerGate){
			
			if (DEBUG_Network_Service)
				showDebugMessage ("[processResponseMessage] from Service Redirector. %s",sm->contentsToString(DEBUG_MSG_Network_Service).c_str());						
			
			socket = serverTCP_Services->getInvolvedSocket (sm);
			
			// Sending data to corresponding client...
			if (socket != NULL){
				serverTCP_Services->sendPacketToClient(sm);
			}

			// Not socket found!
			else{				
				showErrorMessage ("[processResponseMessage] Socket not found... %s", sm->contentsToString(true).c_str());
			}		
		}

		// Msg cames from TCP Network
		else if (sm->getArrivalGate() == fromNetTCPGate){
			
			if (DEBUG_Network_Service)
				showDebugMessage ("[processResponseMessage] from TCP Network. %s",sm->contentsToString(DEBUG_MSG_Network_Service).c_str());
			
			socket = clientTCP_Services->getInvolvedSocket (sm);
						
			// Sending data to corresponding application...
			if (socket != NULL){
				socket->processMessage(sm);
			}

			// Not socket found!
			else{				
				showErrorMessage ("[processResponseMessage] Socket not found... %s", sm->contentsToString(true).c_str());
			}		
		}	
		changeState(NETWORK_OFF);
}


void NetworkService::receivedEstablishedConnection (cMessage *msg){
	
	TCPSocket *socket;
	
		socket = clientTCP_Services->getInvolvedSocket (msg);
			
		// Establishing connection... (client)
		if (socket != NULL)
			socket->processMessage(msg);
			
		// Establishing connection... (server)	
		else		
			serverTCP_Services->arrivesIncommingConnection(msg);		
}


void NetworkService::changeDeviceState (string state,unsigned componentIndex){

	if (strcmp (state.c_str(),MACHINE_STATE_IDLE ) == 0) {

		nodeState = MACHINE_STATE_IDLE;
		changeState (NETWORK_ON);

	} else if (strcmp (state.c_str(),MACHINE_STATE_RUNNING ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		changeState (NETWORK_ON);

	} else if (strcmp (state.c_str(),MACHINE_STATE_OFF ) == 0) {

		nodeState = MACHINE_STATE_OFF;
		changeState (NETWORK_OFF);

	}
}

void NetworkService::changeState (string energyState,unsigned componentIndex){

	if (strcmp (nodeState.c_str(),MACHINE_STATE_OFF ) == 0) {
		energyState = NETWORK_OFF;
	}


	e_changeState (energyState);

}

