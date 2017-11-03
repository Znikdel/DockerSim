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
#include "icancloud_Container_CPU_Message.h"

Register_Class (icancloud_Container_CPU_Message);

//icancloud_Container_CPU_Message::icancloud_Container_CPU_Message() {
 //   // TODO Auto-generated constructor stub

//}

icancloud_Container_CPU_Message::~icancloud_Container_CPU_Message() {
    // TODO Auto-generated destructor stub
}

icancloud_Container_CPU_Message::icancloud_Container_CPU_Message(const char *name, int kind): icancloud_Container_CPU_Message_Base(name,kind){

    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_CPU_Message");
}


icancloud_Container_CPU_Message::icancloud_Container_CPU_Message(const icancloud_Container_CPU_Message& other): icancloud_Container_CPU_Message_Base(other.getName()){

    operator=(other);
    setByteLength (MSG_INITIAL_LENGTH);
    setName ("icancloud_Container_CPU_Message");
}


icancloud_Container_CPU_Message& icancloud_Container_CPU_Message::operator=(const icancloud_Container_CPU_Message& other){

    icancloud_Container_CPU_Message_Base::operator=(other);

    return *this;
}


icancloud_Container_CPU_Message *icancloud_Container_CPU_Message::dup() const{

    icancloud_Container_CPU_Message *newMessage;
    TCPCommand *controlNew;
    TCPCommand *controlOld;
    unsigned int i;

        // Create a new message!
        newMessage = new icancloud_Container_CPU_Message();

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

        // icancloud_Container_CPU_Message parameters...
        newMessage->setCpuTime (getCpuTime());
        newMessage->setTotalMIs (getTotalMIs());
        newMessage->setRemainingMIs (getRemainingMIs());
        newMessage->setCpuPriority (getCpuPriority());
        newMessage->setQuantum (getQuantum());

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


void icancloud_Container_CPU_Message::updateLength (){

        // Set the new size!
        setByteLength (MSG_INITIAL_LENGTH);
}


void icancloud_Container_CPU_Message::executeMIs (unsigned int numberMIs){

    int test;

    test = (getRemainingMIs() - numberMIs);

    if (test > 0)
        setRemainingMIs (test);
    else
        setRemainingMIs (0);
}

void icancloud_Container_CPU_Message::executeTime (simtime_t executedTime){

    simtime_t remainingTime;

    remainingTime = getCpuTime();

    if ((remainingTime - executedTime) > 0.0)
        setCpuTime (remainingTime - executedTime);
    else
        setCpuTime (0);
}



string icancloud_Container_CPU_Message::contentsToString (bool printContents){

    std::ostringstream osStream;

        if (printContents){

            osStream << icancloud_Message::contentsToString(printContents);
            osStream  << " - cpuTime:" <<  getCpuTime() << endl;
            osStream  << " - totalMIs:" <<  getTotalMIs() << endl;
            osStream  << " - remainingMIs:" <<  getRemainingMIs() << endl;
            osStream  << " - CPU priority:" <<  getCpuPriority() << endl;
            osStream  << " - quantum:" <<  getQuantum() << endl;
        }

    return osStream.str();
}


void icancloud_Container_CPU_Message::parsimPack(cCommBuffer *b){

    icancloud_Container_CPU_Message_Base::parsimPack(b);
}


void icancloud_Container_CPU_Message::parsimUnpack(cCommBuffer *b){

    icancloud_Container_CPU_Message_Base::parsimUnpack(b);
}
