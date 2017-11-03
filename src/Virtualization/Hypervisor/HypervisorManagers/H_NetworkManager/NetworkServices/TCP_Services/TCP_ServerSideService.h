#ifndef __TCP_SERVER_SIDE_SERVICE_H_
#define __TCP_SERVER_SIDE_SERVICE_H_

#include <omnetpp.h>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "NetworkService.h"
#include "icancloud_Message.h"

class NetworkService;

class TCP_ServerSideService: public TCPSocket::CallbackInterface{	
	
   /**
	* Structure that represents a connection between two TCP applications (server-side).
	*/
	struct icancloud_TCP_Server_Connector{
		int port;
		int appIndex;
	};
	typedef struct icancloud_TCP_Server_Connector serverTCP_Connector;
	
	protected:
	
		/** Server connections vector*/
		vector <serverTCP_Connector> connections;
				    
	    /** Socket map to manage TCP sockets*/
	    TCPSocketMap socketMap;	    	    
	    
	    /** Pointer to NetworkService object */
	    NetworkService *networkService;
	    
	    /** Local IP */
	    string localIP;   	    
	    
	    /** Output gate to TCP */
	    cGate* outGate_TCP;
	    
	    /** Output gate to TCP */
	    cGate* outGate_icancloudAPI;
	       
	    
	public:
	
		/**
		 * Constructor
		 */
		TCP_ServerSideService(string newLocalIP,														
							cGate* toTCP,
							cGate* toicancloudAPI,
							NetworkService *netService);
		/**
		 * Destructor
		 */
		virtual ~TCP_ServerSideService();
			
			
		/**
		 * Create a new socket that will listen for incomming connections.
		 * @param sm_net Incomming message that must contain the localPort where new connection will listen.
		 */
		void newListenConnection (icancloud_Message *sm_net);
			
		/**
		 * Create a new connection.
		 * @param msg Incomming message that must contain the connId.
		 */
		void arrivesIncommingConnection (cMessage *msg);
		
		
		/**
	     * Send back a packet to corresponding destination.
	     * @param sm Message to send to the corresponding client.
	     */
	    void sendPacketToClient(icancloud_Message *sm);
	    
	    
	    /**
	      * Calculates if a given message correspond to an incomming connection.
	      * @param msg Message.
	      * @return Corresponding TCPSocket if msg is involved in an existing connection or
	      * NULL in other case.
	      */
	     TCPSocket* getInvolvedSocket (cMessage *msg);
	     
	     /**
	      * Exists a connection?
	      */
	     bool existsConnection (int port);
	     
	     int getAppIndexFromPort (int port);
    	
		/**
		 * Close a connection.
		 */
		void closeConnectionReceived(cMessage *sm);

	protected:
  
	    // TCPSocket::CallbackInterface methods
	    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);
	    virtual void socketEstablished(int connId, void *yourPtr);	    
	    virtual void socketPeerClosed(int connId, void *yourPtr);
	    virtual void socketClosed(int connId, void *yourPtr);    
	    virtual void socketFailure(int connId, void *yourPtr, int code);
	    virtual void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status); 
};


#endif

