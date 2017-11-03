#include "SMS_MainMemory.h"


SMS_MainMemory::SMS_MainMemory(unsigned int newMemorySize,
							  unsigned int newReadAheadBlocks,
							  unsigned int newMemoryBlockSize){

	memorySize = newMemorySize;
	readAheadBlocks = newReadAheadBlocks;
	memoryBlockSize = newMemoryBlockSize;
}


void SMS_MainMemory::splitRequest(cMessage *msg){

	int offset;									// Request offset
	unsigned int requestSize;							// Request size

	int currentSubRequest;						// Current subRequest (number)
	int currentOffset;							// Current subRequest offset
	int currentSize;							// Current subRequest size;

	int bytesInFirstBlock;						// Bytes of first blocks (write only)
	int firstRequestBlock;						// First requested block
	int numRequestedBlocks;						// Total number of requested blocks

	int operation;

	icancloud_App_IO_Message *sm_io;				// Message (request) to make the casting!
	icancloud_App_IO_Message *subRequestMsg;		// SubRequest message
	icancloud_App_MEM_Message *sm_mem;				// SubRequest message

		// Init...
		currentSubRequest = 0;

		// Cast the original message
		sm_io = dynamic_cast<icancloud_App_IO_Message *>(msg);

		// NET call response...
		if (sm_io != NULL){

			// Get the request message parameters...
			offset = sm_io->getOffset();
			requestSize = sm_io->getSize();

			// Bytes in first block!
			bytesInFirstBlock =	(requestSize<(memoryBlockSize - (offset%memoryBlockSize)))?
								requestSize:
								memoryBlockSize - (offset%memoryBlockSize);

			numRequestedBlocks = ((abs(requestSize - bytesInFirstBlock) % memoryBlockSize) == 0)?
								  (abs(requestSize - bytesInFirstBlock) / memoryBlockSize)+1:
								  (abs(requestSize - bytesInFirstBlock) / memoryBlockSize)+2;

			// Calculate the involved data blocks
			firstRequestBlock = offset / memoryBlockSize;

			operation = sm_io->getOperation();
			// READ?
			if (operation == SM_READ_FILE){

				currentOffset = firstRequestBlock * memoryBlockSize;

				// read the read_ahead blocks!
				numRequestedBlocks+= readAheadBlocks;

				// Add the new Request!
				addRequest (msg, numRequestedBlocks);

				// Create all corresponding subRequests...
				for (currentSubRequest=0;
					 currentSubRequest<numRequestedBlocks;
					 currentSubRequest++){

					// Creates a new subRequest message with the corresponding parameters
					subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
					subRequestMsg->addRequestToTrace (currentSubRequest);
					subRequestMsg->setParentRequest (msg);
					subRequestMsg->setOffset (currentOffset);
					subRequestMsg->setSize (memoryBlockSize);
					subRequestMsg->updateLength();

					// Insert current subRequest!
					subRequests.push_back (subRequestMsg);

					// Update offset
					currentOffset += memoryBlockSize;
				}
			}

			// WRITE?
			else{

				// Add the new Request!
				addRequest (msg, numRequestedBlocks);

				// Set the first requested block!
				subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
				subRequestMsg->addRequestToTrace (currentSubRequest);
				subRequestMsg->setParentRequest (sm_io);
				subRequestMsg->setOffset (offset);
				subRequestMsg->setSize (bytesInFirstBlock);
				subRequestMsg->updateLength();
				subRequests.push_back (subRequestMsg);

				// Update offset!
				currentOffset = offset + bytesInFirstBlock;

				// Create all corresponding subRequests...
				for (currentSubRequest=1;
					 currentSubRequest<numRequestedBlocks;
					 currentSubRequest++){

					// Set the first requested block!
					subRequestMsg = (icancloud_App_IO_Message *) sm_io->dup();
					subRequestMsg->addRequestToTrace (currentSubRequest);
					subRequestMsg->setParentRequest (sm_io);
					subRequestMsg->setOffset (currentOffset);

					// Current subRequest size
					if ((offset+requestSize-currentOffset) >= memoryBlockSize)
						currentSize =  memoryBlockSize;
					else
						currentSize =  offset+requestSize-currentOffset;

					// Set parameters...
					subRequestMsg->setSize (currentSize);
					subRequestMsg->updateLength();

					// Add current subRequest!
					subRequests.push_back (subRequestMsg);

					// Update offset!
					currentOffset += currentSize;
				}
			}
		}

		// Mem message?
		else{

			sm_mem = dynamic_cast<icancloud_App_MEM_Message *>(msg);

			if (sm_mem != NULL){

				addRequest (msg, 1);

				subRequestMsg = (icancloud_App_IO_Message *) sm_mem->dupGeneric();
				subRequestMsg->addRequestToTrace (currentSubRequest);
				subRequestMsg->setParentRequest (msg);
				subRequests.push_back (subRequestMsg);
			}

			else{
				printf ("Wrong message type!\n");
				exit (0);
			}
		}
		
		//printf ("abcde\n%s\n", requestToStringByIndex (requestVector.size()-1).c_str());
}


bool SMS_MainMemory::arrivesRequiredBlocks(cMessage* request, unsigned int extraBlocks){
	
	int parentIndex, subIndex;
	bool allArrived;

		// Search...
		parentIndex = searchRequest (request);
		allArrived = true;
		subIndex = 0;

		// Request found...
		if (parentIndex != NOT_FOUND){
			
			while ((subIndex < ((int)((requestVector[parentIndex])->getNumSubRequest())- ((int)extraBlocks))) && (allArrived)){
				
				if ((requestVector[parentIndex])->getSubRequest(subIndex) == NULL)
					allArrived = false;
				else
					subIndex++;
			}
		}
		else
			allArrived = false;
		
		
	return allArrived;
}


icancloud_App_IO_Message* SMS_MainMemory::getFirstSubRequest(){

	if (subRequests.empty())
		return NULL;
	else
		return (subRequests.front());
}


icancloud_App_IO_Message* SMS_MainMemory::popSubRequest(){

	icancloud_App_IO_Message *msg;

		if (subRequests.empty())
			msg = NULL;
		else{
			msg = subRequests.front();
			subRequests.pop_front();
		}

	return msg;
}


string SMS_MainMemory::requestToStringByIndex (unsigned int index){

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

			sm_io = dynamic_cast<icancloud_App_IO_Message *>((requestVector[index])->getParentRequest());

			if (sm_io != NULL){
			
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
					else{

						// Cast!
						sm_subReq = check_and_cast<icancloud_App_IO_Message *>((requestVector[index])->getSubRequest(i));

						info << "  subRequest[" << i << "]:"
							 << " Offset:" << sm_subReq->getOffset()
							 << " Size:" << sm_subReq->getSize()
							 << endl;
					}
				}
			}
		}

	return info.str();
}


void SMS_MainMemory::clear (){	
	
	std::list<icancloud_App_IO_Message*>::iterator iter;	
	unsigned int i;

	
		for (i=0; i<requestVector.size(); i++){
			(requestVector[i])->clear();
			delete (requestVector[i]);
		}	
			
		requestVector.clear();
		
		// Remove each subRequest message from list!
		for (iter=subRequests.begin(); iter!=subRequests.end(); iter++){

			if ((*iter) != NULL){
				delete (*iter);
				*iter = NULL;
			}
		}

		// Remove the list!
		subRequests.clear();
}


