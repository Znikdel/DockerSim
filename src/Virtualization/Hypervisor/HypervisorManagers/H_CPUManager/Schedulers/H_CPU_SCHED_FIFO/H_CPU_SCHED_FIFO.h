//
// Class that defines the behavior of hypervisor cpu controller.
//
// The scheduling policy of this module is first input first output.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//


#ifndef HCPUSCHEDFIFO_H_
#define HCPUSCHEDFIFO_H_

#include "H_CPUManager_Base.h"

class H_CPU_SCHED_FIFO : public H_CPUManager_Base{

protected:

    /** Array to show the ID of VM executing in the core 'i'*/
    vector<int> vmIDs;

    /** Queue of messages trying to access to a core */
    vector<icancloud_Message*> coreQueue;

public:

    virtual ~H_CPU_SCHED_FIFO();

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
    void schedulingCPU(icancloud_Message *msg);

    /**
    * Process a response message.
    * @param sm Request message.
    */
    void processHardwareResponse(icancloud_Message *msg);
};

#endif /* HCPUSCHEDFIFO_H_ */
