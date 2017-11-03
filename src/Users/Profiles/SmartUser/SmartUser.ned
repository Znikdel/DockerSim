// Module that implements a smart user behavior.
//
// This module contains the following subModules:
//
// - A smart User Behavior.
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30
package icancloud.src.Users.Profiles.SmartUser;

simple SmartUser
{
    parameters:
        bool printVMs = default (true);
        bool printJobs = default (true);
		int port = default (10000); 
        @class(SmartUser);
        @display("i=abstract/person");
        
}
