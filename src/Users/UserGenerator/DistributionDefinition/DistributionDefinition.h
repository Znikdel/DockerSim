//
// Module that implements a distribution definition
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30

#ifndef DISTRIBUTIONDEFINITION_H_
#define DISTRIBUTIONDEFINITION_H_

#include <omnetpp.h>

class DistributionDefinition: public cSimpleModule{
protected:
    virtual ~DistributionDefinition();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish(){};
};

#endif /* DISTRIBUTIONDEFINITION_H_ */
