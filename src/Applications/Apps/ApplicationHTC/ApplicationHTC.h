#ifndef __APPLICATION_HTC_H_
#define __APPLICATION_HTC_H_

#include <omnetpp.h>
#include "MPI_Base.h"

/**
 * @class ApplicationHTC ApplicationHTC.h "ApplicationHTC.h"
 *
 * This Application models a generic behavior of a HTC application
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 *
 * @updated to iCanCloud by Gabriel González Castañé
 * @date 2012-05-17
 *
 */
class ApplicationHTC : public MPI_Base{

	protected:		
	
		/** Start delay time */
	  	double startDelay_s;
		
		/** Number of files to be processed */
		unsigned int numProcessedFiles;
		
		/** Suffix of the file name */
		string fileNameSuffix;
		
		/** Size of each file */
		unsigned int fileSize_MB;
		
		/** Size of results */
		unsigned int resultsSize_MB;
		
		/** Size of each file */
		unsigned int cpuMIs;	
		
		/** State */
		unsigned int currentState;		
	
		/** Number of remaining files */
		unsigned int currentFile;	
	
		/** Simulation Starting timestamp */
		simtime_t simStartTime;

		/** Simulation Ending timestamp */
		simtime_t simEndTime;
		
		/** Running starting timestamp */
		time_t runStartTime;

		/** Running ending timestamp */
		time_t runEndTime;
		
	
		
	   /**
		* Destructor
		*/
		~ApplicationHTC();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize();

        /*
         * Starts the app execution
         */
        virtual void startExecution();

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
		* @param msg request message.
		*/
		void processRequestMessage (icancloud_Message *sm);
	   
	   /**
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);

       /**
        * This function will be invoked when current blocked process can continue its execution.
        *
        */
        void continueExecution ();

	private:
	
	   /**
		* Execute next state. 
		*/
		void executeNextState();
		
	   /**
	    * Process next file
		*/
		void processNextFile();
		
	   /**
		* Open file
		*/
		void openFile();
		
	   /**
		* Master reads the corresponding data from input dataset.
		*/
		void readInputData();		
	
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
		* Get the current input file name
		* @param currentFileNumber Current file
		* @return Current file name
		*/
		string getCurrentFileName (unsigned int currentFileNumber);

	   /**
		* Get the results file name
		* @return Results file name
		*/
		string getResultsFileName();
			
		/**
		 * Show results of the simulation
		 */
		void showResults ();
};

#endif
