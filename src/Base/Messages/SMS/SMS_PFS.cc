#include "SMS_PFS.h"

const unsigned int SMS_PFS::meta_dataKB = 1;


SMS_PFS::SMS_PFS (unsigned int newStrideSizeKB, unsigned int newNumServers){
	strideSize = newStrideSizeKB * KB;
	numServers = newNumServers;
	currentCommId = 0;
}
SMS_PFS::SMS_PFS(){
   strideSize = 0;
   numServers = 0;
   currentCommId = 0;

}
SMS_PFS::~SMS_PFS(){

}

void SMS_PFS::splitRequest (cMessage *msg){

	unsigned int i;
	icancloud_App_IO_Message *sm_io;			// IO Request message
	icancloud_App_IO_Message *subRequestMsg;	// SubRequests message!

	unsigned int numberOfsubRequest;		// Number of stripped request
	unsigned int requestSize;				// Request Size
	unsigned int subRequestOffset;			// SubRequest offset
	unsigned int subRequestSize;			// Subrequest Size

	int operation;

		// Cast!
    	sm_io = check_and_cast<icancloud_App_IO_Message *>(msg);
        operation = sm_io->getOperation();

		// Open File
		if (operation == SM_OPEN_FILE){

			// Add the new Request!
			addRequest (msg, numServers);

			// One OPEN request per server...
			for (i=0; i<numServers; i++){

				// Copy the message and set new values!
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);

				// Read the metadata in each node!
				subRequestMsg->setOffset (0);
	    		subRequestMsg->setSize(meta_dataKB*KB);
	    		subRequestMsg->setOperation(SM_READ_FILE);

	    		// Set the corresponding server
	    		subRequestMsg->setConnectionId(i);
	    		subRequestMsg->setNfs_connectionID(currentCommId);
	    		// Link current subRequest with its parent request
	    		//setSubRequest (msg, subRequestMsg, i);

	    		// Set message length
	    		subRequestMsg->updateLength();

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (i);
	    		
	    		// Insert current subRequest!
	    		subRequests.push_back (subRequestMsg);
			}
		}

		// Create File (2 messages per server: create+write_metaData)
		else if (operation == SM_CREATE_FILE){

			// Add the new Request!
			addRequest (msg, 2*numServers);

			// Two requests per server...
			for (i=0; i<numServers; i++){

				// Copy the message and set new values! (Create message)
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);

	    		// Set the corresponding server
	    		subRequestMsg->setConnectionId (i);
	    		subRequestMsg->setNfs_connectionID(currentCommId);
	    		// Set message length
	    		subRequestMsg->updateLength();

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (i);

	    		// Link current subRequest with its parent request
	    		//setSubRequest (msg, subRequestMsg, i);
	    		
	    		// Insert current subRequest!
	    		subRequests.push_back (subRequestMsg);

	    		// Write message...
	    		subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);

				// Read the metadata in each node!
				subRequestMsg->setOffset (0);
	    		subRequestMsg->setSize(meta_dataKB*KB);
	    		subRequestMsg->setOperation(SM_WRITE_FILE);

	    		// Set the corresponding server
	    		subRequestMsg->setConnectionId (i);
	    		subRequestMsg->setNfs_connectionID(currentCommId);
	    		// Set message length
	    		subRequestMsg->updateLength();

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (i+numServers);

	    		// Link current subRequest with its parent request
	    		//setSubRequest (msg, subRequestMsg, i+numServers);
	    		
	    		// Insert current subRequest!
	    		subRequests.push_back (subRequestMsg);
			}
		}

		// Delete File
		else if (operation == SM_DELETE_FILE){

			// Add the new Request!
			addRequest (msg, numServers);

			// One OPEN request per server...
			for (i=0; i<numServers; i++){

				// Copy the message and set new values!
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);

	    		// Set the corresponding server
	    		subRequestMsg->setConnectionId (i);
	    		subRequestMsg->setNfs_connectionID(currentCommId);
	    		// Link current subRequest with its parent request
	    		setSubRequest (msg, subRequestMsg, i);

	    		// Set message length
	    		subRequestMsg->updateLength();

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (i);
	    		
	    		// Insert current subRequest!
	    		subRequests.push_back (subRequestMsg);
			}
		}

		// Read/Write File
		else if ((operation == SM_READ_FILE) ||
				 (operation == SM_WRITE_FILE)){

			// Get request size...
			requestSize = sm_io->getByteLength();
			//	    	requestSize = sm_io->getSize();

			// Calculate the number of subRequests
			numberOfsubRequest = ((requestSize%strideSize)==0)?
								  (requestSize/strideSize):
								  (requestSize/strideSize)+1;
			numberOfsubRequest = 4;
			// Add the new Request!
		    addRequest (msg, numberOfsubRequest);

			// First offset
			subRequestOffset = sm_io->getOffset();

			// create all subRequests
			for (i=0; i<numberOfsubRequest; i++){

				// Copy the message and set new values!
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->setParentRequest (msg);

				//  Calculates the current subRequest size
				subRequestSize = (requestSize>=strideSize)?
									strideSize:
									requestSize;

				// Read the metadata in each node!
				subRequestMsg->setOffset (subRequestOffset);
	    		subRequestMsg->setSize (subRequestSize);

	    		// Set the corresponding server
                  subRequestMsg->setConnectionId (i%numServers);
                  subRequestMsg->setCommId(i%numServers);
                  subRequestMsg->setNfs_connectionID(i%numServers);

	    		//printf ("Adding new request. Offset:%d - Size:%d - to server:%d\n", subRequestOffset, subRequestSize, i%numServers);

	    		// Link current subRequest with its parent request
	    		setSubRequest (msg, subRequestMsg, i);

	    		// Set message length
	    		subRequestMsg->updateLength();

	    		// Update the current subRequest Message ID...
	    		subRequestMsg->addRequestToTrace (i);

	    		// update offset!
	    		subRequestOffset+=subRequestSize;
	    		requestSize-=subRequestSize;
	    		
	    		// Insert current subRequest!
	    		subRequests.push_back (subRequestMsg);
			}
		}
}


