// Module that implements a Generic CPU Scheduler.
//
// CPU scheduler that implements a Round-Robin policy
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.OperatingSystems.CPU_Schedulers.CPU_Scheduler_RR;

import icancloud.src.Architecture.NodeComponents.OperatingSystems.CPU_Schedulers.ICPU_Scheduler;


simple CPU_Scheduler_RR like ICPU_Scheduler

{
    parameters:
        double numCPUs;			// Number of CPUs
        int quantum;			// Quantum (in ticks)

        @display("i=block/cogwheel");
    gates:
        input fromOsGate;		// Input gate from Service Redirector
        input fromCPU[];					// Input gates from CPU

        output toOsGate;		// Output gate to Service Redirector
        output toCPU[];					// Output gates to CPU

}
