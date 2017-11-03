
// Module that implements a Node with its virtualization layer (hypervisor).
//
// This class models a node with an hypervisor. It is able to execute applications at node and virtual machines
//  requested by tenants.
//
// @author Gabriel González Castañé
// @date 2014-12-12

#ifndef _NODEVM_H_
#define _NODEVM_H_

#include "VMID.h"
#include "VmMsgController.h"
#include "Hypervisor.h"
#include "Node.h"
#include "AbstractCloudManager.h"

class Hypervisor;

class NodeVL : public Node {

protected:

    Hypervisor* hypervisor;
    vector <VMID*> instancedVMs;						// VMs instanced in the node

public:

    ~NodeVL();

        void initialize();
        void finish();

		void freeResources (int pId, int uId);

		//------------------ To operate with instanced VMs -------------

		bool testLinkVM (int vmCPUs, int vmMemory, int vmStorage, int vmNetIF, string vmTypeID, int uId, int pId);

		/*
		 * Link VMs has to be invoked when an user request petition is attended.
		 */
		void linkVM  (cGate** iGateCPU,cGate** oGateCPU, cGate* iGateMemI,cGate* oGateMemI,cGate* iGateMemO,
		              cGate* oGateMemO, cGate* iGateNet,cGate* oGateNet,cGate* iGateStorage,cGate* oGateStorage,
                      int numCores, string virtualIP, int vmMemory, int vmStorage, int uId, int pId);

		/*
		 * Unlink VM, as to be invoked when the VM is completely deleted.
		 */
		void unlinkVM (int vmNumCores, int vmMemory, int vmStorage, string virtualIP, int uId, int pId);

		/*
		 * Returns the VMs structure size.
		 */
		int getNumOfLinkedVMs ();

		/*
		 * This method returns the vm identifier allocated at 'index' position
		 */
		int getVMID (int index);

		/*
		 * This method set the vm pointer into the node to allow the access from the node to the virtual machines allocated in it.
		 */
		void setVMInstance(VM* vmPtr);

		/*
		 * This method returns the vm pointer with an identifier that matches with the given parameter vmID
		 */
		VM* getVMInstance (int pId, int uId);

		/*
		 * This method returns the num of vm allocated in the node.
		 */
		int getNumVMAllocated(){return instancedVMs.size();};


	    int getNumProcessesRunning(){return (os->getNumProcessesRunning() + getNumVMAllocated());};

		// ------------------- Initial notifications to Cloud Manager -------------------

        void notifyManager (icancloud_Message* msg);

        void setManager(icancloud_Base* manager);

        void initNode ();

        cModule* getHypervisor();

        //------------------ To operate with the state of the node -------------
            virtual void turnOn ();                                                     // Change the node state to on.
            virtual void turnOff ();                                                    // Change the node state to off.

        // ------------------ Methods to migrate virtual machines ----------

        // This method is to create a conection between two nodes to migrate virtual machines
        void createListenToMigrate(){}
        void connectToMigrate (string destinationIP, string localIP, VM* vm,  bool liveMigration, int dirtyingRate){}


};

#endif /* _NODE_H_ */
