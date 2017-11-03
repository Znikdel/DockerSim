#ifndef __SMS_MAIN_MEMORY_
#define __SMS_MAIN_MEMORY_

#include <omnetpp.h>
#include <list>
#include "SplittingMessageSystem.h"
#include "icancloud_App_IO_Message.h"
#include "icancloud_App_MEM_Message.h"


/**
 * @class SMS_MainMemory SMS_MainMemory.h "SMS_MainMemory.h"
 *
 * Class that implements a SplittingMessageSystem abstract class.
 *
 * Specific features of this class:
 *
 * - Performs a linear searching and linear deleting.
 * - Inserts requests following a FIFO algorithm.
 * - Splitting method: Genetares a subrequest per memory block size.
 * - Supported message type: icancloud_App_IO_Message
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 02-10-2007
 */
class SMS_MainMemory : public SplittingMessageSystem {

	protected:

		/** Memory size (in bytes)*/
		unsigned int memorySize;

		/** ReadAheadBlocks blocks */
	    unsigned int readAheadBlocks;

	    /** Memory block size (in bytes) */
		unsigned int memoryBlockSize;

		/** List of subRequests */
		std::list <icancloud_App_IO_Message*> subRequests;


	public:

	   /**
		* Constructor.
		*
		* @param newCacheSize newCacheSize Cache size (in bytes)
		* @param newReadAheadBlocks Number of blocks to pre-fetching (read ahead)
		* @param newCacheBlockSize Cache block size (in bytes)
		*/
		SMS_MainMemory (unsigned int newCacheSize, unsigned int newReadAheadBlocks, unsigned int newCacheBlockSize);

	   /**
 		* Given a request size, this function split a request in <b>requestSizeKB</b> KB subRequests.
 		* @param msg Request message.
 		*/
		void splitRequest (cMessage *msg);
		
	   /**
		* Calculates if required block from the original request have arrived.
		* This blocks not take into account the read-ahead blocks.
		* @param request Original request.
		* @param extraBlocks Number of read-ahead blocks.
		* @return true if original request blocks have been arrived, or false in another case..
		*/
		bool arrivesRequiredBlocks(cMessage* request, unsigned int extraBlocks);

	   /**
		* Gets the first subRequest. This method does not remove the subRequest from list.
		* @return First subRequest if list is not empty or NULL if list is empty.
		*/
		icancloud_App_IO_Message* getFirstSubRequest();

	   /**
		* Pops the first subRequest. This method removes the subRequest from list.
		* @return First subRequest if list is not empty or NULL if list is empty.
		*/
		icancloud_App_IO_Message* popSubRequest();

	   /**
		* Parses a request (with the corresponding sub_request) to string.
		* IMPORTANT: This function must be called next to <b>splitRequest</b>.
		* If not, maybe some sub_requests not been linked to original request.
		* This function is for debugging purpose only.
		*
		* @param index Position ov the requested request.
		* @return A string with the corresponding request info.
		*/
		string requestToStringByIndex (unsigned int index);
		
		
	   /**
 		* Removes all requests
 		*/
		void clear ();
};

#endif /*__SMS_BLOCKSIZE_*/
