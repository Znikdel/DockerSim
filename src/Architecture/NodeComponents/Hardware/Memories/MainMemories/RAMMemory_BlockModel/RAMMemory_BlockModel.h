#ifndef __MAIN_MEMORY_H_
#define __MAIN_MEMORY_H_


#include "HWEnergyInterface.h"
#include "Constants.h"
#include "SMS_MainMemory.h"
#include "icancloud_MemoryBlock.h"

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
#define     IDD5                        "IDD5"

/**
 * @class MainMemory MainMemory.h "MainMemory.h"
 *
 * Class that represents a MainMemory module with read-ahead and write-back funcionality.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 *
 * @author Gabriel Gonzalez Castañe
 * @date 2014-09-09
 */
class RAMMemory_BlockModel : public HWEnergyInterface{

	protected:

		/** Memory Block Size (in KB) */
		unsigned int blockSize_KB;
		
		/** Memory size (in KB)*/
		unsigned int size_KB;
		
		/** Memory size for disk cache (in KB)*/
		unsigned int sizeCache_KB;	

		/** Number of total blocks */
		unsigned int totalBlocks;
		
		/** Number of blocks for application space */
		unsigned int totalAppBlocks;
		
		/** Number of blocks used for application space */
		unsigned int freeAppBlocks;
		
		/** Number of blocks for cache space */
		unsigned int totalCacheBlocks;		
						

		/** readAheadBlocks blocks */
	    unsigned int readAheadBlocks;

	    /** Latency time for read operations (in seconds). */
	    double readLatencyTime_s;
	    
	    /** Latency time for write operations (in seconds). */
	    double writeLatencyTime_s;	    

        /** Latency time for write operations (in seconds). */
        simtime_t searchLatencyTime_s;

	    /** Flush time (in seconds). */
	    double flushTime_s;

        /** Node state */
        string nodeState;

        /** Number of DRAM chips in the same module */
        int numDRAMChips;

        /** Number of connected slots in the node */
        int numModules;

        bool waitingForOperation;
		


	    /** Gate IDs Vector. Output gates to Inputs gates. */
	    cGate* toInputGates;

	    /** Gate IDs Vector. Input gates from Input gates. */
	    cGate* fromInputGates;

	    /** Gate ID. Output gate to Output. */
	    cGate* toOutputGate;

	    /** Gate ID. Input gate from Output. */
	    cGate* fromOutputGate;

	    /** Flush timer */
	    cMessage *flushMessage;

	    /** Pending message */
	    icancloud_Message *pendingMessage;

	    /** SplittingMessageSystem Object*/
	    SMS_MainMemory *SMS_memory;

	    /** Memory block list */
	    list <icancloud_MemoryBlock*> memoryBlockList;

	   	/** Requests Queue */
		cQueue flushQueue;


	   /**
		* Destructor
		*/
		~RAMMemory_BlockModel();

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
		* Send a request to its destination without checking if there are any pending subRequest!
		* @param sm Request message.
		*/
		void sendRequestWithoutCheck (icancloud_Message *sm);

	   /**
		* Process the enqueued subrequests.
		*/
		void processSubRequests ();

	   /**
		* Check if all subRequest corresponding to a concrete Request has arrived.
		* If all subRequest has arrived, then the corresponding Request will vbe sent to App.
		*/
		void arrivesSubRequest (icancloud_App_IO_Message *subRequest);

	   /**
		* Flush all blocks in flushQueue!
		*/
		void flushMemory ();

	   /**
		* Insert a request on flushQueue.
		* If Memory Full, then force the flush!
		*/
		void insertRequestOnFlushQueue (icancloud_App_IO_Message *subRequest);

	   /**
		* Check if memory is full.
		* @return True if memory is full or False if there is any free block.
		*/
		bool isMemoryFull ();

  	   /**
		* Check if all cached blocks are pending.
		* @return True if all cached blocks are pending or false in another case.
		*/
		bool allBlocksPending ();

	   /**
		* Insert the block corresponding to current request (offset-size) at the
		* beginning of the memory.
		*
		* @param fileName File name.
		* @param offset Request offset
		* @param size Request size
		*/
		void insertBlock (string fileName, unsigned int offset, unsigned int size);

	   /**
		* Re-insert the current block at the beginning of the memory.
		*
		* @param block Block to be re-inserted.
		*/
		void reInsertBlock (icancloud_MemoryBlock *block);

	   /**
		* Search for a requested block in memory.
		*
		* @param fileName File name.
		* @param offset Offset
		* @return A pointer to requested block if it is in memory, or NULL in another case.
		*/
	 	icancloud_MemoryBlock * searchMemoryBlock (const char* fileName, unsigned int offset);

	private:

	   /**
		* Parses the memory contents to string!
		* @return Cache list on string format.
		*/
	 	string memoryListToString ();


	   /**
		* Shows the SMS contents. For debug purpose only!
		*/
		void showSMSContents (bool showContents);

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

#endif
