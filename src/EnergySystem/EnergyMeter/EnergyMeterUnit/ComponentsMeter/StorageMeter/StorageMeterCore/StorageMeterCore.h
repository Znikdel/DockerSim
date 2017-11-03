//
// Module that implements a storage meter core
//
// Storage meter core is responsible for performing energy measurements.
// The core supply operations to get storage energy consumed (Joules) and storage power consumption(Watts)
// 
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//

#ifndef STORAGE_METER_CORE_H_
#define STORAGE_METER_CORE_H_

#include "AbstractMeterUnit.h"

class StorageMeterCore : public AbstractMeterUnit {

public:

    // Initialize
        virtual void initialize();

    // Destructor
        virtual ~StorageMeterCore();

    // This method returns the value of the instant power consumed by disks
        double getInstantConsumption(string state = NULL_STATE, int partIndex = -1);

    // This method returns the quantity of jules that the component has consumed
        double getEnergyConsumed(int partIndex = -1);
};

#endif /* STORAGE_METER_CORE_H_ */