icancloud_App_IO_Message* SMS_PFS::getFirstSubRequest(){

	if (subRequests.empty())
		return NULL;
	else
		return (subRequests.front());
}


icancloud_App_IO_Message* SMS_PFS::popSubRequest(){

	icancloud_App_IO_Message *msg;

		if (subRequests.empty())
			msg = NULL;
		else{
			msg = subRequests.front();
			subRequests.pop_front();
		}

	return msg;
}


string SMS_PFS::requestToStringByIndex (unsigned int index){

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
				 << " File:" << sm_io->getFileName()				 << " Offset:" << sm_io->getOffset()
				 <<	" Size:" << sm_io->getByteLength()
				 <<	" subRequests:" << numSubRequest
				 << endl;

			// Get info of all subRequests...
			for (i=0; i<numSubRequest; i++){

				// Is NULL?
				if ((requestVector[index])->getSubRequest(i) == NULL)
					info << "  subRequest[" << i << "]: Not arrived yet!" << endl;
				else{

					// Cast!
					sm_subReq = check_and_cast<icancloud_App_IO_Message *>((requestVector[index])->getSubRequest(i));

					info << "  subRequest[" << i << "]:"
						 << " Op:" << sm_io->operationToString()
						 << " Offset:" << sm_subReq->getOffset()
						 << " Size:" << sm_subReq->getSize()
						 << " CommunicationId:" << sm_subReq->getConnectionId()
						 << " appIndex:" << sm_subReq->getNextModuleIndex()
						 << endl;
				}
			}
		}

	return info.str();
}

void SMS_PFS::clear (){
	
	unsigned int i;
	
		for (i=0; i<requestVector.size(); i++){
			delete (requestVector[i]);
			requestVector[i] = NULL;
		}	
	
	requestVector.clear();
}

