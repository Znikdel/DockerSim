#ifndef __SERVER_APPLICATION_H_
#define __SERVER_APPLICATION_H_

#include <omnetpp.h>
#include "Container_UserJob.h"


/**
 * @class ServerApplication ServerApplication.h "ServerApplication.h"
 *
 * Example of a sequential application without traces.
 * This application alternates I/O operations with CPU.
 *
 * This module needs the following NED parameters:
 *
 * - startDelay (Starting delay time)
 * 
 * @author Gabriel González Castañé
 * @date 2013-12-1
 *
 */

#define DEBUG true

class Container_ServerApplication: public Container_UserJob{
	
	protected:

		/** Size of data chunk to read in each iteration */
		int inputSize;

		/** Number of Instructions to execute */
		int MIs;

		/** Starting time delay */
		unsigned int startDelay;

		/** Simulation Starting timestamp */
		simtime_t simStartTime;

		/** Simulation Ending timestamp */
		simtime_t simEndTime;
		
		/** Running starting timestamp */
		time_t runStartTime;

		/** Running ending timestamp */
		time_t runEndTime;		
				
		/** Call Starting timestamp (IO) */
		simtime_t startServiceIO;
		
		/** Call Ending timestamp (IO) */
		simtime_t endServiceIO;
		
		/** Call Starting timestamp (CPU) */
		simtime_t startServiceCPU;
		
		/** Call Ending timestamp (CPU) */
		simtime_t endServiceCPU;
		
		/** Spent time in CPU system */
		simtime_t total_service_CPU;
		
		/** Spent time in IO system */
		simtime_t total_service_IO;
				
		/** Execute CPU */
		bool executeCPU;
		
		/** Execute read operation */
		bool executeRead;
		
		/** Read Offset */
		int readOffset;

        /** Uptime limit*/
        double uptimeLimit;
        int hitsPerHour;
        cMessage *newIntervalEvent;
        double intervalHit;
        int pendingHits;


	   /**
		* Destructor
		*/
		~Container_ServerApplication();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize();

        /**
         * Start the app execution.
         */
	    void startExecution();

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
		* Process a request message.
		* @param sm Request message.
		*/
		void processRequestMessage (icancloud_Message *sm);

		/**
		* Process a message from the cloudManager or the scheduler...
		* @param sm message.
		*/
		void processSchedulingMessage (cMessage *msg);

	   /**
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);


		void changeState(string newState);

	private:			    

		void newHit();
	   /**
		* Method that creates and sends a new I/O request.
		* @param executeRead Executes a read operation
		* @param executeWrite Executes a write operation
		*/
		void serveWebCode();

	   /**
		* Method that creates and sends a CPU request.
		*/
		void executeCPUrequest();

	   /**
		* Print results.
		*/
		void printResults();

};

#endif
