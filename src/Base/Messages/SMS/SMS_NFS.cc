#include "SMS_NFS.h"


SMS_NFS::SMS_NFS (int requestSize_KB){
	requestSizeNFS = requestSize_KB*KB;
}


void SMS_NFS::splitRequest (cMessage *msg){

	int currentSubRequest;					// Current subRequest
	int numberOfsubRequest;					// Total number of subrequest

	int requestSize;						// Request size!
	int currentOffset;						// Current subRequest offset
	int currentSubRequestSize;				// Current subRequest size
	int currentSize;						// Current accumulated size, 0 to requestSize

	icancloud_App_IO_Message *sm_io;			// Message (request) to make the casting!
	icancloud_App_IO_Message *subRequestMsg;	// SubRequests message!

	int operation;

		// Init...
		numberOfsubRequest = currentSubRequest = currentSubRequestSize = 0;
		currentOffset = currentSize = requestSize = 0;

		// Cast the original message
		sm_io = check_and_cast<icancloud_App_IO_Message *>(msg);
		operation = sm_io->getOperation();

		// Read or write operation! Calculate number of subRequests!
		if ((operation == SM_READ_FILE) ||
	   	    (operation == SM_WRITE_FILE)){

	   	    // Get the offset and size!
//			requestSize = sm_io->getByteLength();
	    	requestSize = sm_io->getSize();
	    	currentOffset = sm_io->getOffset();

			// Calculate the number of subRequests
			numberOfsubRequest = ((requestSize%requestSizeNFS)==0)?
								  (requestSize/requestSizeNFS):
								  (requestSize/requestSizeNFS)+1;

	   	}

	   	// only 1 message, do not split!
	   	else
	   		numberOfsubRequest = 1;

		// Add the new Request!

		addRequest (msg, numberOfsubRequest);

		// Read or write operation!
		if ((operation == SM_READ_FILE) ||
	   	    (operation == SM_WRITE_FILE)){

			// Generate the subRequest!
	   	    for (currentSubRequest=0; currentSubRequest<numberOfsubRequest ; currentSubRequest++){

				// Calculates current subRequest size
	   	        currentSubRequestSize = ((requestSize-currentSize) >= requestSizeNFS)?requestSizeNFS:(requestSize-currentSize);


				// Copy the message and set new values!
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);
				subRequestMsg->setOffset (currentOffset);
				subRequestMsg->setByteLength(currentSubRequestSize);
//	    		subRequestMsg->setSize(currentSubRequestSize);

	    		// Link current subRequest with its parent request
	    		setSubRequest (msg, subRequestMsg, currentSubRequest);

    			// Update current request size part!
    			currentSize+=currentSubRequestSize;
    			currentOffset+=currentSubRequestSize;

    			// Set subRequest message length
    			if (operation == SM_READ_FILE)
					subRequestMsg->setByteLength (SM_NFS2_READ_REQUEST);
				else if (operation == SM_WRITE_FILE)
					subRequestMsg->setByteLength (SM_NFS2_WRITE_REQUEST + currentSubRequestSize);

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (currentSubRequest);
   	    	}
   		}

	   	// Do not split the message!
	   	else if ((operation == SM_CREATE_FILE) ||
	   	    	 (operation == SM_DELETE_FILE) ||
	   	    	 (operation == SM_OPEN_FILE) ||
	   	     	 (operation == SM_CLOSE_FILE)){

	    		// Copy the message!
	    		subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
	    		subRequestMsg->setParentRequest (msg);

	    		// Link current subRequest with its parent request
	    		setSubRequest (msg, subRequestMsg, 0);

				// Set subRequest message length
				if (operation == SM_CREATE_FILE)
					subRequestMsg->setByteLength (SM_NFS2_CREATE_REQUEST);
				else if (operation == SM_DELETE_FILE)
					subRequestMsg->setByteLength (SM_NFS2_DELETE_REQUEST);
				else if (operation == SM_OPEN_FILE)
					subRequestMsg->setByteLength (SM_NFS2_OPEN_REQUEST);
				else if (operation == SM_CLOSE_FILE)
					subRequestMsg->setByteLength (SM_NFS2_CLOSE_REQUEST);

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (currentSubRequest);
			}
}


string SMS_NFS::requestToStringByIndex (unsigned int index){

	std::ostringstream info;
	int i;
	int numSubRequest;
	icancloud_App_IO_Message *sm_subReq;
	icancloud_App_IO_Message *sm_io;


		// Request not found...
		if ((index>=requestVector.size()) || (index<0)){
			info << "Request" << index << " Not Found!" << endl;
		}

		// Request found!
		else{

			sm_io = check_and_cast<icancloud_App_IO_Message *>((requestVector[index])->getParentRequest());

			// Get the number of subRequests
			numSubRequest = getNumberOfSubRequest (sm_io);

			// Original request info...
			info << " Op:" << sm_io->operationToString()
				 << " File:" << sm_io->getFileName()
				 << " Offset:" << sm_io->getOffset()
				 <<	" Size:" << sm_io->getSize()
				 <<	" subRequests:" << numSubRequest
				 << endl;

			// Get info of all subRequests...
			for (i=0; i<numSubRequest; i++){

				// Is NULL?
				if ((requestVector[index])->getSubRequest(i) == NULL)
					info << "  subRequest[" << i << "]: Not arrived yet!" << endl;

				// SubRequest is here!!!
				else{

					// Cast!
					sm_subReq = check_and_cast<icancloud_App_IO_Message *>((requestVector[index])->getSubRequest(i));

					// Has already arrived?
					if (!sm_subReq->getIsResponse())
						info << "  subRequest[" << i << "]: Not sent yet!" << endl;
					else
						info << "  subRequest[" << i << "]:"
							 << " Op:" << sm_subReq->operationToString()
							 << " Offset:" << sm_subReq->getOffset()
							 << " Size:" << sm_subReq->getSize()
							 << endl;
				}
			}
		}

	return info.str();
}


void SMS_NFS::clear (){
	
	unsigned int i;
	
		for (i=0; i<requestVector.size(); i++){
			(requestVector[i])->clear();
			delete (requestVector[i]);
		}

	requestVector.clear();
}

