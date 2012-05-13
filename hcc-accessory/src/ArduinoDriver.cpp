#include <Servo.h>
#include "ArduinoDriver.h"

static Servo servos[SERVO_COUNT];
void debug(char *type, int channel, int payload);

ArduinoDriver::ArduinoDriver() {
	reset();
}

ArduinoDriver::~ArduinoDriver() {
	reset();
}

void ArduinoDriver::process(Command *command) {

	uint8_t channel = command->addressBuffer[0];
	uint16_t payload = ((uint16_t *) &command->payloadBuffer)[0];

	switch (command->targetType) {

	case SERVO:

		setServo(channel, payload + 1000);
		break;

	case RELAY:

		setRelay(channel, payload);
		break;

	default:

		Serial.print(F("ERROR: Don't know how to handle target "));
		Serial.println(command->targetType);

		break;
	}
}

void ArduinoDriver::setServo(uint8_t channel, uint16_t microseconds) {

	debug("S", channel, microseconds);

	if (channel < 0 || channel >= SERVO_COUNT) {

		Serial.print(F("ERROR: Servo channel out of range: "));
		Serial.println(channel, DEC);

		return;
	}

	servos[channel].writeMicroseconds(microseconds);
}

void ArduinoDriver::setRelay(uint8_t channel, uint8_t state) {

	debug("R", channel, state);

	if (channel < 0 || channel >= RELAY_COUNT) {

		Serial.print(F("ERROR: Relay channel out of range: "));
		Serial.println(channel, DEC);

		return;
	}

	digitalWrite(channel + RELAY_FIRST, state > 0 ? 1 : 0);
}

void ArduinoDriver::reset() {

	for (int offset = 0; offset < SERVO_COUNT; offset++) {

		servos[offset].attach(offset + SERVO_FIRST);
		servos[offset].writeMicroseconds(1500);
	}

	for (int offset = 0; offset <= RELAY_COUNT; offset++) {

		pinMode(offset + RELAY_FIRST, OUTPUT);
		digitalWrite(offset + RELAY_FIRST, 0);
	}
}

char *ArduinoDriver::getName() {

	return "(Arduino)";
}

void debug(char *type, int channel, int payload) {

	Serial.print(type);
	Serial.print((int) channel);
	Serial.print(F("="));
	Serial.println(payload);
}

