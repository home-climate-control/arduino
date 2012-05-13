#ifndef HCC_ACCESSORY_H_
#define HCC_ACCESSORY_H_

#include <stdint.h>

typedef uint64_t Address1Wire;

enum AddressType {
	NATIVE = 0x00, // "ARDUINO" is already used
	ONEWIRE = 0x01,
	XBEE = 0x02,
	ADDRESS_MAX = 0x03
};

// Address lengths corresponding to AddressType
const uint8_t addressLength[] = {
		1, // Just the channel number
		sizeof(Address1Wire) + 1, // 1-Wire address + channel number
		sizeof(XBeeAddress64) + 1 // XBee address + channel number
};

#endif /* HCC_ACCESSORY_H_ */
