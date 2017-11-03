/**
 *
 * @class StorageNode StorageNode.cc StorageNode.h
 *
 *  Class to link to storageNode.ned
 *
 * @authors: Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2014-12-11
 */

#ifndef StorageNode_H_
#define StorageNode_H_

#include <omnetpp.h>

class StorageNode: public cSimpleModule{
protected:
    virtual ~StorageNode();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish(){};
};

#endif /* FSUSERDEFINITION_H_ */
