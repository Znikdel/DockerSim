
// Module that implements a Node.
//
// This class models Computing node.
//
// @author Gabriel González Castañé
// @date 2014-12-12

#ifndef _NODE_H_
#define _NODE_H_

#include "AbstractNode.h"
#include "EnergyMeterController.h"
#include "RoutingTable.h"
#include "ElementType.h"
#include "AbstractDCManager.h"

class Node : public AbstractNode{

protected:

		 bool energyMeter;                                  // true = energy meter active. false otherwise.
		 bool storageNode;                                  // true = the node is a storage node.

 		 EnergyMeterController* energyMeterPtr;             // Pointer to the energy meter

		 virtual void initialize();
		 virtual void finish();

public:

		//------------------ To operate with the state of the node -------------
            virtual void turnOn ();                                                     // Change the node state to on.
            virtual void turnOff ();                                                    // Change the node state to off.

        //------------------ To operate check the energy of the node -------------

            double getCPUInstantConsumption(int partIndex = -1){return energyMeterPtr->cpuInstantConsumption(energyMeterPtr->getCurrentCPUState(),partIndex);};
            double getCPUEnergyConsumed(int partIndex = -1){return energyMeterPtr->getCPUEnergyConsumed(partIndex);};
            double getMemoryInstantConsumption(int partIndex = -1){return energyMeterPtr->getMemoryInstantConsumption(energyMeterPtr->getCurrentMemoryState(),partIndex);};
            double getMemoryEnergyConsumed(int partIndex = -1){return energyMeterPtr->getMemoryEnergyConsumed(partIndex);};
            double getNICInstantConsumption(int partIndex = -1){return energyMeterPtr->getNICInstantConsumption(energyMeterPtr->getCurrentNICState(),partIndex);};
            double getNICEnergyConsumed(int partIndex = -1){return energyMeterPtr->getNICEnergyConsumed(partIndex);};
            double getStorageInstantConsumption(int partIndex = -1){return energyMeterPtr->getStorageInstantConsumption(energyMeterPtr->getCurrentStorageState(),partIndex);};
            double getStorageEnergyConsumed(int partIndex = -1){return energyMeterPtr->getStorageEnergyConsumed(partIndex);};
            double getPSUConsumptionLoss(){return energyMeterPtr->getPSUConsumptionLoss();};
            double getPSUEnergyLoss(){return energyMeterPtr->getPSUEnergyLoss();};
            double getInstantConsumption(){return energyMeterPtr->getNodeInstantConsumption();};
            double getEnergyConsumed(){return energyMeterPtr->getNodeEnergyConsumed();};
            double getSubsystemsConsumption(){return energyMeterPtr->getNodeSubsystemsConsumption();};
            void resetEnergyHistory (){energyMeterPtr->resetNodeEnergy();};

         // ------------------------- Memorization ------------------------------
            // Get component names
            string getCPUName(){return energyMeterPtr->getCPUName();};
            string getMemoryName(){return energyMeterPtr->getMemoryName();};
            string getStorageName(){return energyMeterPtr->getStorageName();};
            string getNetworkName(){return energyMeterPtr->getNetworkName();};
            void loadMemo(MemoSupport* cpu,MemoSupport* memory,MemoSupport* storage,MemoSupport* network){energyMeterPtr->loadMemo(cpu,memory,storage,network);};

       virtual void setManager(icancloud_Base* manager);

       virtual void notifyManager(icancloud_Message* msg);

       virtual void initNode ();

};

#endif /* _NODE_H_ */
