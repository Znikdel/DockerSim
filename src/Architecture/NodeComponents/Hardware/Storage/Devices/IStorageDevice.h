#ifndef _ISTORAGEDEVICE_H_
#define _ISTORAGEDEVICE_H_

#include "HWEnergyInterface.h"
#include "StorageController.h"
#include "icancloud_BlockList_Message.h"
#include "Constants.h"

class StorageController;

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
class IStorageDevice: public HWEnergyInterface{


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

	    StorageController* storageMod;

		/** Request time. */
		simtime_t requestTime;
	
		/** Pending message */
	    cMessage *pendingMessage; 

       virtual int numInitStages() const {return 2;}

      /**
       *  Module initialization.
       */
       virtual void initialize(int stage);
	    
	    void e_changeState (string energyState);

public:
	   /**
	 	* Module ending. 		
	 	*/ 
	    virtual void finish() = 0;
	
	
	   /**
		* Get the outGate to the module that sent <b>msg</b>
		* @param msg Arrived message.
		* @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
		*/ 
	    virtual cGate* getOutGate (cMessage *msg) = 0;

	   /**
		* Process a self message.
		* @param msg Self message.
		*/
	    virtual void processSelfMessage (cMessage *msg) = 0;

	   /**
		* Process a request message.
		* @param sm Request message.
		*/
	    virtual void processRequestMessage (icancloud_Message *sm) = 0;

	   /**
		* Process a response message.
		* @param sm Request message.
		*/
	    virtual void processResponseMessage (icancloud_Message *sm) = 0;
		
	   /**
 		* Method that calculates the spent time to process the current request.
 		* @param sm Message that contains the current I/O request.
 		* @return Spent time to process the current request. 
 		*/
	    virtual simtime_t service (icancloud_Message *sm) = 0;
		
				/*
		 * Change the energy state of the memory given by node state
		 */
	    virtual void changeDeviceState (string state, unsigned componentIndex = 0) = 0;

		/*
		 * Change the energy state of the disk
		 */
	    virtual void changeState (string energyState, unsigned componentIndex = 0) = 0;
};

#endif

