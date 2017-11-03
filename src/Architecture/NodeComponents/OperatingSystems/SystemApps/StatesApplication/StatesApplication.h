#ifndef _STATES_APPLICATION_H_
#define _STATES_APPLICATION_H_

#include <omnetpp.h>
#include "icancloud_Base.h"


/**
 * @class StatesApplication StatesApplication.h "StatesApplication.h"
 *
 *
 * Module that implements a StatesApplication.
 *
 * This application is an application system responsible for managing the states of the hardware devices
 * 
 * @author Gabriel Gonzalez Castane
 * @date 2013-03-04
 */

class StatesApplication: virtual public icancloud_Base{
	
	protected:

    //**  Timeout between each iteration of the aplication */
    simtime_t timeout;

    //**  holds pointer to the timeout self-message */
    cMessage *timeoutEvent;

		// Flag that decide if the application has to send a message to change the state of the CPU
		bool   sendToCpu;

		// Flag that decide if the application has to send a message to change the state of the Memory
		bool   sendToMemory;

		// Flag that decide if the application has to send a message to change the state of the Disk
		bool   sendToDisk;

		// Flag that decide if the application has to send a message to change the state of the Network
		bool   sendToNetwork;

		int numCPUs;

		int numBS;

		int numNetworkDevices;

        /** Input gate from OS. */
		cGate* fromOSGate;

        /** Output gate to OS. */
		cGate* toOSGate;

        /** Network type */
        string netType;

        // The actual state of the node
        string actualState;

    //    cGate* fromOSfromConGate;
   //     cGate* toOStoConGate;



	   /**
		* Destructor
		*/
		~StatesApplication();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize();

	   /**
 		* Module ending.
 		*/
	    virtual void finish();

       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or NULL if gate not found.
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

	public:
        // Start the simulation to the initial state!
        void initState (string newState);

		// Change the actual state to newState
		void changeState(string newState);

		// This method allow checking the actual state of the node.
		string getState(){return actualState;};

	private:

		void send_msg_to_change_states (bool cpu, bool memory, bool network, bool disk);
		void icancloud_request_changeState_network (string newState, vector<int> devicesIndexToChange);
        void icancloud_request_changeState_memory (string newState);
        void icancloud_request_changeState_cpu (string newState, vector<int> devicesIndexToChange);
        void icancloud_request_changeState_IO (string newState, vector<int> devicesIndexToChange);



};

#endif
