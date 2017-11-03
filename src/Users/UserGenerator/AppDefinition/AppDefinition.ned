// Module that implements an AppDefinition
//
// This module contains a vector of application definitions
//
// - A smart User Behavior.
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30

package icancloud.src.Users.UserGenerator.AppDefinition;

import icancloud.src.Applications.Apps.AppBase;

module AppDefinition
{
    parameters:
        int appQuantity;
    submodules:
        application[appQuantity]: AppBase {
            parameters:
                @display("i=msg/job;p=98,129,row");
        }
}
