//
// Module that implements a memory meter core
//
// Memory meter core is responsible for performing energy measurements.
// The core supply operations to get memory energy consumed (Joules) and memory power consumption(Watts)
// 
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//

#ifndef MEMORYMETERCORE_H_
#define MEMORYMETERCORE_H_

#include "AbstractMeterUnit.h"
#include "RAMmemory.h"

class MemoryMeterCore : public AbstractMeterUnit {

    int numModules;                              // Number of memory modules plugged in the main board
    int numDRAMChips;                            // Number of DRAM Chips per module

public:

    // Initialize
        virtual void initialize();

    // Destructor
	    virtual ~MemoryMeterCore();

    // This method returns the value of the instant power consumed by CPU
    double getInstantConsumption(string state = NULL_STATE, int partIndex = -1);

    // This method returns the quantity of jules that
    double getEnergyConsumed(int partIndex = -1);

};

#endif /* MEMORYMETERCORE_H_ */




