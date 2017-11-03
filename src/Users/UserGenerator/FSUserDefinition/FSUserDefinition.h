//
// Module that implements a file system configuration given by a tenant for executing jobs
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30

#ifndef FSUSERDEFINITION_H_
#define FSUSERDEFINITION_H_

#include <omnetpp.h>

class FSUserDefinition: public cSimpleModule{
protected:
    virtual ~FSUserDefinition();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish(){};
};

#endif /* FSUSERDEFINITION_H_ */
