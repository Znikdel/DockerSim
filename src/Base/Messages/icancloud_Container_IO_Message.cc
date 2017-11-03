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

#include <icancloud_Container_IO_Message.h>

//icancloud_Container_IO_Message::icancloud_Container_IO_Message() {
    // TODO Auto-generated constructor stub

//}

icancloud_Container_IO_Message::~icancloud_Container_IO_Message() {
    // TODO Auto-generated destructor stub
}

icancloud_Container_IO_Message::icancloud_Container_IO_Message(const char *name, int kind): icancloud_Container_IO_Message_Base(name,kind){

    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_IO_Message");
}


icancloud_Container_IO_Message::icancloud_Container_IO_Message(const icancloud_Container_IO_Message& other): icancloud_Container_IO_Message_Base(other.getName()){

    operator=(other);
    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_IO_Message");
}


icancloud_Container_IO_Message& icancloud_Container_IO_Message::operator=(const icancloud_Container_IO_Message& other){

    icancloud_Container_IO_Message_Base::operator=(other);

    return *this;
}


icancloud_Container_IO_Message *icancloud_Container_IO_Message::dup() const{

    icancloud_Container_IO_Message *newMessage;
    TCPCommand *controlNew;
    TCPCommand *controlOld;
    int i;

        // Create a new message!
        newMessage = new icancloud_Container_IO_Message();

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

        // icancloud_Container_IO_Message parameters...
        newMessage->setFileName (getFileName());
        newMessage->setOffset (getOffset());
        newMessage->setSize (getSize());

        newMessage->setNfs_destAddress(getNfs_destAddress());
        newMessage->setNfs_destPort(getNfs_destPort());
        newMessage->setNfs_requestSize_KB(getNfs_requestSize_KB());
        newMessage->setNfs_connectionID(getNfs_connectionID());
        newMessage->setNfs_type(getNfs_type());
        newMessage->setNfs_connectionID(getNfs_connectionID());

        // icancloud changing state parameters ..
        newMessage->setChangingState(state);
        for (i=0; i< ((int)change_states_index.size()); i++){
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
        for (i=0; i<((int)trace.size()); i++){
            newMessage->addNodeTrace (trace[i].first, trace[i].second);
        }

    return (newMessage);
}


void icancloud_Container_IO_Message::updateLength (){

    unsigned long int newSize;
    std::ostringstream osStream;
    int operation = getOperation();

        // Request!
        if (!getIsResponse()){

            // READ
            if (operation == SM_READ_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // WRITE
            else if (operation == SM_WRITE_FILE)
                newSize = MSG_INITIAL_LENGTH + getSize();

            // OPEN
            else if (operation == SM_OPEN_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // CLOSE
            else if (operation == SM_CLOSE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // CREATE
            else if (operation == SM_CREATE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // DELETE
            else if (operation == SM_DELETE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            else if (operation == SM_SET_IOR)
                newSize = MSG_INITIAL_LENGTH;

            // Unknown operation!
            else{
                osStream << "Error updating icancloud_Container_IO_Message length (request). Wrong operation:"
                         << operationToString (operation);
                throw cRuntimeError(osStream.str().c_str());
            }
        }

        // Response!
        else{

            // READ
            if (operation == SM_READ_FILE)
                newSize = MSG_INITIAL_LENGTH + getSize();

            // WRITE
            else if (operation == SM_WRITE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // OPEN
            else if (operation == SM_OPEN_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // CLOSE
            else if (operation == SM_CLOSE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // CREATE
            else if (operation == SM_CREATE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // DELETE
            else if (operation == SM_DELETE_FILE)
                newSize = MSG_INITIAL_LENGTH;

            // Unknown operation!
            else{
                osStream << "Error updating SIMCAM_App_IO_Message length (request). Wrong operation:"
                         << operationToString (operation);
                throw cRuntimeError(osStream.str().c_str());
            }
        }


    // Set the new size!
    setByteLength (newSize);
}


string icancloud_Container_IO_Message::getIsPending_string(){

    string result;

        if (getIsPending())
            result = "true";
        else
            result = "false";

    return result;
}


string icancloud_Container_IO_Message::contentsToString (bool printContents){

    std::ostringstream osStream;

        if (printContents){

            osStream << icancloud_Message::contentsToString(printContents);

            osStream  << " - fileName:" <<  getFileName() << endl;
            osStream  << " - offset:" <<  getOffset() << endl;
            osStream  << " - size:" <<  getSize() << endl;
            osStream  << " - operation:" <<  operationToString(getOperation()) << endl;
//          osStream  << " - nfs_destAddress_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfs_destPort_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfs_id_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfs_type_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfs_requestSize_KB_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfsCell_index_var:" <<  getIsPending_string() << endl;
//          osStream  << " - nfs_connectionID_var:" <<  getIsPending_string() << endl;

        }

    return osStream.str();
}


void icancloud_Container_IO_Message::parsimPack(cCommBuffer *b){

    icancloud_Container_IO_Message_Base::parsimPack(b);
}


void icancloud_Container_IO_Message::parsimUnpack(cCommBuffer *b){

    icancloud_Container_IO_Message_Base::parsimUnpack(b);
}

