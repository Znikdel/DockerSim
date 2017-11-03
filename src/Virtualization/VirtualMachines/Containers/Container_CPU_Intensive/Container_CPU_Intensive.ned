// Module that implements a CPU intensive app.
//
// This application generate requests to the cpu system of a set of MIs a set of iterations
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30
//
// @author updated to iCanCloudDocker by Zahra Nikdel
// @date 2017-03-17

package icancloud.src.Virtualization.VirtualMachines.Containers.Container_CPU_Intensive;

import icancloud.src.Virtualization.VirtualMachines.Containers.IContainer;

simple Container_CPU_Intensive like IContainer

{
    parameters:
        string application_netType = default ("INET");		// Network type (INET or BASIC)        
        double startDelay;									// Starting delay time!        
        int inputSize;										// Input size of data
        int outputSize;										// Output size of data
        int MIs;											// Number of MIs to execute
        int iterations = default (0);						// Number of iterations
        @display("i=msg/job");
    gates:
     //   input fromDockerEngine @loose;			// Input gate from OS (Operating System)
     //   output toDockerEngine @loose;				// Output gate to OS (Operating System)
        input fromOS @loose;			// Input gate from OS (Operating System)
        output toOS @loose;				// Output gate to OS (Operating System)
}


