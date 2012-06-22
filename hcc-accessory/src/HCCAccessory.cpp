#include <Wire.h>

#include <Max3421e.h>
#include <Usb.h>

// Includes below are not necessary for strict C++ compilation,
// but Arduino IDE breaks if they're not provided.
// Hint: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1295488739
// start ---
#include <XBee.h>
#include <Servo.h>
// end ---

#include <AndroidAccessory.h>
#include "Command.h"
#include "DataSample.h"
#include "ArduinoDriver.h"

#define ACCESSORY_MANUFACTURER "DIY Zoning Project"
#define ACCESSORY_MODEL "Equilibrium"
#define ACCESSORY_DESCRIPTION "Home Climate Control"
#define ACCESSORY_VERSION "0.1"
#define ACCESSORY_URL "http://www.homeclimatecontrol.com/adk"

// VT: FIXME: Read this from EEPROM
#define ACCESSORY_SERIAL "e8b3 a6a4"

AndroidAccessory acc(
		ACCESSORY_MANUFACTURER,
		ACCESSORY_MODEL,
		ACCESSORY_DESCRIPTION,
		ACCESSORY_VERSION,
		ACCESSORY_URL,
		ACCESSORY_SERIAL);

void setup();
void setAnalogReference();
void loop();
void read();
void write();

// VT: NOTE: Can't instantiate anything substantial here, setup() hasn't been called yet
// and Arduino goes nuts if you do anything more than look at it cross-eyed

Driver* drivers[4] {
	(Driver *) NULL,
	(Driver *) NULL,
	(Driver *) NULL };

void setup() {

	Serial.begin(115200);

	Serial.print(F("\n"));
	Serial.print(ACCESSORY_MODEL);
	Serial.print(F(" v"));
	Serial.print(ACCESSORY_VERSION);
	Serial.print(" serial # ");
	Serial.println(ACCESSORY_SERIAL);
	Serial.println(ACCESSORY_URL);

	setAnalogReference();

	// This will never be freed
	drivers[0] = new ArduinoDriver();

	acc.powerOn();
}

void setAnalogReference() {

	// Setting analog reference to 1.1V will restrict the top measurable temperature to just
	// 43.3°C for LM34 and 60°C for TMP36. This will provide the best temperature resolution
	// and quality of control, though.

	// Setting it to 2.56V will restrict top measurable temperature to 124.4°C for LM34
	// and 206°C for TMP36 - more than enough for casual use.

	analogReference(INTERNAL1V1);

	// http://arduino.cc/it/Reference/AnalogReference says:
	//
	//     After changing the analog reference, the first few readings from analogRead() may not be accurate.
	//
	// To counter this, let's spend some time settling (it appears that first few reads are inaccurate
	// even if the reference voltage hasn't been changed)

	long start = millis();

	do {

		for (int address = A0; address < A0 + 6; address++) {

			analogRead(address);
		}

		// Apparently, 100ms is enough
	} while (millis() - start < 100);
}

void loop() {

	if (acc.isConnected()) {

		read();
		write();

	} else {

//		Serial.println(F("Not connected"));

		for (int offset = 0; drivers[offset] != NULL; offset++) {

			Driver *driver = drivers[offset];

			if (driver == NULL) {
				break;
			}

//			Serial.print(F("Driver@"));
//			Serial.print(offset);
//			Serial.print(F(": "));
//			Serial.println(driver->getName());

			// VT: FIXME: Might want to make sure this happens once upon disconnect,
			// to allow reset() to do something heavy
			driver->reset();
		}
	}

	delay(10);
}

void parse(Command *command);

void read() {

	if (!acc.available()) {
		return;
	}

	Command *command = Command::read(&acc);

	if (command != NULL) {

//		Serial.print(F("Command received: "));
//		command->dump();
//		Serial.println(F(""));

		parse(command);

		delete command;
	}
}

void heartbeat(unsigned long now);
void readNativeSensors(unsigned long now);

void write() {

	unsigned long now = millis();

	heartbeat(now);
	readNativeSensors(now);
}

void parse(Command *command) {

	Driver* driver = drivers[command->addressType];

	if (driver == NULL) {

		Serial.print(F("ERROR: No driver for address type "));
		Serial.println(command->addressType, HEX);

		Serial.print(F("Driver address: "));
		Serial.println((uint32_t)driver, HEX);

		for (int offset = 0; offset < 4; offset++) {

			Serial.print(F("Driver@"));
			Serial.print(offset);
			Serial.print(F("="));
			Serial.println((uint32_t)drivers[offset], HEX);
		}

		return;
	}

	driver->process(command);
}


unsigned long heartbeatLastMillis = 0;
unsigned long HEARTBEAT_INTERVAL_MILLIS = 5000;

void printDD(long value){

	if (value < 10)
		Serial.print('0');
	Serial.print(value);
}

void printDDD(long value){

	if (value < 100)
		Serial.print('0');

	printDD(value);
}

void heartbeat(unsigned long now) {

	if (now - HEARTBEAT_INTERVAL_MILLIS > heartbeatLastMillis) {

		heartbeatLastMillis = now;
		Serial.print(F("Alive at +"));

		unsigned long millis = now % 1000;
		unsigned long seconds = now / 1000;
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		unsigned days = hours / 24;

	 	seconds %= 60;
	 	minutes %= 60;
	 	hours %= 24;

		Serial.print(days);
		Serial.print(" ");
		printDD(hours);
		Serial.print(":");
		printDD(minutes);
		Serial.print(":");
		printDD(seconds);
		Serial.print(".");
		printDDD(millis);
		Serial.println("");

		Serial.flush();
	}
}

unsigned long sensorsLastReadMillis = 0;
unsigned long POLLING_INTERVAL_MILLIS = 500;

void readNativeSensors(unsigned long now) {

	if (now - POLLING_INTERVAL_MILLIS < sensorsLastReadMillis) {

		// Can't afford to read sensors too often, this will flood the serial stream
		return;
	}

	sensorsLastReadMillis = now;

	for (byte address = 0; address < 6; address++) {

		DataSample dataSample(NATIVE, &address, analogRead(address + A0));

		dataSample.write(&acc);
	}
}
