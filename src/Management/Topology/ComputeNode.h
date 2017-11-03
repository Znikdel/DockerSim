/**
 *
 * @class StorageNode StorageNode.cc StorageNode.h
 *
 *  Class to link to computeNode.ned
 *
 * @authors: Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2014-06-08
 */
#ifndef COMPUTENODE_H_
#define COMPUTENODE_H_

#include <omnetpp.h>

class ComputeNode: public cSimpleModule{
protected:
    virtual ~ComputeNode();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish(){};
};

#endif /* FSUSERDEFINITION_H_ */
