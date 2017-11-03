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

#ifndef ICANCLOUD_CONTAINER_CPU_MESSAGE_H_
#define ICANCLOUD_CONTAINER_CPU_MESSAGE_H_
#include "icancloud_Container_CPU_Message_m.h"
class icancloud_Container_CPU_Message : public icancloud_Container_CPU_Message_Base{
public:
    //icancloud_Container_CPU_Message();
    virtual ~icancloud_Container_CPU_Message();


/**
 * Constructor of icancloud_Message
 * @param name Message name
 * @param kind Message kind
 */
 icancloud_Container_CPU_Message (const char *name=NULL, int kind=0);


/**
 * Constructor of icancloud_Message
 * @param other Message
 */
 icancloud_Container_CPU_Message(const icancloud_Container_CPU_Message& other);


/**
 * = operator
 * @param other Message
 */
 icancloud_Container_CPU_Message& operator=(const icancloud_Container_CPU_Message& other);


/**
 * Method that makes a copy of a icancloud_Message
 */
 virtual icancloud_Container_CPU_Message *dup() const;


/**
 * Update the message length
 */
 void updateLength ();


/**
 * Update the MIs to be exucuted
 * @param numberMIs Number of instructions (measured in MI) to be executed
 */
 void executeMIs (unsigned int numberMIs);


/**
 * Update the amount of time to be exucuted
 * @param executedTime Amount of time for executing current CB
 */
 void executeTime (simtime_t executedTime);


/**
 * Parse all parameters of current message to string.
 * @param printContents Print message contents.
 * @return String with the corresponding contents.
 */
 virtual string contentsToString (bool printContents);


/**
 * Serializes a icancloud_Message.
 * @param b Communication buffer
 */
 virtual void parsimPack(cCommBuffer *b);


/**
 * Deserializes a icancloud_Message.
 * @param b Communication buffer
 */
 virtual void parsimUnpack(cCommBuffer *b);
};

#endif
