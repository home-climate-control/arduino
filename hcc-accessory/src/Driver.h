#ifndef DRIVER_H_
#define DRIVER_H_

#include "Command.h"

class Driver {
public:
	Driver();
	virtual ~Driver();

	virtual void process(Command *command) = 0;
	virtual void reset() = 0;
	virtual char *getName() = 0;
};

#endif /* DRIVER_H_ */
