#include "icancloud_MPI_Message.h"

Register_Class(icancloud_MPI_Message);

icancloud_MPI_Message::icancloud_MPI_Message(const char *name, int kind): icancloud_MPI_Message_Base(name,kind){

	setByteLength (MSG_INITIAL_LENGTH);
	setName ("icancloud_MPI_Message");
}


icancloud_MPI_Message::icancloud_MPI_Message(const icancloud_MPI_Message& other): icancloud_MPI_Message_Base(other.getName()){

	operator=(other);
	setByteLength (MSG_INITIAL_LENGTH);
	setName ("icancloud_MPI_Message");
}


icancloud_MPI_Message& icancloud_MPI_Message::operator=(const icancloud_MPI_Message& other){

	icancloud_MPI_Message_Base::operator=(other);

	return *this;
}


icancloud_MPI_Message *icancloud_MPI_Message::dup() const{

	icancloud_MPI_Message *newMessage;
	TCPCommand *controlNew;
	TCPCommand *controlOld;

	int i;

		// Create a new message!
		newMessage = new icancloud_MPI_Message();

		// Base parameters...
        newMessage->setOperation (getOperation());
        newMessage->setIsResponse (getIsResponse());
        newMessage->setRemoteOperation (getRemoteOperation());
        newMessage->setConnectionId (getConnectionId());
        newMessage->setCommId(getCommId());
        newMessage->setSourceId (getSourceId());
        newMessage->setNextModuleIndex (getNextModuleIndex());
        newMessage->setResult (getResult());
        newMessage->setUid(getUid());
        newMessage->setPid(getPid());
        newMessage->setByteLength (getByteLength());
        newMessage->setParentRequest (getParentRequest());

        // icancloud_App_NET_Message parameters...
        newMessage->setDestinationIP (getDestinationIP());
        newMessage->setDestinationPort (getDestinationPort());
        newMessage->setLocalIP (getLocalIP());
        newMessage->setLocalPort (getLocalPort());
        newMessage->setSize (getSize());
        newMessage->setData (getData());

        // User
        newMessage->setVirtual_user(getVirtual_user());

        // IPs
        newMessage->setVirtual_destinationIP(getVirtual_destinationIP());
        newMessage->setVirtual_localIP(getVirtual_localIP());

        // Ports..
        newMessage->setVirtual_destinationPort(getVirtual_destinationPort());
        newMessage->setVirtual_localPort(getVirtual_localPort());

        newMessage->setFsType(getFsType());
        newMessage->setTargetPosition(getTargetPosition());

		// icancloud_MPI_Message parameters...
		newMessage->setSourceRank (getSourceRank());
		newMessage->setDestRank (getDestRank());
		newMessage->setFileName (getFileName());
		newMessage->setOffset (getOffset());			
		
		// Copy the control info, if exists!
		if (getControlInfo() != NULL){
			controlOld = check_and_cast<TCPCommand *>(getControlInfo());
			controlNew = new TCPCommand();
			controlNew = controlOld->dup();
			newMessage->setControlInfo (controlNew);
		}
		
		// Reserve memory to trace!
		newMessage->setTraceArraySize (getTraceArraySize());

		// Copy trace!
		for (i=0; i<((int)trace.size()); i++){
			newMessage->addNodeTrace (trace[i].first, trace[i].second);
		}

	return (newMessage);
}


string icancloud_MPI_Message::contentsToString (bool printContents){

    std::ostringstream osStream;

        if (printContents){

            osStream << icancloud_Message::contentsToString(printContents);

            osStream  << " - Local IP:" <<  getLocalIP() << endl;
            osStream  << " - Destination IP:" <<  getDestinationIP() << endl;
            osStream  << " - Local Port:" <<  getLocalPort() << endl;
            osStream  << " - Destination Port:" <<  getDestinationPort() << endl;

            osStream  << " - Virtual Local IP:" <<  getVirtual_localIP() << endl;
            osStream  << " - Virtual Destination IP:" <<  getVirtual_destinationIP() << endl;
            osStream  << " - Virtual User:" <<  getVirtual_user() << endl;
            osStream  << " - Virtual Local Port:" <<  getVirtual_localPort() << endl;
            osStream  << " - Virtual Destination Port:" <<  getVirtual_destinationPort() << endl;

            osStream  << " - source rank:" <<  getSourceRank() << endl;
            osStream  << " - destination rank:" <<  getDestRank() << endl;
            osStream  << " - file name:" <<  getFileName() << endl;
            osStream  << " - offset:" <<  getOffset() << endl;
        }

    return osStream.str();
}
