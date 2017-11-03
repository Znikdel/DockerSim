#include "TCP_ServerSideService.h"



TCP_ServerSideService::TCP_ServerSideService(string newLocalIP,			    										    						
				    						cGate* toTCP,
				    						cGate* toicancloudAPI,
				    						NetworkService *netService){

		// Init...
		localIP = newLocalIP;	    	    
	    outGate_TCP = toTCP;
	    outGate_icancloudAPI = toicancloudAPI;
	    networkService = netService;	    	    
}


TCP_ServerSideService::~TCP_ServerSideService(){
	socketMap.deleteSockets ();		
}


void TCP_ServerSideService::newListenConnection (icancloud_Message *sm){
	
	TCPSocket *newSocket;	
	icancloud_App_NET_Message *sm_net;
	serverTCP_Connector newConnection;
		
	
		// Cast to icancloud_App_NET_Message
		sm_net = dynamic_cast<icancloud_App_NET_Message *>(sm);
		
		// Wrong message?
		if (sm_net == NULL)
			networkService->showErrorMessage ("[TCP_ServerSideService::newListenConnection] Error while casting to icancloud_App_NET_Message!");
			
		// Exists a previous connection listening at the same port
		if (existsConnection(sm_net->getLocalPort()))
			networkService->showErrorMessage ("[TCP_ServerSideService::newListenConnection] This connection already exists. Port:%d!", sm_net->getLocalPort());			

		// Connection entry
		newConnection.port = sm_net->getLocalPort();
		newConnection.appIndex = sm_net->getNextModuleIndex();		 
		connections.push_back (newConnection);

		// Create a new socket
		newSocket = new TCPSocket();
		
		// Listen...

		newSocket->setOutputGate(outGate_TCP);
		newSocket->setDataTransferMode(TCP_TRANSFER_OBJECT);
		newSocket->bind(*(localIP.c_str()) ? IPvXAddress(localIP.c_str()) : IPvXAddress(), sm_net->getLocalPort());	    
		newSocket->setCallbackObject(this);
		newSocket->listen();
	    
	    // Debug...
	    if (DEBUG_TCP_Service_Server)
	    	networkService->showDebugMessage ("[TCP_ServerSideService] New connection for App[%d] is listening on %s:%d",	    										
	    										sm_net->getNextModuleIndex(),
	    										localIP.c_str(),
												sm_net->getLocalPort());

	    // Delete msg
	    delete (sm);
}


void TCP_ServerSideService::arrivesIncommingConnection (cMessage *msg){
	
	TCPSocket *socket;	
	
		// Search an existing connection...
		socket = socketMap.findSocketFor(msg);
        
        // Connection does not exist. Create a new one!
        if (!socket){
        	
            // Create a new socket
            socket = new TCPSocket(msg);
            socket->setOutputGate(outGate_TCP);
            socket->setDataTransferMode(TCP_TRANSFER_OBJECT);
            socket->setCallbackObject(this);            
            socketMap.addSocket(socket);
            
            if (DEBUG_TCP_Service_Server)
		    networkService->showDebugMessage ("[TCP_ServerSideService] Arrives an incoming connection from %s:%d to local connection %s:%d with connId = %d",
													socket->getRemoteAddress().str().c_str(),
													socket->getRemotePort(),
													socket->getLocalAddress().str().c_str(),
													socket->getLocalPort(),
													socket->getConnectionId());
													            
            // Process current operation!
        	socket->processMessage(msg);           
        }
        else
        	networkService->showErrorMessage ("[TCP_ServerSideService::arrivesIncommingConnection] Connection already exists. ConnId =  %d", socket->getConnectionId());
}


void TCP_ServerSideService::sendPacketToClient(icancloud_Message *sm){

	TCPSocket *socket;

		// Search for the socket
		socket = socketMap.findSocketFor(sm);

	         if (!socket)
	         	networkService->showErrorMessage ("[sendPacketToClient] Socket not found to send the message: %s\n", sm->contentsToString(true).c_str());
	         	
	         else{
	         	
	         	if (DEBUG_TCP_Service_Server)
	         		networkService->showDebugMessage ("Sending message to client %s:%d. %s", 
												socket->getRemoteAddress().str().c_str(),
												socket->getRemotePort(),
												sm->contentsToString(DEBUG_TCP_Service_MSG_Server).c_str());	         		         		         		         	
	         	         	
	         	delete sm->removeControlInfo();
			    TCPSendCommand *cmd = new TCPSendCommand();
			    cmd->setConnId(socket->getConnectionId());
			    sm->setControlInfo(cmd);
				sm->setKind (TCP_C_SEND);	
				networkService->sendResponseMessage (sm);   	         		         	

	         }
}


