#ifndef VDW_MCP23017_H
#define VDW_MCP23017_H

#include "Particle.h"
#define VDW_MCP23017_DEBUG_ENABLED 1

class VDW_MCP23017{
    public:
        VDW_MCP23017(uint8_t addr = 0x20, uint8_t interruptPin = 0)
            : _addr(addr)
            , _interruptPin(interruptPin) {}

        void init(){
            // Setup I2C
            if (!Wire.isEnabled()) {
                Wire.begin();
            }
        }

        // Update the direction (Hi/LOW) of the pin.
        // checks the stored (local) direction of the pin and only requests a write if different.
        // waits until the next update() call to change the value of the pin
        void writePin(uint8_t pin, bool dir);

        // Update the direction (HI/LOW) of the pin
        // does the same this as writePin, but does not wait for update() function.
        // should be used sparringly to avoid excessive I2C calls
        void writePinNow(uint8_t pin, bool dir);

        // Get the state (HI/LOW) of the pin
        // Returns the state that was last read in the last update() call.
        bool readPin(uint8_t pin);

        // Get the state (HI/LOW) of the pin
        // Does the same as readpin() but makes an immediate read call to the device
        bool readPinNow(uint8_t pin);

        // Makes I2C calls and checks for errors.
        // The majority of I2C calls are performed within update() so that excessive I2C call are avoided
        // If an I2C call fails to complete successfully. Update will try again up to _maxRetries before
        // requesting a reset of the device.
        void update();

        // Signals the update() loop that an interrupt was recieved
        // If an interrupt pin is supplied to the object in the constructor, this class will automagically
        // check for interrupts in the update() function. This method is useful if an interrupt pin cannot be
        // supplied to the object in order to inform the it of an interrupt.
        // Interrupt will not be handled until the next update() call. 
        void interrupt(bool interrupt = true);

        // Returns true if there is an error with the MCP23017 that requires a reset.
        // Reset of the I2C device is handled by the controlling loop so that multiple I2C devices
        // can share the same reset pin.
        bool requestReset(){
            return _requestReset;
        }

        void retries(uint8_t retries){
            _maxRetries = retries;
        }
    private:
        int16_t readRegister(uint8_t reg, uint8_t bytes=1);
        bool writeRegister(uint8_t reg, uint8_t data);
        bool clearBit(uint8_t reg, uint8_t bit);
        bool setBit(uint8_t reg, uint8_t bit);

        // returns true if the pin is an output pin and the supplied dir is different from the current dir
        bool pinShouldUpdate(uint8_t pin, bool dir);

        // Device Settings
        uint8_t _addr;
        uint8_t _interruptPin;
        uint8_t _maxRetries;
        bool _requestReset;

        // Flags
        bool _writeRequested;

};

#endif