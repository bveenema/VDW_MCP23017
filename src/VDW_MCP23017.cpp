#include "VDW_MCP23017.h"

bool VDW_MCP23017::setMode(uint8_t pin, bool mode, bool pullup){
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Set Mode:\n\tPin: %d\n\tMode: %d\n\tPull-up: %d",pin, mode, pullup);
	#endif

	bool returnVal = false;

	// determine the PORT
	PORT port = getPort(pin);
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if requested mode different from current
    if(pinShouldChange(pin, _reg[port].IODIR, mode)){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Mode Change");
		#endif
		_reg[port].IODIR = bitWrite(_reg[port].IODIR, pin%8, mode);
		_reg[port].writeReg[IODIR] = 1;
        _writeRequested = true;
		returnVal = true;
    }
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Mode Change");
	#endif

	// Determine if requested pullup different from current
    if(pinShouldChange(pin, _reg[port].GPPU, pullup)){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Pullup Change");
		#endif
		_reg[port].GPPU = bitWrite(_reg[port].GPPU, pin%8, pullup);
		_reg[port].writeReg[GPPU] = 1;
        _writeRequested = true;
		returnVal = true;
    }
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Pullup Change");
	#endif
	return returnVal;
}

bool VDW_MCP23017::writePin(uint8_t pin, bool dir){
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Pin:\n\tPin: %d\n\tDir: %d",pin, dir);
	#endif

	// determine the PORT
	PORT port = getPort(pin);
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if the pin is an OUTPUT (Can't write an INPUT)
	if(bitRead(_reg[port].IODIR, pin%8) && bitRead(_reg[port].IODIR, pin%8)){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tPin is INPUT: %d, %d",bitRead(_reg[port].IODIR, pin%8), bitRead(_reg[port].IODIR, pin%8));
		#endif
		return false;
	}

	// Determine if requested setting different from current
    if(pinShouldChange(pin, _reg[port].OLAT, dir)){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Change");
		#endif
		_reg[port].OLAT = bitWrite(_reg[port].OLAT, pin%8, dir);
		_reg[port].writeReg[OLAT] = 1;
        _writeRequested = true;
		return true;
    }
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO CHANGE", port);
	#endif
	return false;
}

bool VDW_MCP23017::writePinNow(uint8_t pin, bool dir){
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Pin NOW:\n\tPin: %d\n\tDir: %d",pin, dir);
	#endif

	// determine the PORT
	PORT port = getPort(pin);
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if the pin is an output (Can't write an input)
	if(bitRead(_reg[port].IODIR, pin%8) == 1){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tPin is INPUT");
		#endif
		return false;
	}

	// Determine if requested setting different from current
    if(pinShouldChange(pin, _reg[port].OLAT, dir)){
		#ifdef VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tWriting Pin");
		#endif
		_reg[port].OLAT = bitWrite(_reg[port].OLAT, pin%8, dir);
		if(writeRegister(MCP23017_OLAT[port], _reg[port].OLAT, _reg[port].OLAT)) return true;
    }
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO CHANGE", port);
	#endif
	return false;
}

bool VDW_MCP23017::readPin(uint8_t pin){
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Read Pin :\n\tPin: %d",pin);
	#endif

	// Determine the port
	PORT port = getPort(pin);
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	return bitRead(_reg[port].GPIO, pin%8);
}

bool VDW_MCP23017::readPinNow(uint8_t pin){
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Read Pin NOW:\n\tPin: %d\n\tDir: %d",pin);
	#endif

	// Determine the port
	PORT port = getPort(pin);
	#ifdef VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Get the value of the pin
	return bitRead(readRegister(MCP23017_GPIO[port], 1), pin%8);
}


void VDW_MCP23017::update(){
	// don't run update() unless the device has been initialized
	if(!_initialized) return;

	static uint8_t attemptsMade = 0;
    // Check for Interrupts

    // Read all Inputs
	if(_readRequested){
		int16_t gpioA = readRegister(MCP23017_GPIO[PORTA], 1);
		int16_t gpioB = readRegister(MCP23017_GPIO[PORTB], 1);

		if(gpioA < 0 || gpioB <0){
			#if VDW_MCP23017_DEBUG_ENABLED
				Serial.printlnf("GPIO Read Failed: %d, %d",gpioA, gpioB);
			#endif
		} else {
			_reg[PORTA].GPIO = gpioA;
			_reg[PORTB].GPIO = gpioB;
			_readRequested = false;
		}
		_readRequested = false;
	}

    // Make any requested writes
    if(_writeRequested){
		bool success = true;
		for(uint8_t i=0; i<2; i++){
			if(_reg[i].writeReg[IODIR]) success &= 
		}

		for(uint8_t i=0; i<2; i++){
			if(_reg[i].IODIR != _reg[i].IODIR) 		success &= writeRegister(MCP23017_IODIR[i], _reg[i].IODIR, _reg[i].IODIR);
			if(_reg[i].IPOL != _reg[i].IPOL) 			success &= writeRegister(MCP23017_IPOL[i], _reg[i].IPOL, _reg[i].IPOL);
			if(_reg[i].GPINTEN != _reg[i].GPINTEN) 	success &= writeRegister(MCP23017_GPINTEN[i], _reg[i].GPINTEN, _reg[i].GPINTEN);
			if(_reg[i].DEFVAL != _reg[i].DEFVAL) 		success &= writeRegister(MCP23017_DEFVAL[i], _reg[i].DEFVAL, _reg[i].DEFVAL);
			if(_reg[i].INTCON != _reg[i].INTCON) 		success &= writeRegister(MCP23017_INTCON[i], _reg[i].INTCON, _reg[i].INTCON);
			if(_reg[i].GPPU != _reg[i].GPPU) 			success &= writeRegister(MCP23017_GPPU[i], _reg[i].GPPU, _reg[i].GPPU);
			if(_reg[i].OLAT != _reg[i].OLAT) 			success &= writeRegister(MCP23017_OLAT[i], _reg[i].OLAT, _reg[i].OLAT);
		}
		if(success) _writeRequested = false;
		#if VDW_MCP23017_DEBUG_ENABLED
			else Serial.printlnf("Register Write Failed");
		#endif
	}

	// All comms completed sucessfully, reset and do it again
	if(!_writeRequested && ! _readRequested){
		attemptsMade = 0;
		if(_reg[0].IODIR != 0x00 && _reg[1].IODIR != 0x00) _readRequested = true;
	} else {
		attemptsMade++;
	}

	// Check for max retries
    if(attemptsMade > _maxRetries){
        _requestReset = true;
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Too Many Retries");
		#endif
    }
}

// readRegister and writeRegister are only functions directly tied Particle Ecosystem, except for millis() calls
int16_t VDW_MCP23017::readRegister(uint8_t reg, uint8_t bytes){
	uint8_t numBytesReceived = 0;
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.endTransmission();
	numBytesReceived = Wire.requestFrom(_addr, bytes);
	// Return bytes if they were received, otherwise, return -1
	if(numBytesReceived > 0) return Wire.read();
	else {
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Read Failed: %d, %d",_addr, reg);
		#endif
		return -1;
	}
}

bool VDW_MCP23017::writeRegister(uint8_t reg, uint8_t data, uint8_t &localReg){
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(data);
    uint8_t writeResponse = Wire.endTransmission();

    if(writeResponse == 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Write Register:\n\tresponse: %d\n\tregister: %d\n\tdata: %d", writeResponse, reg, data);
		#endif
		localReg = data;
		return true;
	}

	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Failed:\n\tresponse: %d\n\tregister: %d\n\tdata: %d", writeResponse, reg, data);
	#endif

	return false;
}

VDW_MCP23017::PORT VDW_MCP23017::getPort(uint8_t pin){
	return (pin<8) ? PORTA : PORTB;
}

bool VDW_MCP23017::pinShouldChange(uint8_t pin, uint8_t reg, bool newState){
	if(newState == bitRead(reg, pin)) return false;
	return true;
}