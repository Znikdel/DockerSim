// Module that implements a Generic Cache
//
// Null memory.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.Hardware.Memories.Caches.BlockCache;

import icancloud.src.Architecture.NodeComponents.Hardware.Memories.Caches.ICache;

simple BlockCache like ICache

{
    parameters:
        double numInputs;	// Number of inputs
        double hitRatio;	// Cache hit ratio [0..1]

    gates:
        input fromInput[];	// Input gates from imputs
        input fromOutput;		// Input gate from output

        output toInput[];		// Output gates to inputs
        output toOutput;		// Output gate to output

}

