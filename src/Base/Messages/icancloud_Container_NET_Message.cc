//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <icancloud_Container_NET_Message.h>
Register_Class (icancloud_Container_NET_Message);
//icancloud_Container_NET_Message::icancloud_Container_NET_Message() {
    // TODO Auto-generated constructor stub

//}

icancloud_Container_NET_Message::~icancloud_Container_NET_Message() {
    // TODO Auto-generated destructor stub
}



icancloud_Container_NET_Message::icancloud_Container_NET_Message(const char *name, int kind): icancloud_Container_NET_Message_Base(name,kind){

    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_NET_Message");
}


icancloud_Container_NET_Message::icancloud_Container_NET_Message(const icancloud_Container_NET_Message& other): icancloud_Container_NET_Message_Base(other.getName()){

    operator=(other);
    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_NET_Message");
}


icancloud_Container_NET_Message& icancloud_Container_NET_Message::operator=(const icancloud_Container_NET_Message& other){

    icancloud_Container_NET_Message_Base::operator=(other);

    return *this;
}


icancloud_Container_NET_Message *icancloud_Container_NET_Message::dup() const{

    icancloud_Container_NET_Message *newMessage;
    TCPCommand *controlNew;
    TCPCommand *controlOld;
    unsigned int i;

        // Create a new message!
        newMessage = new icancloud_Container_NET_Message();

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

        // icancloud_Container_NET_Message parameters...
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

        // icancloud changing state parameters ..
        newMessage->setChangingState(state);

        for (i=0; i < change_states_index.size(); i++){
            newMessage->add_component_index_To_change_state(change_states_index[i]);
        }

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
        for (i=0; i<trace.size(); i++){
            newMessage->addNodeTrace (trace[i].first, trace[i].second);
        }

    return (newMessage);
}


void icancloud_Container_NET_Message::updateLength (){

    unsigned long int newSize;
    int operation = getOperation();

    // Set the new size!
    newSize = MSG_INITIAL_LENGTH;
    if (!getIsResponse() ) {
        if ((operation == SM_SEND_DATA_NET) || (operation == SM_ITERATIVE_PRECOPY)){
            newSize = MSG_INITIAL_LENGTH + getSize();
        }
    }

    setByteLength (newSize);
}


string icancloud_Container_NET_Message::contentsToString (bool printContents){

    std::ostringstream osStream;

        if (printContents){

            osStream << icancloud_Message::contentsToString(printContents);

            osStream  << " - Local IP:" <<  getLocalIP() << endl;
            osStream  << " - Destination IP:" <<  getDestinationIP() << endl;
            osStream  << " - Local Port:" <<  getLocalPort() << endl;
            osStream  << " - Destination Port:" <<  getDestinationPort() << endl;
            osStream  << " - Size:" <<  getSize() << endl;
            osStream  << " - Data:" <<  getData() << endl;
            osStream  << " - Virtual Local IP:" <<  getVirtual_localIP() << endl;
            osStream  << " - Virtual Destination IP:" <<  getVirtual_destinationIP() << endl;
            osStream  << " - Virtual User:" <<  getVirtual_user() << endl;
            osStream  << " - Virtual Local Port:" <<  getVirtual_localPort() << endl;
            osStream  << " - Virtual Destination Port:" <<  getVirtual_destinationPort() << endl;

        }

    return osStream.str();
}


void icancloud_Container_NET_Message::parsimPack(cCommBuffer *b){

    icancloud_Container_NET_Message_Base::parsimPack(b);
}


void icancloud_Container_NET_Message::parsimUnpack(cCommBuffer *b){

    icancloud_Container_NET_Message_Base::parsimUnpack(b);
}
