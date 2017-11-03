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
#include "icancloud_Container_MEM_Message.h"
#include "icancloud_Container_IO_Message.h"


Register_Class (icancloud_Container_MEM_Message);

//icancloud_Container_MEM_Message::icancloud_Container_MEM_Message() {
    // TODO Auto-generated constructor stub

//}

icancloud_Container_MEM_Message::~icancloud_Container_MEM_Message() {
    // TODO Auto-generated destructor stub
}


icancloud_Container_MEM_Message::icancloud_Container_MEM_Message(const char *name, int kind): icancloud_Container_MEM_Message_Base(name,kind){

    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_MEM_Message");
}


icancloud_Container_MEM_Message::icancloud_Container_MEM_Message(const icancloud_Container_MEM_Message& other): icancloud_Container_MEM_Message_Base(other.getName()){

    operator=(other);
    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_MEM_Message");
}


icancloud_Container_MEM_Message& icancloud_Container_MEM_Message::operator=(const icancloud_Container_MEM_Message& other){

    icancloud_Container_MEM_Message_Base::operator=(other);

    return *this;
}


icancloud_Container_MEM_Message *icancloud_Container_MEM_Message::dup() const{

    icancloud_Container_MEM_Message *newMessage;
    TCPCommand *controlNew;
    TCPCommand *controlOld;
    unsigned int i;

        // Create a new message!
        newMessage = new icancloud_Container_MEM_Message();

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

        // icancloud_Container_MEM_Message parameters...
        newMessage->setMemSize (getMemSize());
        newMessage->setRegion (getRegion());

        // icancloud changing state parameters ..
        newMessage->setChangingState(state);
        for (i=0; i< change_states_index.size(); i++){
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


icancloud_Message *icancloud_Container_MEM_Message::dupGeneric() const{

    icancloud_Container_IO_Message *newMessage;
    unsigned int i;

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

        // Copy the control info, if exists!
        //if (getControlInfo() != NULL){
            //newMessage->setControlInfo (getControlInfo());
        //}

        // Reserve memory to trace!
        newMessage->setTraceArraySize (getTraceArraySize());

        // Copy trace!
        for (i=0; i<trace.size(); i++){
            newMessage->addNodeTrace (trace[i].first, trace[i].second);
        }

    return (newMessage);
}


void icancloud_Container_MEM_Message::updateLength (){

        // Set the new size!
        setByteLength (MSG_INITIAL_LENGTH);
}


string icancloud_Container_MEM_Message::contentsToString (bool printContents){

    std::ostringstream osStream;

        if (printContents){

            osStream << icancloud_Message::contentsToString(printContents);
            osStream  << " - Requested mem size:" <<  getMemSize() << endl;
            osStream  << " - Memory region:" <<  regionToString(getRegion()) << endl;
        }

    return osStream.str();
}


string icancloud_Container_MEM_Message::regionToString (int region){

    string result;

        if (region == SM_MEMORY_REGION_CODE)
            result = "SM_MEMORY_REGION_CODE";

        else if (region == SM_MEMORY_REGION_LOCAL_VAR)
            result = "SM_MEMORY_REGION_LOCAL_VAR";

        else if (region == SM_MEMORY_REGION_GLOBAL_VAR)
            result = "SM_MEMORY_REGION_GLOBAL_VAR";

        else if (region == SM_MEMORY_REGION_DYNAMIC_VAR)
            result = "SM_MEMORY_REGION_DYNAMIC_VAR";

        else
            result = "REGION not found!";

    return result;
}


void icancloud_Container_MEM_Message::parsimPack(cCommBuffer *b){

    icancloud_Container_MEM_Message_Base::parsimPack(b);
}


void icancloud_Container_MEM_Message::parsimUnpack(cCommBuffer *b){

    icancloud_Container_MEM_Message_Base::parsimUnpack(b);
}



