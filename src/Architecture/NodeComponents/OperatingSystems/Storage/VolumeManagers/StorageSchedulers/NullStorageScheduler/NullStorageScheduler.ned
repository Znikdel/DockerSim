// Module that implements a Generic Storage Scheduler
//
// Null Scheduler
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package icancloud.src.Architecture.NodeComponents.OperatingSystems.Storage.VolumeManagers.StorageSchedulers.NullStorageScheduler;

import icancloud.src.Architecture.NodeComponents.OperatingSystems.Storage.VolumeManagers.StorageSchedulers.IStorageScheduler;

simple NullStorageScheduler like IStorageScheduler

{
    gates:
        input fromInput;		// Input gate from input
        input fromOutput;		// Input gate from output		

        output toInput;		// Output gate to input
        output toOutput;		// Output gate to output

}

