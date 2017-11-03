/*
 * @class cGateManager cGateManager.h "cGateManager.h"
 *
 * This class manages the dynamic creation gates, increasement and decreasement of gates vectors, and
 * link and unlink gates.
 *
 * @author Gabriel Gonzalez Castane
 * @date 2014-22-10
 */

#ifndef CGATEMANAGER_H_
#define CGATEMANAGER_H_

#include "cgate.h"

#include <omnetpp.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#include <math.h>
#include "stdio.h"
#include "stdlib.h"

using std::vector;
using std::string;

class cGateManager {

    struct gateStructure_t{
        cGate* gate;
        bool gateHit;
    };

    typedef gateStructure_t gateStructure;

    vector <int> holes;

    vector <gateStructure*> gates;

    cModule* mod;

public:

     cGateManager(cModule* mod);

      /*
       * Destructor
       */
      virtual ~cGateManager();

      /*
       * This method check if a gate is free.
       */
    int isGateEmpty(int index);

    /*
     * This method link a real gate with its virtual gate at vector's position
     */
    void linkGate(string gateName, int index);

    /*
     * This method create a new gate or give a free gate. It returns the position where
     * it will be created
     */
    int newGate(string gateName);

    /*
     * This method returns the gate by a given position as parameter
     */
    cGate* getGate(int index);

    /*
     * This method free the gate at position given as parameter disconnecting its possible
     * conexions
     */
    cGate* freeGate(int index);

    /*
     *  This method connects a gate (IN) with the gate given as parameter
     */
    void connectIn(cGate* gate, int index);

    /*
     *  This method connects a gate (OUT) with the gate given as parameter
     */
    void connectOut(cGate* gate, int index);

    /*
     * This method returns the gate position by a given gate identification
     */
    int searchGate(int gateId);


};

#endif /* GATEDATA_H_ */
