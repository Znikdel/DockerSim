// Module that represents a Generic Disk
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.Hardware.Storage.Devices;

moduleinterface IStorageDevice

{
    parameters:
    	@display("i=device/disk");
    gates:
        input in;					// Input gate.		
        output out;					// Output gate.
        
    //    input cin;					// Input gate.		
    //    output cout;					// Output gate.

}

