#ifndef _RAM_MEMORY_H_
#define _RAM_MEMORY_H_

#include "HWEnergyInterface.h"
#include "Constants.h"

//----------------------------------------- MEMORY ----------------------------------------- //

#define     MEMORY_STATE_IDLE               "memory_idle"                   // Memory OFF
#define     MEMORY_STATE_READ               "memory_read"                   // Memory OFF
#define     MEMORY_STATE_WRITE              "memory_write"                  // Memory OFF
#define     MEMORY_STATE_OFF                "memory_off"                    // Memory ON
#define     MEMORY_STATE_SEARCHING          "memory_search"                     // Memory ON

    //-------------------------------------- DDR3 ------------------------------------------//

#define     IDD0                        "IDD0"                          // Operating current: One bank active-precharge
#define     IDD2P                       "IDD2P"                         // Precharge power-down current (fast PDN Exit)
#define     IDD2N                       "IDD2N"                         // Precharge standby current
#define     IDD3P                       "IDD3P"                         // Active power-down current
#define     IDD3N                       "IDD3N"                         // Active standby current
#define     IDD4R                       "IDD4R"                         // Operating standby current
#define     IDD4W                       "IDD4W"                         // Operating burst write current
#define     IDD5                        "IDD5"                          // Burst refresh current


/**
 * @class RAMMemory RAMMemory.h "RAMMemory.h"
 *
 * Memory module that implements the basic functionality of memory system
 * This module does not use cache blocks.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 *
 * @author Gabriel Gonz&aacute;lez Casta;&ntilde;&eacute;
 * @date 2013-10-27
 */
class RAMmemory : public HWEnergyInterface{

	protected:

		/** Memory Block Size (in KB) */
		unsigned int blockSize_KB;
		
		/** Memory size (in MB)*/
		unsigned int size_MB;

		/** Number of total blocks */
		unsigned int totalBlocks;

		/** Number of blocks used for application space */
		unsigned int freeAppBlocks;

		/** Number of blocks for application space */
		unsigned int totalAppBlocks;

	    /** Latency time for read operations (in seconds). */
		simtime_t readLatencyTime_s;

	    /** Latency time for write operations (in seconds). */
		simtime_t writeLatencyTime_s;
		
		/** Latency time for write operations (in seconds). */
		simtime_t searchLatencyTime_s;

	    /** Gate IDs Vector. Output gates to Inputs gates. */
	    cGate* toInputGates;

	    /** Gate IDs Vector. Input gates from Input gates. */
	    cGate* fromInputGates;

	    /** Gate ID. Output gate to Output. */
	    cGate* toOutputGate;

	    /** Gate ID. Input gate from Output. */
	    cGate* fromOutputGate;

	    /** Pending message */
	    icancloud_Message *pendingMessage;

	   	/** Latency message */
	    cMessage *operationMessage;

	    /** Node state */
	    string nodeState;

		/** Number of DRAM chips in the same module */
		int numDRAMChips;

		/** Number of connected slots in the node */
		int numModules;

		bool waitingForOperation;

	   /**
		* Destructor
		*/
		~RAMmemory();

	   /**
	 	*  Module initialization.
	 	*/
	    void initialize();

	   /**
	 	* Module ending.
	 	*/
	    void finish();

	   /**
		* Process current request message.
		*/
		void processCurrentRequestMessage ();


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

	   /*
		* Send a request to its destination!
		* @param sm Request message.
		*/
		void sendRequest (icancloud_Message *sm);

		/*
		 * Change the energy state of the memory given by node state
		 */
		void changeDeviceState (string state,unsigned componentIndex = 0) ;

		/*
		 *  Change the energy state of the memory
		 */
		void changeState (string energyState,unsigned componentIndex = 0);

public:
		/*
		 * Returns the percent of memory occupation
		 */
		float getMemoryOccupation(){return (((totalBlocks - freeAppBlocks) * 100) / totalBlocks);};

};

#endif /* __BASIC_MAIN_MEMORY_H_ */
