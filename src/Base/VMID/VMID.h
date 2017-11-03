/*
 * @class VMID VMID.h "VMID.h"
 *
 * This class associate to a virtual machine the user unique identifier given by omnetpp core when a new module has been created,
 * and the vm id given by same way.
 *
 * @author Gabriel Gonzalez Castane
 * @date 2012-08-18
 */

#ifndef VMID_H_
#define VMID_H_

#include <string>
#include "VM.h"

using namespace std;

class VM;

class VMID {

private:

	int user;
	VM* vm;
	int vmID;

public:
	VMID();

	void initialize(int newUser, int vmid);

	void setVM (VM* vmPtr){vm = vmPtr;};

	virtual ~VMID(){};

	int getUser (){return user;};

	int getVMID (){return vmID;};

	VM* getVM (){return vm;};


};

#endif /* VMID_H_ */
