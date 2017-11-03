// Module that represents a Generic CPU core.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.Hardware.CPUs.CPUcores;

moduleinterface ICPUcore

{
    parameters:
        double speed;			// CPU core speed (in MIPS)
        double tick_s;			// Tick time (in seconds)

        @display("i=device/cpu");
    gates:
        input in;			// Input gate
        output out;		// Output gate
        
    //    input cin;			// Input gate
    //    output cout;		// Output gate

}
