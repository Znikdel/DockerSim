// Module that represents a Generic Main Memory
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.Hardware.Memories.MainMemories;

moduleinterface IMainMemory
{
    
    parameters:
        
        double size_MB;					// Memory size (in MB)
    	double blockSize_KB;
        @display("i=icons/DDR");
        
        
    gates:
        input fromInput;				// Input gates from imputs
        input fromOutput;				// Input gate from output

        output toInput;					// Output gates to inputs
        output toOutput;				// Output gate to output

}