TCPSocket* TCP_ServerSideService::getInvolvedSocket (cMessage *msg){

	return (socketMap.findSocketFor(msg));
}


void TCP_ServerSideService::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent){
	
	icancloud_Message *sm;
	TCPSocket *socket;
	int appIndex;		
		
			
		// Casting
		sm = dynamic_cast<icancloud_Message *>(msg);
		
		if (sm == NULL)
			networkService->showErrorMessage ("[socketDataArrived] Error while casting to icancloud_Message!");
			
		// Get involved socket
		socket = socketMap.findSocketFor(sm);
		
		if (socket == NULL)
			networkService->showErrorMessage ("[socketDataArrived] Socket not found. %s!", sm->contentsToString(true).c_str());			

		// Search for the server App index		
		appIndex = getAppIndexFromPort (socket->getLocalPort());

		if (DEBUG_TCP_Service_Server)	
			networkService->showDebugMessage ("[socketDataArrived] Server Index:%d\n", appIndex);
		
		if (appIndex == NOT_FOUND)
			networkService->showErrorMessage ("[socketDataArrived] Port not found. %d!", socket->getLocalPort());
		
		// Set server index in app vector!
		sm->setNextModuleIndex (appIndex);
		
		if (DEBUG_TCP_Service_Server)	
			networkService->showDebugMessage ("[socketDataArrived] Arrived data from %s:%d. Server Index:%d. %s", 
											socket->getRemoteAddress().str().c_str(),
											socket->getRemotePort(),
											appIndex,
											sm->contentsToString(DEBUG_TCP_Service_MSG_Server).c_str());	
		
	// Send message to Service Redirector
	networkService->sendRequestMessage (sm, outGate_icancloudAPI);
}


void TCP_ServerSideService::socketEstablished(int connId, void *yourPtr){
	
	if (DEBUG_TCP_Service_Server)
		networkService->showDebugMessage ("[socketEstablished-server] Socket establised with connId = %d", connId);
}


void TCP_ServerSideService::socketPeerClosed(int connId, void *yourPtr){
	
	if (DEBUG_TCP_Service_Server)
		networkService->showDebugMessage ("Socket Peer Closed with connId = %d", connId);
}


void TCP_ServerSideService::socketClosed(int connId, void *yourPtr){
	
	if (DEBUG_TCP_Service_Server)
		networkService->showDebugMessage ("Socket Closed with connId = %d", connId);
}


void TCP_ServerSideService::socketFailure(int connId, void *yourPtr, int code){
	
	if (DEBUG_TCP_Service_Server)
		networkService->showDebugMessage ("Socket Failure with connId = %d", connId);
}


void TCP_ServerSideService::socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status){
	
	if (DEBUG_TCP_Service_Server)
		networkService->showDebugMessage ("Socket Status Arrived with connId = %d", connId);
}


bool TCP_ServerSideService::existsConnection (int port){
	
	bool found;
	unsigned int i;
	
		// init
		found = false;
		i = 0;	
				
		// Search for the connection...
		while ((!found) && (i<connections.size())){
			
			if (connections[i].port == port)
				found = true;
			else
				i++;
		}		
				
	return found;	
}


int TCP_ServerSideService::getAppIndexFromPort (int port){
	
	bool found;
	unsigned int i;
	int result;
	
		// init
		result = NOT_FOUND;
		found = false;
		i = 0;	
				
		// Search for the connection...
		while ((!found) && (i<connections.size())){
			
			if (connections[i].port == port)
				found = true;
			else
				i++;
		}				
		
		// Assign appIndex...
		if (found) {
			result = connections[i].appIndex;

			if (DEBUG_TCP_Service_Server)	
				networkService->showDebugMessage ("[TCP_server:getAppIndexFromPort] found AppIndex %d for local port %d", result, port);
		} else {
			if (DEBUG_TCP_Service_Server)	
				networkService->showDebugMessage ("[TCP_server:getAppIndexFromPort] Not found AppIndex for local port %d", port);
		}

				
	return result;	
}


void TCP_ServerSideService::closeConnectionReceived(cMessage *sm){

	TCPSocket *socket;

		// Search for the socket
		socket = socketMap.findSocketFor(sm);

		 if (!socket){
			 delete(sm);
		 }else{
			socket->close();
			socket = socketMap.removeSocket(socket);
			delete(socket);
	        delete (sm);
		 }

}

