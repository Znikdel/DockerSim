#ifndef __CPU_SCHEDULER_FIFO_H_
#define __CPU_SCHEDULER_FIFO_H_

#include <omnetpp.h>
#include "icancloud_Base.h"
#include "Constants.h"

/**
 * @class CPU_Scheduler_FIFO CPU_Scheduler_FIFO.h "CPU_Scheduler_FIFO.h"
 *   
 * CPU scheduler that implements a FIFO policy
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 */
class CPU_Scheduler_FIFO :public icancloud_Base{


	protected:		 	     
		 	  
		 	  
		/** Number of CPUs */
		unsigned int numCPUs;
		
		/** Request queue array */
	    cQueue requestsQueue;
	    
	    /** Array to show the CPU with an idle state */
	    bool* isCPU_Idle;
		 	     
 		/** Output gate to Service Redirector. */
	    cGate* toOsGate;

	    /** Input gate from Service Redirector. */
	    cGate* fromOsGate;

	    /** Output gates to CPU. */
	    cGate** toCPUGate;

	    /** Input gates from CPU. */
	    cGate** fromCPUGate;
	    
	    /** Queued sms to send to the cpu */
	    unsigned int queueSize;

	   /**
	    * Destructor.
	    */    		
	    ~CPU_Scheduler_FIFO();			
	  	        			  	    	    
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
		 * Search for an idle CPU
		 * @return The index of an idle CPU or NOT_FOUND if all CPU are busy. 
		 */ 
		int searchIdleCPU ();
};

#endif
