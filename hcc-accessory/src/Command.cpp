#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include "Command.h"

typedef unsigned long timestamp;

const timestamp READ_DELAY = 1000;

// Payload will be of two types: servo position (3 nibbles) and relay state (1 bit).
// Since relay commands will be rare, it is acceptable to waste 15 bits in order to simplify the logic
const int PAYLOAD_LENGTH = 2;

Command::Command(TargetType targetType, AddressType addressType, byte *addressBuffer, byte *payloadBuffer) :
		targetType(targetType),
		addressType(addressType) {

	if (addressType < ADDRESS_MAX && addressBuffer != NULL) {

		for (int offset = 0; offset < addressLength[addressType]; offset++) {

			this->addressBuffer[offset] = addressBuffer[offset];
		}
	}

	if (targetType < TARGET_MAX && payloadBuffer != NULL) {

		for (int offset = 0; offset < PAYLOAD_LENGTH; offset++) {

			this->payloadBuffer[offset] = payloadBuffer[offset];
		}
	}
}

void Command::dump() {

	Serial.print(F("targetType="));
	Serial.print((int) targetType);
	Serial.print(F(", addressType="));
	Serial.print((int) addressType);
}

// XBee 64 bit address is the longest
static byte _addressBuffer[sizeof(XBeeAddress64) + 1];
static byte _payloadBuffer[sizeof(uint16_t)];

Command* Command::read(Stream *inputStream) {

	timestamp start = millis();

	while (inputStream->available() < 1) {

		if (millis() - start > READ_DELAY) {

			Serial.println(F("read() timed out"));
			return NULL;
		}

		delay(10);
	}

	int frameLength = inputStream->read();

//	Serial.print(F("Frame length: "));
//	Serial.println(frameLength);

	while (inputStream->available() < frameLength) {

		if (millis() - start > READ_DELAY) {

			Serial.print(F("Sync lost @"));
			Serial.println(inputStream->available());

			return NULL;
		}

		delay(10);
	}

	TargetType targetType = (TargetType) inputStream->read();
	AddressType addressType = (AddressType) inputStream->read();

//	Serial.print(F("Target type: "));
//	Serial.println(targetType);
//
//	Serial.print(F("Payload length: "));
//	Serial.println(payloadLength[targetType]);
//
//	Serial.print(F("Address type: "));
//	Serial.println(addressType);
//
//	Serial.print(F("Address length: "));
//	Serial.println(addressLength[addressType]);
//
//	Serial.print(F("Address:"));

	for (int offset = 0; offset < addressLength[addressType]; offset++) {

		_addressBuffer[offset] = inputStream->read();
//		Serial.print(F(" "));
//		Serial.print(_addressBuffer[offset], HEX);
	}

//	Serial.println(F(""));
//	Serial.print(F("Payload:"));

	for (int offset = 0; offset < PAYLOAD_LENGTH; offset++) {

		_payloadBuffer[offset] = inputStream->read();
//		Serial.print(F(" "));
//		Serial.print(_payloadBuffer[offset], HEX);
	}

//	Serial.println(F(""));

	return new Command(targetType, addressType, _addressBuffer, _payloadBuffer);
}
