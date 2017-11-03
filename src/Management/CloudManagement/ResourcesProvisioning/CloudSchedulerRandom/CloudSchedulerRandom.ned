// Module that implements a cloud scheduler
//
// This scheduler is a random policy. It selects a random node between all the data center to get the 
// requested resources.
//
// @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2014-12-11
        
package icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudSchedulerRandom;

import icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudManagerScheduler;

module CloudSchedulerRandom extends CloudManagerScheduler
{
    parameters:		
        
        @class(CloudSchedulerRandom);
        @display("i=msg/job");
        
}
