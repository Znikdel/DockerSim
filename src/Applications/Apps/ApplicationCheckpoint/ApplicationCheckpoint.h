#ifndef __APPLICATION_CHECKPOINT_H_
#define __APPLICATION_CHECKPOINT_H_

#include <omnetpp.h>
#include "MPI_Base.h"


/**
 * @class ApplicationCheckpoint ApplicationCheckpoint.h "ApplicationCheckpoint.h"
 *
 * This Application models a generic behavior of a checkpointing application
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 *
 * @updated to iCanCloud by Gabriel González Castañé
 * @date 2012-05-17
 */
class ApplicationCheckpoint : public MPI_Base{

	protected:
	
		unsigned int numIterations;
		unsigned int dataToWrite_KB;
		unsigned int sliceCPU;
		unsigned int currentIteration;
		unsigned int totalResponses;
		
		char outFileName [NAME_SIZE];
		unsigned int currentOffset;
	
	
		/** Simulation Starting timestamp */
		simtime_t simStartTime;

		/** Simulation Ending timestamp */
		simtime_t simEndTime;
		
		/** Running starting timestamp */
		time_t runStartTime;

		/** Running ending timestamp */
		time_t runEndTime;
		
		/** State */
		unsigned int currentState;

		simtime_t totalIO;
		simtime_t ioStart;
		simtime_t ioEnd;

		simtime_t totalCPU;
		simtime_t cpuStart;
		simtime_t cpuEnd;

		
	   /**
		* Destructor
		*/
		~ApplicationCheckpoint();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize();

	   /**
 		* Module ending.
 		*/
	    virtual void finish();

	   /**
		* Process a self message.
		* @param msg Self message.
		*/
		void processSelfMessage (cMessage *msg);
	   
	   /**
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);
		
	   /**
		* This function calculate next state of a given process.
		* 
		*/
		void calculateNextState ();	
		
	   /**
		* Process next iteration
		* 
		*/
		void nextIteration ();		
		
	   /**
		* This function will be invoked when current blocked process can continue its execution.		
		* 
		*/
		void continueExecution ();	

	private:
	
	   /**
		* Worker processes processing CPU.
		*/
		void processingCPU();	
		
	   /**
		* Master process writes the data of current iteration
		*/
		void writtingData();
	
	   /**
		* Process a NET call response.
		* 
		* @param responseMsg Response message.
		*/
		void processNetCallResponse (icancloud_App_NET_Message *responseMsg);
		
		
	   /**
		* Process an IO call response.
		* 
		* @param responseMsg Response message.
		*/
		void processIOCallResponse (icancloud_App_IO_Message *responseMsg);
		
	   /**
		* Process a CPU call response.
		* 
		* @param responseMsg Response message.
		*/
		void processCPUCallResponse (icancloud_App_CPU_Message *responseMsg);
	
		/**
		 * Parses a state to a string format.
		 * 
		 * @return State in string format.
		 */
		string stateToString(int state);
		
		/**
		 * Show results of the simulation
		 */
		void showResults ();
};

#endif
