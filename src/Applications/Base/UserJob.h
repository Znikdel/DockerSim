
/**
 * @class UserJob UserJob.h UserJob.cc
 *
 * This class models a executing job.
 *
 * When an application is launched by a tenant, this class identifies where is the application allocated
 * and who is the owner of it.
 *
 * @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2014-12-12
 */

#ifndef UserJob_H_
#define UserJob_H_

#include "Machine.h"
#include "AbstractUser.h"
#include "jobBase.h"

class Machine;
class AbstractUser;

class UserJob : virtual public jobBase {

protected:

        /** Where the app is executed*/
        Machine* mPtr;

        /** Owner of the app */
        AbstractUser* userPtr;

protected:

    	/*
    	 * Destructor
    	 */
    	~UserJob();

        /**
         * Module initialization.
         */
         virtual void initialize();

        /**
         * Module ending.
         */
         virtual void finish();

        /**
         * Process a self message.
         * @param msg Self message.
         */
         virtual void processSelfMessage (cMessage *msg) = 0;

        /**
         * Process a request message.
         * @param sm Request message.
         */
         virtual void processRequestMessage (icancloud_Message *sm) = 0;

        /**
         * Process a response message from a module in the local node.
         * @param sm Request message.
         */
         virtual void processResponseMessage (icancloud_Message *sm) = 0;

public:

       /**
        * Start the app execution.
        */
        virtual void startExecution ();

        /*
         * This method returns a copy of the job
         */
        UserJob* cloneJob(cModule* userMod);

        /*
         * Setter and getter for application owner
         */
        void setUpUser (AbstractUser *user);
        AbstractUser* getAppOwner () {return userPtr;};

        /*
         * Setter and getter for the machine where application is executed
         */
        void setMachine (Machine* m);
        Machine* getMachine (){return mPtr;};
};

#endif /* UserJob_H_ */
