// Module that implements a General user behavior.
//
// This module contains the following subModules:
//
// - A general user behaviour
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30
package icancloud.src.Users.Profiles.GeneralUser;

simple GeneralUser
{
    parameters:
        bool printVMs = default (true);
        bool printJobs = default (true);
        bool printContainerJobs = default (true);
        
        @class(GeneralUser);
        @display("i=abstract/person");
        
}
