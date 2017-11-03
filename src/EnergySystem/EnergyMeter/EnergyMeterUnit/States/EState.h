//
// This class is associated to the EState ned module.
//
// A state is defined by an id and the energy consumed when device is performing operations at that state.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//

#ifndef ESTATE_H_
#define ESTATE_H_

#include <omnetpp.h>
#include <sstream>
#include <vector>
#include <string>
#include <map>
using std::string;
using std::pair;
using std::vector;

class EState : public cSimpleModule{

private:

	string state;         // The id for the state
	double cValue;        // The value of consumption of the component for the state.

public:
    /*
    *  Module initialization.
    */
    virtual void initialize();

   /**
    * Module ending.
    */
    virtual void finish(){};

    void handleMessage(cMessage *msg){};

	string getState();
	double getConsumptionValue();

};

#endif /* ESTATE_H_ */
