//
// Class that defines the behavior of hypervisor network controller.
//
// The scheduling policy of this module is first input first output.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//

#ifndef HNETSCHEDFIFO_H_
#define HNETSCHEDFIFO_H_

#include "H_NETManager_Base.h"

class H_NET_SCHED_FIFO : public H_NETManager_Base{

protected:

public:

    virtual ~H_NET_SCHED_FIFO();

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
    void schedulingNET(icancloud_Message *sm);

    /**
    * Process a response message.
    * @param sm Request message.
    */
    void processResponseMessage(icancloud_Message *msg);
};

#endif /* HCPUSCHEDFIFO_H_ */
