#ifndef __TCP_CLIENT_SIDE_SERVICE_H_
#define __TCP_CLIENT_SIDE_SERVICE_H_

#include <omnetpp.h>
#include "TCPSocket.h"
#include "IPvXAddressResolver.h"
#include "TCPSocketMap.h"
#include "NetworkService.h"
#include "icancloud_Message.h"
#include "INETDefs.h"

class NetworkService;

class TCP_ClientSideService : public TCPSocket::CallbackInterface{
	
   /**
	* Structure that represents a connection between two TCP applications (client-side).
	*/
	struct icancloud_TCP_Client_Connector{
		TCPSocket *socket;
		cPacket *msg;
	};
	typedef struct icancloud_TCP_Client_Connector clientTCP_Connector;


	protected:
		
		/** Client connections vector*/
		vector <clientTCP_Connector> connections;
		     
	    /** Socket map to manage TCP sockets*/
	    TCPSocketMap socketMap;
	    	    
	    /** Pointer to NetworkService object */
	    NetworkService *networkService;
	    
	    /** Local IP */
	    string localIP;	    	    
	    
	    /** Output gate to TCP */
	    cGate* outGate_TCP;	    

	public:
  
	  	/**
	  	 * Constructor.
	  	 * @param newLocalIP Local IP	  	 
	  	 * @param toTCP Gate to TCP module
	  	 * @param netService NetworkService object
	  	 */
	    TCP_ClientSideService(string newLocalIP,    						 
	    						cGate* toTCP,
	    						NetworkService *netService);
	    
	    /**
	     * Destructor
	     */
	    virtual ~TCP_ClientSideService();	    
	    
	    /**
	     * Create a connection with server.
	     * @param sm Message that contains the data to establish connection with server.
	     */
	    void createConnection(icancloud_Message *sm);
	    
	    /**
	     * Send a packet to corresponding destination.
	     * @param sm Message to send to the corresponding server.
	     */
	    void sendPacketToServer(icancloud_Message *sm);
	       
	    /**
	     * Close a connection.	     
	     */
	    void closeConnection(icancloud_Message *sm);
	    
	    /**
	     * Search a connection by ID.
	     * @param connId Connection ID.
	     * @return Index of the connection in the connection vector if this connection exists, or
	     * NOT_FOUND in other case.
	     */
	     int searchConnectionByConnId(int connId);
	     
	     /**
	      * Calculates if a given message correspond to an outcomming connection.
	      * @param msg Message.
	      * @return Corresponding TCPSocket if msg is involved in an existing connection or
	      * NULL in other case.
	      */
	     TCPSocket* getInvolvedSocket (cMessage *msg);
    
    
	protected:    
    
	    // TCPSocket::CallbackInterface methods
	    virtual void socketEstablished(int connId, void *yourPtr);
	    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);
	    virtual void socketPeerClosed(int connId, void *yourPtr);
	    virtual void socketClosed(int connId, void *yourPtr);    
	    virtual void socketFailure(int connId, void *yourPtr, int code);
	    virtual void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status) {delete status;}
	    
	    // Debug methods	    
	    void printSocketInfo (TCPSocket *socket);
};

#endif


