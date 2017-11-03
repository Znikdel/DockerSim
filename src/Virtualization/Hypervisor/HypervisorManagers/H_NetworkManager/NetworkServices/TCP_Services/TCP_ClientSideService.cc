#include "TCP_ClientSideService.h"


TCP_ClientSideService::TCP_ClientSideService(string newLocalIP,						 
											cGate* toTCP,
											NetworkService *netService){
    localIP = newLocalIP;    
    outGate_TCP = toTCP;
    networkService = netService;
}


TCP_ClientSideService::~TCP_ClientSideService(){
	socketMap.deleteSockets ();
	connections.clear();
}


void TCP_ClientSideService::createConnection(icancloud_Message *sm){

	clientTCP_Connector newConnection;
	icancloud_App_NET_Message *sm_net;

	
		// Cast to icancloud_App_NET_Message
		sm_net = dynamic_cast<icancloud_App_NET_Message *>(sm);
		
		// Wrong message?
		if (sm_net == NULL)
			networkService->showErrorMessage ("[createConnection] Error while casting to icancloud_App_NET_Message!");
	
		// Attach the message to the corresponding connection
		newConnection.msg = sm;
		
		// Create the socket
		newConnection.socket = new TCPSocket();
		newConnection.socket->setDataTransferMode(TCP_TRANSFER_OBJECT);
	    newConnection.socket->bind(*(localIP.c_str()) ? IPvXAddress(localIP.c_str()) : IPvXAddress(), -1);
	    newConnection.socket->setCallbackObject(this);
	    newConnection.socket->setOutputGate(outGate_TCP);
		newConnection.socket->renewSocket();
				
		// Add new socket to socketMap
	    socketMap.addSocket (newConnection.socket);
	    
	    // Add new connection to vector
		connections.push_back (newConnection);
		
		// Debug...
		if (DEBUG_TCP_Service_Client)
			networkService->showDebugMessage ("[createConnection] local IP:%s ---> %s:%d. %s", 
												localIP.c_str(),
												sm_net->getDestinationIP(), 
												sm_net->getDestinationPort(),
												sm_net->contentsToString(DEBUG_TCP_Service_MSG_Client).c_str());

	
	    // Establish Connection	    
	    newConnection.socket->connect(IPvXAddressResolver().resolve(sm_net->getDestinationIP()), sm_net->getDestinationPort());
}


void TCP_ClientSideService::sendPacketToServer(icancloud_Message *sm){
		
	 int index;
		// Search for the connection...
	 	index = searchConnectionByConnId (sm->getConnectionId());
	 	
	 	// Connection not found?
	 	if (index == NOT_FOUND)
	 		networkService->showErrorMessage ("[sendPacketToServer] Socket not found!");
	 		
	 	else{
	 		         	
			if (DEBUG_TCP_Service_Client)
				networkService->showDebugMessage ("[sendPacketToServer] Sending message to %s:%d. %s", 
													connections[index].socket->getRemoteAddress().str().c_str(),
													connections[index].socket->getRemotePort(),
													sm->contentsToString(DEBUG_TCP_Service_MSG_Client).c_str());
			
         	connections[index].socket->send(sm);	     	
	 	}
} 


void TCP_ClientSideService::closeConnection(icancloud_Message *sm){
	
	TCPSocket *socket;
	
		// Search for the socket
		socket = socketMap.findSocketFor(sm);
	    	
	         if (!socket)
	         	networkService->showErrorMessage ("[closeConnection] Socket not found to send the message: %s\n", sm->contentsToString(true).c_str());	                 
	         else{
	         	
	         	if (DEBUG_TCP_Service_Client)
	         		networkService->showDebugMessage ("Closing connection %s:%d. %s", 
												socket->getRemoteAddress().str().c_str(),
												socket->getRemotePort(),
												sm->contentsToString(DEBUG_TCP_Service_MSG_Client).c_str());
	         	socket->close();

				socket = socketMap.removeSocket(socket);

				delete(socket);
	         }	

	         delete (sm);
}


