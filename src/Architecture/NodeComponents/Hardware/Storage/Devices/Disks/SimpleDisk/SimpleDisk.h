#ifndef _SIMPLE_DISK_H_
#define _SIMPLE_DISK_H_

#include "IStorageDevice.h"

//----------------------------------------- DISK STATES ----------------------------------------- //

#define     DISK_ON                     "disk_on"                       // Disk ON
#define     DISK_OFF                    "disk_off"                      // Disk OFF
#define     DISK_ACTIVE                 "disk_active"                       // Disk ACTIVE
#define     DISK_IDLE                   "disk_idle"                     // Disk IDLE

/**
 * @class SimpleDisk SimpleDisk.h "SimpleDisk.h"
 *   
 * Class that simulates a disk with a user defined bandwidth (for reads and writes)
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 *
 * @author Gabriel Gonz&aacute;lez Casta;&ntilde;&eacute;
 * @date 2012-23-11
 *
 */
class SimpleDisk: public IStorageDevice{


	protected:
				  	
	  	/** Read bandwidth */
	  	unsigned int readBandwidth;
	  	
	  	/** Write bandwidth */
	  	unsigned int writeBandwidth;
	
		/** Gate ID. Input gate. */
		cGate* inGate;	
		
		/** Gate ID. Output gate. */
		cGate* outGate;	
	    	    
	    /** Node state */
	    string nodeState;

	private: 

		/** Request time. */
		simtime_t requestTime;
	
		/** Pending message */
	    cMessage *pendingMessage; 
  
  
  	   /**
  		* Destructor. 
  		*/
  		~SimpleDisk();


        int numInitStages() const {return 3;}

       /**
        *  Module initialization.
        */
        void initialize(int stage);
	    
	   	    
	   /**
	 	* Module ending. 		
	 	*/ 
	    void finish();
			   		
		
	private:
	
	
	   /**
		* Get the outGate to the module that sent <b>msg</b>
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
 		* Method that calculates the spent time to process the current request.
 		* @param sm Message that contains the current I/O request.
 		* @return Spent time to process the current request. 
 		*/
		simtime_t service (icancloud_Message *sm);
		
		
	   /**
		* Method that implements the Disk simulation equation.
		* 
		* @param numBlocks Number of blocks to read/write.
		* @param operation Operation type.
		* @return Spent time to perform the corresponding I/O operation.
		*/
		simtime_t storageDeviceTime (size_blockList_t numBlocks, char operation);

		/*
		 * Change the energy state of the memory given by node state
		 */
		void changeDeviceState (string state, unsigned componentIndex = 0);

		/*
		 * Change the energy state of the disk
		 */
		void changeState (string energyState, unsigned componentIndex = 0);
};

#endif

