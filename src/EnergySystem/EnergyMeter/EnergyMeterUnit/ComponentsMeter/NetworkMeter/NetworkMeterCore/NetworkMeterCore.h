//
// Module that implements a network interface card (NIC) meter core
//
// NIC meter core is responsible for performing energy measurements.
// The core supply operations to get NIC energy consumed (Joules) and NIC power consumption(Watts)
// 
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//

#ifndef NETWORK_METER_CORE_H_
#define NETWORK_METER_CORE_H_

#include "AbstractMeterUnit.h"

class NetworkMeterCore : public AbstractMeterUnit {

public:

    virtual void initialize();

    virtual ~NetworkMeterCore();

    // This method returns the value of the instant power consumed by CPU
    double getInstantConsumption(string state = NULL_STATE, int partIndex = -1);

    // This method returns the quantity of jules that
    double getEnergyConsumed(int partIndex = -1);

};

#endif /* NETWORK_CONSUMPTION_CALCULATOR_H_ */




