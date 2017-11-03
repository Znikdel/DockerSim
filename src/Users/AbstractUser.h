/**
 * @class AbstractUser AbstractUser.cc AbstractUser.h
 *
 * This class has to be imported in order to create an user profile by implementing the abstract methods.
 *
 * @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2014-06-10
 */

#ifndef _ABSTRACT_USER_H_
#define _ABSTRACT_USER_H_

#include "userStorage.h"
#include "AbstractNode.h"
#include "UserJob.h"
#include "Container_UserJob.h"
//class UserJob;
//class Container_UserJob;

class AbstractUser : public userStorage {

protected:

    /** The user marks of time.  **/
		simtime_t startTime;
        simtime_t endTime;

    /** Attributes for log files*/
        bool print;                     // If it has to print the results or not
        string file;                    // The file name where the data are going to be printed


		/*******************************************************************************
		 *            Virtual methods to create different users
		 *******************************************************************************/

		// The initialize of the user behavior. Start VMs, all, any .. ?
		virtual void userInitialization () = 0;

		/*
		 * This method is invoked if it were necessary to realize any operation before the
		 * user leave the cloud system
		 */
		virtual void userFinalization () = 0;

		/*
		 * This method select a the resources to execute the given job as parameter.
		 */
		virtual AbstractRequest* selectResourcesJob (jobBase* job) = 0;
        virtual AbstractRequest* selectResourcesContainerJob (Container_jobBase* job) = 0;

		/*
		 * This method returns a job of the user list of jobs waiting to be executed at waiting queue.
		 */
		virtual jobBase* selectJob () = 0;
        virtual Container_jobBase* selectContainerJob () = 0;
   //     virtual UserJob* selectJob () = 0;
  //      virtual Container_UserJob* selectContainerJob () = 0;
		/*
		 *  This method is invoked from the application when a job has finished.
		 *  It is useful to extract values from the job, or the timings of the executions.
		 *
         *  The method notify_UserJobHasFinished is responsible for deleting the job, move it from running queue
         *   to finish queue
         *  and call to schedule.
         */

		virtual void jobHasFinished (jobBase* job) = 0;
        virtual void containerjobHasFinished (Container_jobBase* job) = 0;

		/*
		 * When the CloudManager attends a request and creates the VMs, it notifies this fact to
		 * the user with request attended. The parameters are a vector with pointers created to the VMs modules and
		 * the virtual machine type which they belong.
		 */
		virtual void requestAttended (AbstractRequest* req) = 0;

		/*
		 * When a request operation is unknown or it provokes an exception, this method
		 * will be called by CloudManager.
		 */
		virtual void requestErrorDeleted (AbstractRequest* req) = 0;

		/*
		 * The order of events that defines the behavior of each type of user.
		 */
        virtual void schedule() = 0;

        /*
         * This method allocates the job given as parameter into the machine associated to it
         */
        virtual int allocateJob(jobBase* job) = 0;
        virtual int allocateContainerJob(Container_jobBase* job) = 0;

        /*
         * This method send the request to the manager.
         */
        virtual void send_request_to_manager(AbstractRequest* req) = 0;

        /*******************************************************************************
         *                              END Virtual methods
         *******************************************************************************/


            /**
             * Module initialization.
             */
             void initialize ();


            /**
            * Module ending.
            */
            void finish();

            /*
             * This method finalizes the user structures.
             * It Supress all the Jobs from the Waiting Queue to the Finish Queue.
             *  Move those jobs to the finish queue
             *
             * If everything is correct, return true.
             * Otherwise return false without perform any action against jobs.
             *
             */
            virtual bool finalizeUser();

            /*
             * This method prints the results of the job
             */
            string printJobResults (JobResultsSet* resultSet);
            string printContainerJobResults (Container_JobResultsSet* resultSet);

            /**
             * Get the out Gate to the module that sent <b>msg</b>.
             * @param msg Arrived message.
             * @return Gate (out) to module that sent <b>msg</b> or NULL if gate not found.
             */
             virtual cGate* getOutGate (cMessage *msg){ return NULL; };

            /**
             * Process a self message.
             * @param msg Self message.
             */
             virtual void processSelfMessage (cMessage *msg){};

            /**
             * Process a request message.
             * @param sm Request message.
             */
             virtual void processRequestMessage (icancloud_Message *sm){};

            /**
             * Process a response message.
             * @param sm Request message.
             */
             virtual void processResponseMessage (icancloud_Message *sm){};

public:

        /*
         * Destructor.
         */
        virtual ~AbstractUser ();

        /**
         * Module initialization.
         */
        void initParameters (string userBehavior, string userId, string fileLog = "");


        /*
         * This method is invoked by UserGenerator in order to initialize the user behavior
         */
        void startUser(){userInitialization();};

        /*
         * This method is invoked by UserGenerator in order to add a job to user's jobList
         */
        void addParsedJob (jobBase *job);
//Zahra Nikdel:

        void addParsedContainerJob (Container_jobBase *job);

         /*
          * This method starts dinamically an application.
          */
         void start_up_job_execution (AbstractNode* destinationExecute, jobBase* job, JobQueue* qSrc, JobQueue* qDst, int qDst_pos = -1);
         void start_up_container_job_execution (AbstractNode* destinationExecute, Container_jobBase* job, Container_JobQueue* qSrc, Container_JobQueue* qDst, int qDst_pos = -1);

        // ---------------------------------------------------------------------------
        // ---------------- Operations to notify user (System Manager)   -------------
        // ---------------------------------------------------------------------------

        /*
         * This method is invoked by system manager / scheduler when a job has finished.
         *
         * It removes from the running queue and insert into the finish queue and then
         * call to virtual jobHasFinished().
         * The method finalizes calling to the schedule method.
         */
        virtual void notify_UserJobHasFinished (jobBase* job);
        virtual void notify_UserContainerJobHasFinished (Container_jobBase* job);

        /*
         * This method is invoked by cloud manager / scheduler when a request has been attendeed.
         */
        virtual void notify_UserRequestAttendeed (AbstractRequest* req);

        /*
         * This methods are invoked by cloud manager / scheduler when a request has provoked an error.
         */
        virtual void notify_UserRequestError (AbstractRequest* req){requestAttended (req);};


        bool hasPendingRequests(){return ((hasPendingStorageRequests()) && (userBase::hasPendingRequests()));};

};

#endif
