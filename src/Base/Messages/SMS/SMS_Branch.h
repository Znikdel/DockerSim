#ifndef __SMS_BRANCH_
#define __SMS_BRANCH_

#include <omnetpp.h>
#include <list>
#include "SplittingMessageSystem.h"
#include "icancloud_BlockList_Message.h"


/**
 * @class SMS_Branch SMS_Branch.h "SMS_Branch.h"
 *
 * Class that implements a SplittingMessageSystem abstract class.
 *
 * Specific features of this class:
 *
 * - Performs a linear searching and linear deleting.
 * - Inserts requests following a FIFO algorithm.
 * - Splitting method: Genetares a subrequest per file branch.
 * - Supported message type: icancloud_BlockList_Message
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 02-10-2007
 */
class SMS_Branch: public SplittingMessageSystem {

	protected:

		/** List of subRequests */
		std::list <icancloud_BlockList_Message*> subRequests;


	public:

	   /**
		* Constructor.
		*
		*/
		SMS_Branch ();

	   /**
 		* Given a request size, this function split a request in <b>requestSizeKB</b> KB subRequests.
 		* @param msg Request message.
 		*/
		void splitRequest (cMessage *msg);

	   /**
		* Gets the first subRequest. This method does not remove the subRequest from list.
		* @return First subRequest if list is not empty or NULL if list is empty.
		*/
		icancloud_BlockList_Message* getFirstSubRequest();

	   /**
		* Pops the first subRequest. This method removes the subRequest from list.
		* @return First subRequest if list is not empty or NULL if list is empty.
		*/
		icancloud_BlockList_Message* popSubRequest();

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

#endif /*__SMS_BRANCH_*/
