//
// Module that implements a cpu meter core
//
// CPU meter core is responsible for performing energy measurements.
// The core supply operations to get cpu energy consumed (Joules) and cpu power consumption(Watts)
// 
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//


#ifndef CPUMETERCORE_H_
#define CPUMETERCORE_H_

#include "CPUController.h"
#include "AbstractMeterUnit.h"

class CPUMeterCore : public AbstractMeterUnit {

public:

    virtual void initialize();

	virtual ~CPUMeterCore();

    // This method returns the value of the instant power consumed by CPU
    double getInstantConsumption(string state = NULL_STATE, int partIndex = -1);

    // This method returns the quantity of jules that
    double getEnergyConsumed(int partIndex = -1);

};

#endif /* CPUMETERCORE_H_ */




