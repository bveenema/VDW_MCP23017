#include "VDW_MCP23017.h"

void VDW_MCP23017::writePin(uint8_t pin, bool dir){
    if(pinShouldUpdate(pin, dir)){
        _writeRequested = true;
    }
}


void VDW_MCP23017::update(){
    // Check for Interrupts

    // Read all Inputs

    // Make any requested writes
    if(_writeRequested) return;

    uint8_t attemptsMade = 0;
    if(attemptsMade > _maxRetries){
        _requestReset = true;
    }
}

// readRegister and writeRegister are only functions directly tied Particle Ecosystem, except for millis() calls
int16_t VDW_MCP23017::readRegister(uint8_t reg, uint8_t bytes){
	uint8_t numBytesReceived = 0;
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.endTransmission();
	numBytesReceived = Wire.requestFrom(_addr, bytes);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tRecieved %d Bytes", numBytesReceived);
	#endif
	// Return bytes if they were received, otherwise, return -1
	if(numBytesReceived > 0) return Wire.read();
	else {
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Read Failed: %d, %d",_addr, reg);
		#endif
		return -1;
	}
}

bool VDW_MCP23017::writeRegister(uint8_t reg, uint8_t data){
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(data);
    uint8_t writeResponse = Wire.endTransmission();

    if(writeResponse == 0) return true;

	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Failed:\n\tresponse: %d\n\tregister: %d\n\tdata: %d", reg, data);
	#endif

	return false;
}

bool VDW_MCP23017::clearBit(uint8_t reg, uint8_t bit){
	int16_t value = readRegister(reg);

	// check if register was received
	if( value < 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Register Read Failed: CAM: %d, REG: %d",_addr,reg);
		#endif
		return false;
	}

	value &= ~(1 << bit);

	if(writeRegister(reg, value)) return true;
	else return false;
}

bool VDW_MCP23017::setBit(uint8_t reg, uint8_t bit){
	uint8_t value = readRegister(reg);

	// check if register was received
	if( value < 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Register Read Failed: CAM: %d, REG: %d",_addr,reg);
		#endif
		return false;
	}

	value |= (1 << bit);
	
	if(writeRegister(reg, value)) return true;
	else return false;
}