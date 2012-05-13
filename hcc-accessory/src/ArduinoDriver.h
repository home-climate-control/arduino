#ifndef ARDUINO_DRIVER_H_
#define ARDUINO_DRIVER_H_

#include "Driver.h"

#define SERVO_FIRST 8
#define SERVO_COUNT 6

#define RELAY_FIRST 2
#define RELAY_COUNT 6

class ArduinoDriver : public Driver {
public:
	ArduinoDriver();
	virtual ~ArduinoDriver();
	void process(Command *command);
	void reset();
	char *getName();
	void setServo(uint8_t channel, uint16_t microseconds);
	void setRelay(uint8_t channel, uint8_t state);
};

#endif /* ARDUINO_DRIVER_H_ */