int TCP_ClientSideService::searchConnectionByConnId(int connId){
	
	int result;
	bool found;
	unsigned int i;
	
		// Init
		found = false;
		i = 0;
		
			// Search for the connection...
			while ((!found) && (i<connections.size())){
				
				if (connections[i].socket->getConnectionId() == connId)
					found = true;
				else
					i++;
			}
			
			// Set the result
			if (found)
				result = i;			
			else
				result = NOT_FOUND;
				
	return result;	
}


TCPSocket* TCP_ClientSideService::getInvolvedSocket (cMessage *msg){
	return (socketMap.findSocketFor(msg));
}


void TCP_ClientSideService::socketEstablished(int connId, void *ptr){
	 
	 int index;
	 icancloud_App_NET_Message *sm_net;
	 
	 
	 	// Search for the connection...
	 	index = searchConnectionByConnId (connId);
	 	
	 	// Connection not found?
	 	if (index == NOT_FOUND)
	 		networkService->showErrorMessage ("[socketEstablished] Socket not found!");

//	 	// Retry
//	 	else if (connId < 0){
//
//	 		// Cast
//			sm_net = dynamic_cast<icancloud_App_NET_Message *>(connections[index].msg);
//
//			// Try again!ç
//			connections[index].socket->renewSocket();
//	 		connections[index].socket->connect(IPAddressResolver().resolve(sm_net->getDestinationIP()), sm_net->getDestinationPort());
//
//	 	}
	 		
	 	// All OK!
	 	else{

	 		// Cast
	 		sm_net = dynamic_cast<icancloud_App_NET_Message *>(connections[index].msg);
	 		
	 		// Detach
	 		connections[index].msg  = NULL;

	 		// update msg status
	 		sm_net->setIsResponse (true);
	 		sm_net->setConnectionId (connId);
	 		sm_net->setLocalIP (connections[index].socket->getLocalAddress().str().c_str());
			sm_net->setLocalPort (connections[index].socket->getLocalPort());

	 		// debug...
	 		if (DEBUG_TCP_Service_Client)
	 			networkService->showDebugMessage ("[socketEstablished-client] %s Connection established with %s:%d", 
												localIP.c_str(),
												connections[index].socket->getRemoteAddress().str().c_str(),
												connections[index].socket->getRemotePort());
												
	 		// Send back the response
	 		networkService->sendResponseMessage (sm_net);
	 	}	 		
}


void TCP_ClientSideService::socketDataArrived(int connId, void *ptr, cPacket *msg, bool urgent){

	icancloud_Message *sm;
	int index;
	
		// Casting
		sm = dynamic_cast<icancloud_Message *>(msg);
		
		// Show debug?
		if (DEBUG_TCP_Service_Client){
			
			index = searchConnectionByConnId (connId);
	 	
		 	// Connection not found?
		 	if (index == NOT_FOUND)
		 		networkService->showErrorMessage ("[socketDataArrived] Socket not found!");
					 	
			networkService->showDebugMessage ("Arrived data from %s:%d. %s", 
											connections[index].socket->getRemoteAddress().str().c_str(),
											connections[index].socket->getRemotePort(),
											sm->contentsToString(DEBUG_TCP_Service_MSG_Client).c_str());
		}
		
		// Send back the message...		
		networkService->sendResponseMessage (sm);
}


void TCP_ClientSideService::socketPeerClosed(int, void *){	
	if (DEBUG_TCP_Service_Client)
		networkService->showDebugMessage ("Socket Peer closed");
}


void TCP_ClientSideService::socketClosed(int connId, void *ptr){
	if (DEBUG_TCP_Service_Client)
		networkService->showDebugMessage ("Socket closed");
}


void TCP_ClientSideService::socketFailure(int connId, void *ptr, int code){	
	if (DEBUG_TCP_Service_Client)
		networkService->showDebugMessage ("Socket Failure");
}


void TCP_ClientSideService::printSocketInfo (TCPSocket *socket){
	
	printf ("Socket info. ConnId:%d\n", socket->getConnectionId());
	printf (" - Source IP:%s   Source Port:%d", socket->getLocalAddress().str().c_str(), socket->getLocalPort());
	printf (" - Destination IP:%s - Destination Port:%d\n", socket->getRemoteAddress().str().c_str(), socket->getRemotePort());
}


