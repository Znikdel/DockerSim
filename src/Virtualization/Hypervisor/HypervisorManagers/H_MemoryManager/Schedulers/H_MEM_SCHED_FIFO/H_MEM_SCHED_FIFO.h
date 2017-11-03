//
// Class that defines the behavior of hypervisor memory controller.
//
// The scheduling policy of this module is first input first output.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//
#ifndef HMEMSCHEDFIFO_H_
#define HMEMSCHEDFIFO_H_

#include "H_MemoryManager_Base.h"

class H_MEM_SCHED_FIFO : public H_MemoryManager_Base{

protected:

    struct memoryManage_t{
        int uId;
        int pId;
        int vmGate;
        int vmTotalBlocks_KB;
        int remainingBlocks_KB;
    };
    typedef memoryManage_t memoryCell;

    vector<memoryCell*> memoryCells;

    int totalMemory_Blocks;
    int memoryAvailable_Blocks;

public:

    virtual ~H_MEM_SCHED_FIFO();

    /*
     * Module initialization
     */
    void initialize();

    /*
     * Module finalization
     */
    void finish();

    /*
     * Scheduler fifo
     */
    void schedulingMemory(icancloud_Message *msg);

    /*
    * This method sets the input and output gates from vm to the network manager
    */
   int setVM (cGate* oGateI, cGate* oGateO, cGate* iGateI, cGate* iGateO, int uId, int pId, int requestedMemory_KB);

   /*
    * This method delete the gates connected to the VM with id given as parameter
    */
   void freeVM(int uId, int pId);

   /*
    * This method returns the total memory size in MB
    */
   int getTotalMemory (){return (unsigned int) ceil ((totalMemory_Blocks * blockSize_KB));};

   /*
    * This method returns the available memory size in MB
    */
   int getRemainingMemory(){return (unsigned int) ceil ((memoryAvailable_Blocks * blockSize_KB));};

   /*
    * This method returns the position of the gate at gateManager of a VM by a user id and vm id;
    */
   int getVMGateIdx(int uId, int pId);

   /*
    * This method returns the occupation of the memory for a user and a vm
    */
   double getVMMemoryOccupation_MB(int uId, int pId);

private:
   /*
    * Translate the size requested to memory blocks
    */
   unsigned int requestedSizeToBlocks(unsigned int size){return (unsigned int) ceil (size / blockSize_KB);}

   void printCells(string methodName);



};

#endif /* HMEMSCHEDFIFO_H_ */
