#include "VDW_MCP23017.h"

bool VDW_MCP23017::setMode(uint8_t pin, bool mode, bool pullup){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Set Mode:\n\tPin: %d\n\tMode: %d\n\tPull-up: %d",pin, mode, pullup);
	#endif

	bool returnVal = false;

	// Invert mode (INPUT is 1 on MCP23017 and 0 in Particle/Wiring Ecosystem)
	mode = !mode;

	// determine the PORT
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if requested mode different from current
    if(pinShouldChange(pin, _reg[port].IODIR, mode)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Mode Change");
		#endif
		_reg[port].IODIR = bitWrite(_reg[port].IODIR, pin%8, mode);
		bitSet(_reg[port].writeReg, IODIR);
        _writeRequested = true;
		returnVal = true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Mode Change Mode:%d, Reg: %d", mode, bitRead(_reg[port].IODIR, pin%8));
	#endif

	// Determine if requested pullup different from current
    if(pinShouldChange(pin, _reg[port].GPPU, pullup)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Pullup Change");
		#endif
		_reg[port].GPPU = bitWrite(_reg[port].GPPU, pin%8, pullup);
		bitSet(_reg[port].writeReg, GPPU);
        _writeRequested = true;
		returnVal = true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Pullup Change");
	#endif
	return returnVal;
}

bool VDW_MCP23017::setModeNow(uint8_t pin, bool mode, bool pullup){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Set Mode NOW:\n\tPin: %d\n\tMode: %d\n\tPull-up: %d",pin, mode, pullup);
	#endif

	bool returnVal = false;

	// Invert mode (INPUT is 1 on MCP23017 and 0 in Particle/Wiring Ecosystem)
	mode = !mode;

	// determine the PORT
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if requested mode different from current
    if(pinShouldChange(pin, _reg[port].IODIR, mode)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Mode Change");
		#endif
		_reg[port].IODIR = bitWrite(_reg[port].IODIR, pin%8, mode);
		if(writeRegister(MCP23017_IODIR[port], _reg[port].IODIR)){
			returnVal = true;
		} else {
			bitSet(_reg[port].writeReg, IODIR);
			_writeRequested = true;
		}
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Mode Change Mode:%d, Reg: %d", mode, bitRead(_reg[port].IODIR, pin%8));
	#endif


	// Determine if requested pullup different from current
    if(pinShouldChange(pin, _reg[port].GPPU, pullup)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Pullup Change");
		#endif
		_reg[port].GPPU = bitWrite(_reg[port].GPPU, pin%8, pullup);
		if(writeRegister(MCP23017_GPPU[port], _reg[port].IODIR)){
			returnVal = true;
		} else{
			bitSet(_reg[port].writeReg, GPPU);
        	_writeRequested = true;
		}
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO Pullup Change");
	#endif
	return returnVal;
}

bool VDW_MCP23017::writePin(uint8_t pin, bool dir){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Pin:\n\tPin: %d\n\tDir: %d",pin, dir);
	#endif

	// determine the PORT
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if the pin is an OUTPUT (Can't write an INPUT)
	if(bitRead(_reg[port].IODIR, pin%8) && bitRead(_reg[port].IODIR, pin%8)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tPin is INPUT: %d, %d",bitRead(_reg[port].IODIR, pin%8));
		#endif
		return false;
	}

	// Determine if requested setting different from current
    if(pinShouldChange(pin, _reg[port].OLAT, dir)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tRequesting Change");
		#endif
		_reg[port].OLAT = bitWrite(_reg[port].OLAT, pin%8, dir);
		bitSet(_reg[port].writeReg, OLAT);
        _writeRequested = true;
		return true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO CHANGE", port);
	#endif
	return false;
}

bool VDW_MCP23017::writePinNow(uint8_t pin, bool dir){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Write Pin NOW:\n\tPin: %d\n\tDir: %d",pin, dir);
	#endif

	// determine the PORT
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if the pin is an output (Can't write an input)
	if(bitRead(_reg[port].IODIR, pin%8) == 1){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tPin is INPUT");
		#endif
		return false;
	}

	// Determine if requested setting different from current
    if(pinShouldChange(pin, _reg[port].OLAT, dir)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tWriting Pin");
		#endif
		_reg[port].OLAT = bitWrite(_reg[port].OLAT, pin%8, dir);
		if(writeRegister(MCP23017_OLAT[port], _reg[port].OLAT)){
			return true;
		} else { // if write fails, add it to the queue for the next update() call
			bitSet(_reg[port].writeReg, OLAT);
        	_writeRequested = true;
			return false;
		}
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO CHANGE", port);
	#endif
	return false;
}

bool VDW_MCP23017::readPin(uint8_t pin){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Read Pin :\n\tPin: %d",pin);
	#endif

	// Determine the port
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	#if VDW_MCP23017_DEBUG_ENABLED
		bool value = bitRead(_reg[port].GPIO, pin%8);
		Serial.printlnf("\tValue: %d",value);
		return value;
	#else
		return bitRead(_reg[port].GPIO, pin%8);
	#endif
}

bool VDW_MCP23017::readPinNow(uint8_t pin){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Read Pin NOW:\n\tPin: %d",pin);
	#endif

	// Determine the port
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Get the value of the pin
	#if VDW_MCP23017_DEBUG_ENABLED
		_reg[port].GPIO = readRegister(MCP23017_GPIO[port], 1);
		bool value = bitRead(_reg[port].GPIO, pin%8);
		Serial.printlnf("\tValue: %d",value);
		return value;
	#else
		_reg[port].GPIO = readRegister(MCP23017_GPIO[port], 1);
		return bitRead(_reg[port].GPIO, pin%8);
	#endif
}

bool VDW_MCP23017::setInterrupt(uint8_t pin, uint8_t mode){
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("Set Interrupt :\n\tPin: %d",pin);
	#endif

	bool returnVal = false;

	// Determine the port
	PORT port = getPort(pin);
	#if VDW_MCP23017_DEBUG_ENABLED
		Serial.printlnf("\tPORT: %d", port);
	#endif

	// Determine if the pin is an INPUT (Can't have interrupt on OUTPUT)
	if(bitRead(_reg[port].IODIR, pin%8) == 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tPin is OUTPUT");
		#endif
		return false;
	}

	// Determine if requested INTCON setting different from current
    if(pinShouldChange(pin, _reg[port].INTCON, (mode != CHANGE))){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tUpdate INTON");
		#endif
		_reg[port].INTCON = bitWrite(_reg[port].INTCON, pin%8, (mode != CHANGE));
		bitSet(_reg[port].writeReg, INTCON);
		_writeRequested = true;
		returnVal = true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO INTCON CHANGE", port);
	#endif

	// Determine if requested DEFVAL setting different from current
	if(pinShouldChange(pin, _reg[port].DEFVAL, (mode == FALLING))){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tUpdate DEFVAL");
		#endif
		_reg[port].DEFVAL = bitWrite(_reg[port].DEFVAL, pin%8, (mode == FALLING));
		bitSet(_reg[port].writeReg, DEFVAL);
		_writeRequested = true;
		returnVal = true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO DEFVAL CHANGE", port);
	#endif

	// Determine if requested GPINTEN setting different from current
	if(pinShouldChange(pin, _reg[port].GPINTEN, 1)){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("\tUpdate GPINTEN");
		#endif
		_reg[port].GPINTEN = bitWrite(_reg[port].GPINTEN, pin%8, 1);
		bitSet(_reg[port].writeReg, GPINTEN);
		_writeRequested = true;
		returnVal = true;
    }
	#if VDW_MCP23017_DEBUG_ENABLED
		else Serial.printlnf("\tNO GPINTEN CHANGE", port);
	#endif

	return returnVal;
}


void VDW_MCP23017::update(){
	#if VDW_MCP23017_LOOP_TIMER_ENABLED
		uint32_t loopTimer = micros();
	#endif

	// don't run update() unless the device has been initialized
	if(!_initialized) return;

	static uint8_t attemptsMade = 0;
    // Check for Interrupts
	if(_interrupt || detectInterrupt()){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("INTERRUPT DETECTED:\n\tInt Pin: %d\n\tInt Pin Value: %d\n\t_interrupt: %d", _interruptPin, digitalRead(_interruptPin),_interrupt);
		#endif
		int16_t gpioA = readRegister(MCP23017_GPIO[PORTA], 1);
		int16_t gpioB = readRegister(MCP23017_GPIO[PORTB], 1);

		if(gpioA < 0 || gpioB < 0){
			#if VDW_MCP23017_DEBUG_ENABLED
				Serial.printlnf("GPIO Read Failed: %d, %d",gpioA, gpioB);
			#endif
		} else {
			_reg[PORTA].GPIO = gpioA;
			_reg[PORTB].GPIO = gpioB;
		}
	}

    // Read all Inputs
	if(_interruptMode) _readRequested = false;
	if(_readRequested){
		int16_t gpioA = readRegister(MCP23017_GPIO[PORTA], 1);
		int16_t gpioB = readRegister(MCP23017_GPIO[PORTB], 1);

		if(gpioA < 0 || gpioB < 0){
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
		for(uint8_t i=0; i<2; i++){
			if(bitRead(_reg[i].writeReg, IODIR	)) 	bitWrite(_reg[i].writeReg, 	IODIR, 		!writeRegister(MCP23017_IODIR[i], 	_reg[i].IODIR));
			if(bitRead(_reg[i].writeReg, IPOL	)) 	bitWrite(_reg[i].writeReg, 	IPOL, 		!writeRegister(MCP23017_IPOL[i], 	_reg[i].IPOL));
			if(bitRead(_reg[i].writeReg, GPINTEN)) 	bitWrite(_reg[i].writeReg, 	GPINTEN, 	!writeRegister(MCP23017_GPINTEN[i], _reg[i].GPINTEN));
			if(bitRead(_reg[i].writeReg, DEFVAL	)) 	bitWrite(_reg[i].writeReg, 	DEFVAL, 	!writeRegister(MCP23017_DEFVAL[i], 	_reg[i].DEFVAL));
			if(bitRead(_reg[i].writeReg, INTCON	)) 	bitWrite(_reg[i].writeReg, 	INTCON, 	!writeRegister(MCP23017_INTCON[i], 	_reg[i].INTCON));
			if(bitRead(_reg[i].writeReg, GPPU	)) 	bitWrite(_reg[i].writeReg, 	GPPU, 		!writeRegister(MCP23017_GPPU[i],	_reg[i].GPPU));
			if(bitRead(_reg[i].writeReg, OLAT	)) 	bitWrite(_reg[i].writeReg, 	OLAT, 		!writeRegister(MCP23017_OLAT[i], 	_reg[i].OLAT));
		}
		if(_reg[PORTA].writeReg == 0 && _reg[PORTB].writeReg == 0) _writeRequested = false;
		#if VDW_MCP23017_DEBUG_ENABLED
			else Serial.printlnf("Register Write Failed: %d, %d",_reg[PORTA].writeReg, _reg[PORTB].writeReg);
		#endif
	}

	// All comms completed sucessfully, reset and do it again
	if(!_writeRequested && ! _readRequested){
		attemptsMade = 0;
		if(!_interruptMode && _reg[0].IODIR != 0x00 && _reg[1].IODIR != 0x00) _readRequested = true;
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
	#if VDW_MCP23017_LOOP_TIMER_ENABLED
		Serial.printlnf("\t\t\t\tUpdate Cycle Time: %d", micros() - loopTimer);
	#endif
}

bool VDW_MCP23017::configureInterrupts(bool mirror, bool odr, bool intpol){
	bitWrite(_regIOCON, MIRROR, mirror);
	bitWrite(_regIOCON, ODR, odr);
	bitWrite(_regIOCON, INTPOL, intpol);

	return writeRegister(MCP23017_IOCON, _regIOCON);
}

// readRegister and writeRegister are only functions directly tied Particle Ecosystem, except for millis() calls
int16_t VDW_MCP23017::readRegister(uint8_t reg, uint8_t bytes){
	uint8_t numBytesReceived = 0;
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.endTransmission();
	numBytesReceived = Wire.requestFrom(_addr, bytes);
	// Return bytes if they were received, otherwise, return -1
	if(numBytesReceived > 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Read Register:\n\tregister: %d",reg);
		#endif
		return Wire.read();
	}
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

    if(writeResponse == 0){
		#if VDW_MCP23017_DEBUG_ENABLED
			Serial.printlnf("Write Register:\n\tresponse: %d\n\tregister: %d\n\tdata: %d", writeResponse, reg, data);
		#endif
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
	if(newState == bitRead(reg, pin%8)) return false;
	return true;
}