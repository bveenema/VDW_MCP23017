#ifndef VDW_MCP23017_H
#define VDW_MCP23017_H

#include "Particle.h"

#define VDW_MCP23017_DEBUG_ENABLED 1

enum {
    GPA0,
    GPA1,
    GPA2,
    GPA3,
    GPA4,
    GPA5,
    GPA6,
    GPA7,
    GPB0,
    GPB1,
    GPB2,
    GPB3,
    GPB4,
    GPB5,
    GPB6,
    GPB7,
} MCP23017_Pins;

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

            // Initialize Pins
            for(uint8_t i=0; i<2; i++){
                writeRegister(MCP23017_IODIR[i], _currentReg[i].IODIR, _currentReg[i].IODIR);
                writeRegister(MCP23017_GPPU[i], _currentReg[i].GPPU, _currentReg[i].GPPU);
                writeRegister(MCP23017_GPIO[i], _currentReg[i].GPIO, _currentReg[i].GPIO);
            }
            _initialized = true;
        }

        // Set the mode (INPUT, OUTPUT) and enable/disable the pullup of the pin
        // Checks the stored (local) mode of the pin and only requests a change if different
        // waits until the next update() call to change the mode of the pin
        bool setMode(uint8_t pin, bool mode, bool pullup = 0);

        // Set the mode (INPUT, OUTPUT, INPUT_PULLDOWN) of the pin
        // does the same as setMode(), but does not wait for update() function.
        // Should be used sparingly to avoid excessive I2C calls
        bool setModeNow(uint8_t pin, bool mode, bool pullup = 0);

        // Update the direction (Hi/LOW) of the pin.
        // checks the stored (local) direction of the pin and only requests a write if different.
        // waits until the next update() call to change the value of the pin
        bool writePin(uint8_t pin, bool dir);

        // Update the direction (HI/LOW) of the pin
        // does the same this as writePin, but does not wait for update() function.
        // should be used sparingly to avoid excessive I2C calls
        bool writePinNow(uint8_t pin, bool dir);

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

    #if !VDW_MCP23017_DEBUG_ENABLED  
        private:
    #endif

        typedef enum PORT{
            PORTA,
            PORTB,
        };

        int16_t readRegister(uint8_t reg, uint8_t bytes=1);
        bool writeRegister(uint8_t reg, uint8_t data, uint8_t &localReg);
        PORT getPort(uint8_t pin);

        uint8_t bitRead(uint8_t value, uint8_t bit) { return ((value) >> (bit)) & 0x01; }
        uint8_t bitSet(uint8_t value, uint8_t bit) { return (value) |= (1UL << (bit)); }
        uint8_t bitClear(uint8_t value, uint8_t bit) { return (value) &= ~(1UL << (bit)); }
        uint8_t bitWrite(uint8_t value, uint8_t bit, bool bitvalue) { return bitvalue ? bitSet(value, bit) : bitClear(value, bit); }

        // returns true if the new setting does not match the current setting of the pin
        bool pinShouldChange(uint8_t pin, uint8_t reg, bool newState);

        // Device Settings
        uint8_t _addr = 0x20;
        uint8_t _interruptPin = 0;
        uint8_t _maxRetries = 5;

        // Flags
        bool _initialized = false;
        bool _writeRequested = false;
        bool _readRequested = false;
        bool _requestReset = false;

        // Register Settings
        struct {
            uint8_t IODIR = 0xFF;
            uint8_t IPOL = 0x00;
            uint8_t GPINTEN = 0x00;
            uint8_t DEFVAL = 0x00;
            uint8_t INTCON = 0x00;
            uint8_t GPPU = 0x00;
            uint8_t INTF = 0x00;
            uint8_t INTCAP = 0x00;
            uint8_t GPIO = 0x00;
            uint8_t OLAT = 0x00;
        } _currentReg[2], _newReg[2];

        // MCP23017 Control Registers
        const uint8_t MCP23017_IODIR[2] = {0x00, 0x01}; // I/O Direction Register (~1: INPUT, 0: OUTPUT)
        const uint8_t MCP23017_IPOL[2] = {0x02, 0x03}; // Input Polarity Register (1: GPIO reg opposite pin, ~0: GPIO reg same as pin)
        const uint8_t MCP23017_GPINTEN[2] = {0x04, 0x05}; // Interrupt-on-change pins (1: enabled, ~0: disabled)
        const uint8_t MCP23017_DEFVAL[2] = {0x06, 0x07}; // Default Value Register (Compare value for int-on-change, interrupt if opposite)
        const uint8_t MCP23017_INTCON[2] = {0x08, 0x09}; // Int-On-Change Control Reg (1: Compared against DEFVAL, ~0: Compared against previous)
        const uint8_t MCP23017_IOCON = 0x0A; // Configuration Reg (BANK, MIRROR, SEQOP, DISSLW, HAEN, ODR, INTPOL)
        const uint8_t MCP23017_GPPU[2] = {0x0C, 0x0D}; // Pull-Up Register (1: Enabled, ~0: Disabled)
        const uint8_t MCP23017_INTF[2] = {0x0E, 0x0F}; // Interrupt Flag Reg (1: Pin Caused Int ~0: interrupt not pending)
        const uint8_t MCP23017_INTCAP[2] = {0x10, 0x11}; // Interupt Capture Reg (Value of GPIO at time of interrupt)
        const uint8_t MCP23017_GPIO[2] = {0x12, 0x13}; // Gen Purp I/O Reg (1: Logic High ~0: Logic Low)
        const uint8_t MCP23017_OLAT[2] = {0x14, 0x15}; // Output Latch Reg (1: Logic High ~0: Logic Low)
};

#endif