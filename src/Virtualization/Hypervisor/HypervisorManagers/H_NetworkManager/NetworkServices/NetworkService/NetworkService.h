#ifndef _E_NETWORK_SERVICE_H_
#define _E_NETWORK_SERVICE_H_

#include <omnetpp.h>
#include "HWEnergyInterface.h"
#include "TCP_ClientSideService.h"
#include "TCP_ServerSideService.h"
#include "icancloud_App_NET_Message.h"

#define     NETWORK_ON                  "network_on"                    // Network OFF
#define     NETWORK_OFF                 "network_off"                   // Network ON
/**
 * @class NetworkService NetworkService.h "NetworkService.h"
 *   
 * Network Service Module
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 *
 * @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2012-10-23
 */


class TCP_ClientSideService;
class TCP_ServerSideService;

class NetworkService : public HWEnergyInterface{


	protected:	
		
		/** Local IP address */
		string localIP;
										
		/** TCP Client-side Services */
		TCP_ClientSideService *clientTCP_Services;
		
		/** TCP Server-side Services */
		TCP_ServerSideService *serverTCP_Services;
		
		/** Input gate from Service Redirector */
	    cGate* fromNetManagerGate;

	    /** Input gate from Network (TCP) */
	    cGate* fromNetTCPGate;

	    /** Output gate to Service Redirector */
	    cGate* toNetManagerGate;

	    /** Output gate to Network (TCP) */
	    cGate* toNetTCPGate;	
	    
	    /** Node state */
	    string nodeState;

        vector <icancloud_Message*> sm_vector;
        int lastOp;

	    /**
	    * Destructor.
	    */    		
	    ~NetworkService();		
	  	        			  	    	    
	   /**
	 	*  Module initialization.
	 	*/
	    void initialize();
	    
	   /**
	 	* Module ending.
	 	*/ 
	    void finish();

	    
	private:
	
	
	   /**
		* This method classifies an incoming message and invokes the corresponding method
		* to process it.
		*
		* For self-messages, it invokes processSelfMessage(sm);
		* For request-messages, it invokes processRequestMessage(sm);
		* For response-messages, it invokes processResponseMessage(sm);
		*/
		void handleMessage (cMessage *msg);
	
	   /**
		* Get the outGate ID to the module that sent <b>msg</b>
		* @param msg Arrived message.
		* @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
		*/ 
		cGate* getOutGate (cMessage *msg);	
	  
	   /**
		* Process a self message.
		* @param msg Self message.
		*/
		void processSelfMessage (cMessage *msg);

	   /**
		* Process a request message.
		* @param sm Request message.
		*/
		void processRequestMessage (icancloud_Message *sm);

	   /**
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);
		
		/**
		 * Receiving an established connection message.
		 */
		void receivedEstablishedConnection (cMessage *msg);


		/*
		 * Change the energy state of the memory given by node state
		 */
		void changeDeviceState (string state, unsigned componentIndex = 0);

		/*
		 *  Change the energy state of the memory
		 */
		void changeState (string energyState,unsigned componentIndex = 0);
};

#endif
