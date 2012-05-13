#ifndef DATA_SAMPLE_H_
#define DATA_SAMPLE_H_

#include "HCCAccessory.h"

class DataSample {
public:
	DataSample(AddressType addressType, const byte *address, uint16_t sample);
	~DataSample();
	void write(Stream *outputStream);

	const AddressType addressType;
	const uint16_t sample;
	byte *address;
};

inline DataSample::DataSample(AddressType addressType, const byte *address, uint16_t sample) :
	addressType(addressType),
	sample(sample) {

	this->address = new byte(addressLength[addressType]);

	memcpy(this->address, address, addressLength[addressType]);
}

inline DataSample::~DataSample() {

	delete address;
}

inline void DataSample::write(Stream *outputStream) {

	uint8_t frameLength = sizeof(uint8_t) + addressLength[addressType] + sizeof(sample);

//	Serial.print(F("Frame: "));
//	Serial.print(frameLength, DEC);
//	Serial.print(F("["));
//	Serial.print(addressType, DEC);
//	Serial.print(F(":"));
//
//	for (int offset = 0; offset < addressLength[addressType]; offset++) {
//		Serial.print(address[offset], HEX);
//	}
//
//	Serial.print(F(":"));
//	Serial.print(sample, DEC);
//	Serial.println(F("]"));

	outputStream->write(frameLength);
	outputStream->write(addressType);
	outputStream->write((const uint8_t *)address, addressLength[addressType]);
	outputStream->write((const uint8_t *) &sample, 2);

	outputStream->flush();
}

#endif /* DATA_SAMPLE_H_ */
