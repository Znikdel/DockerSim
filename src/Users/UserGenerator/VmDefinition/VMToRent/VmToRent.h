//
// Module that implements a virtual machine definition that tenants can rent.
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30

#ifndef VMTORENT_H_
#define VMTORENT_H_

#include <omnetpp.h>

class VmToRent: public cSimpleModule{
protected:
    virtual ~VmToRent();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish(){};
};

#endif /* VMTORENT_H_ */
