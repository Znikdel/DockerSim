// Module that implements an AppDefinition
//
// This module contains a vector of application definitions
//
// - A smart User Behavior.
//
// @author Zahra Nikdel
// @date 2017-03-20

package icancloud.src.Users.UserGenerator.ContainerDefinition;

import icancloud.src.Virtualization.VirtualMachines.Containers.Container;

module ContainerDefinition
{
    parameters:
        int containerQuantity;
    submodules:
        container[containerQuantity]: Container {
            parameters:
                @display("i=msg/job;p=98,129,row");
        }
}
