#ifndef __icancloud_REQUEST__
#define __icancloud_REQUEST__

#include "icancloud_Message.h"
#include "icancloud_App_IO_Message.h"

/**
 * Class that represents a Request.
 */
class icancloud_Request{

	cMessage *parentRequest;			/**< Request request */
	vector <cMessage*> subRequests;		/**< Vector with pointers to subRequests */
	unsigned int arrivedSubRequests;	/**< Number of arrived subRequests */

	
	public:
	   
	   /**
		* Constructor.
		*/
		icancloud_Request ();
		
	   /**
		* Constructor.
		* @param newParent Request message
		*/
		icancloud_Request (cMessage *newParent);
	
		/**
		* Constructor.
		* @param newParent Request message
		* @param numSubReq Number of subRequest
		*/
		icancloud_Request (cMessage *newParent, unsigned int numSubReq);
		
	   /**
		* Destructor.
		*/
		~icancloud_Request ();	
	
		
		cMessage* getParentRequest ();
		void setParentRequest (cMessage* newParent);
		unsigned int getNumSubRequest ();
		unsigned int getNumArrivedSubRequest ();
		void setSubRequest (cMessage* subRequest, unsigned int index);
		void addSubRequest (cMessage* subRequest);
		icancloud_Message* getSubRequest (unsigned int index);	
		icancloud_Message* popSubRequest (unsigned int index);	
		icancloud_Message* popNextSubRequest ();
		bool arrivesAllSubRequest ();
		void arrivesSubRequest (cMessage* subRequest, unsigned int index);
		void clearSubRequests();
		void clear ();
};


#endif /*__icancloud_REQUEST__*/
