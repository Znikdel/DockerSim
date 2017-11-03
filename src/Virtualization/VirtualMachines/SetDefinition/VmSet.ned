//
// This class defines a Virtual Machine Set. 
//
// A virtual machine set is a vector of VmImages.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//


package icancloud.src.Virtualization.VirtualMachines.SetDefinition;

import icancloud.src.Virtualization.VirtualMachines.SetDefinition.VmImage;

module VmSet
{
    parameters:

		int vmImageQuantity;
    submodules:

        vmImage[vmImageQuantity]: VmImage {
            parameters:
                @display("i=msg/job;p=98,129,row");
        }
}
