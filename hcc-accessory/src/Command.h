/*
 * Command.h
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#include <Stream.h>
#include <Arduino.h>
#include <XBee.h>

#include "HCCAccessory.h"
/*
 * Command frame layout:
 *
 * @0 Frame length, not including this byte
 * @1 TargetType
 * @2 AddressType
 * @4 Address, including channel
 * (@3 + address length) Payload, two bytes
 */

enum TargetType {
	NONE = 0x00,
	RELAY = 0x01,
	SERVO = 0x02,
	TARGET_MAX = 0x03
};

class Command {

public:

	Command(TargetType targetType, AddressType addressType, byte *addressBuffer, byte *payloadBuffer);
	void dump();
	static Command* read(Stream *inputStream);
	void write(Stream *outputStream);

	const TargetType targetType;
	const AddressType addressType;

// VT: FIXME: public for now, till kinks are ironed out
//private:

	// XBee 64 bit address is the longest
	byte addressBuffer[sizeof(XBeeAddress64) + 1];
	byte payloadBuffer[sizeof(uint32_t)];

};

#endif /* COMMAND_H_ */
