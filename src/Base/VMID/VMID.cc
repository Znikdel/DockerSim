/*
 * VMID.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: gabriel
 */

#include "VMID.h"

VMID::VMID(){
    user = -1;
    vmID = -1;
    vm = NULL;
}

void VMID::initialize(int newUser, int vmid) {
	user = newUser;
	vmID = vmid;
	vm = NULL;
}
