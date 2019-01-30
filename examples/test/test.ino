/*
 * Project M6-Firmware
 * Description:
 * Author:
 * Date:
 */

#include "VDW_MCP23017.h"

// ------------------------
// Pin Configuration
// ------------------------
#define SDA_PIN D0
#define SCL_PIN D1

// Connected to MCP23017 reset pin with external pull-up resistor
#define I2C_RESET_PIN D7

// Interrupt Pin
#define IO_INT TX

// Test pins are wired to each other through 1k resistor
#define PHOTON_TEST_PIN A0
#define IOEXP_TEST_PIN GPA7

// ------------------------
// End Pin Configuration
// ------------------------

SYSTEM_MODE(SEMI_AUTOMATIC);

uint32_t testTimer = 0;
VDW_MCP23017 IO_EXP_1(0x20, IO_INT);


void setup() {
  Serial.begin(57000);
  delay(5000);
  
  pinMode(I2C_RESET_PIN, OUTPUT);
  pinMode(PHOTON_TEST_PIN, INPUT);
  pinMode(IO_INT, INPUT);
  digitalWrite(I2C_RESET_PIN, HIGH);

  // Reset I2C
  digitalWrite(I2C_RESET_PIN, LOW);
  delay(10);
  digitalWrite(I2C_RESET_PIN, HIGH);

  // Initialize 
  testTimer = millis();
}

void loop() {
  // Update the IO Expander
  IO_EXP_1.update();

  // ---------------------------------------------
  // MCP23017 Tests
  // ---------------------------------------------
  static uint32_t testNumber = 1;

  switch(testNumber){
    case 1: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: pinShouldChange should return true\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        setupTest = true;
      }
      if(!runTest) {
        if( IO_EXP_1.pinShouldChange(GPA0, IO_EXP_1._reg[0].IODIR, 0) ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail");
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 2: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: pinShouldChange should return false\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        setupTest = true;
      }
      if(!runTest) {
        if( !IO_EXP_1.pinShouldChange(GPA0, IO_EXP_1._reg[0].IODIR, 1) ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.printlnf("Test Fail, %d", IO_EXP_1._reg[0].IODIR);
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 3: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Initialize IO Exp\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        IO_EXP_1.init();
        setupTest = true;
      }
      if(!runTest) {
        if( IO_EXP_1._reg[0].IODIR == 0xFF && IO_EXP_1.readRegister(0x00, 1) == 0xFF && Wire.isEnabled() ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail");
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 4: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();

      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Update Pin Mode to Output\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        pinMode(PHOTON_TEST_PIN, INPUT_PULLDOWN);
        IO_EXP_1.setMode(IOEXP_TEST_PIN, OUTPUT);
        Serial.printlnf("OUTPUT value: %d, INPUT value: %d",OUTPUT, INPUT);
        setupTest = true;
      }
      if(!runTest) {
        if(IO_EXP_1._reg[0].IODIR == 0x7F 
          && IO_EXP_1.readRegister(0x00) == 0x7F){
            Serial.println("Success");
            runTest = true;
            testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail");
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 5: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Write to Pin\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        pinMode(PHOTON_TEST_PIN, INPUT_PULLDOWN);
        IO_EXP_1.setMode(IOEXP_TEST_PIN, OUTPUT);
        IO_EXP_1.writePin(IOEXP_TEST_PIN, 1);
        setupTest = true;
      }
      if(!runTest) {
        Serial.printlnf("Photon Test Pin: %d", digitalRead(PHOTON_TEST_PIN));
        if(digitalRead(PHOTON_TEST_PIN) == 1){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail");
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 6: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Read from Pin\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        pinMode(PHOTON_TEST_PIN, OUTPUT);
        IO_EXP_1.setMode(IOEXP_TEST_PIN, INPUT, 1);
        setupTest = true;
        break;
      }
      if(!runTest) {
        if( IO_EXP_1.readPin(IOEXP_TEST_PIN) == 0 ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.printlnf("Test Fail: %d", IO_EXP_1.readPin(IOEXP_TEST_PIN));
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 200){
          digitalWrite(PHOTON_TEST_PIN, LOW);
        }
        delay(100);
      }
      break;
    }

    case 7: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Write Pin Now\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        pinMode(PHOTON_TEST_PIN, INPUT_PULLDOWN);
        IO_EXP_1.setMode(IOEXP_TEST_PIN, OUTPUT);
        IO_EXP_1.writePin(IOEXP_TEST_PIN, 0);
        setupTest = true;
        break;
      }
      if(!runTest) {
        if(millis() - testTimer > 200){
          IO_EXP_1.writePinNow(IOEXP_TEST_PIN, 1);
        }

        Serial.printlnf("Photon Test Pin: %d", digitalRead(PHOTON_TEST_PIN));
        if( digitalRead(PHOTON_TEST_PIN) == 1 ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.printlnf("Test Fail: %d", IO_EXP_1.readPin(IOEXP_TEST_PIN));
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 8: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Write Multiple Pins in one update cycle\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        pinMode(PHOTON_TEST_PIN, INPUT_PULLDOWN);

        IO_EXP_1.setMode(IOEXP_TEST_PIN, OUTPUT);
        IO_EXP_1.setMode(GPA6, OUTPUT);
        IO_EXP_1.setMode(GPA5, OUTPUT);
        IO_EXP_1.setMode(GPA4, OUTPUT);
        IO_EXP_1.setMode(GPB0, OUTPUT);
        IO_EXP_1.setMode(GPB1, OUTPUT);

        IO_EXP_1.writePin(IOEXP_TEST_PIN, 1);
        IO_EXP_1.writePin(GPA6, 1);
        IO_EXP_1.writePin(GPA5, 1);
        IO_EXP_1.writePin(GPA4, 0);
        IO_EXP_1.writePin(GPB0, 1);
        IO_EXP_1.writePin(GPB1, 0);
        setupTest = true;
        break;
      }
      if(!runTest) {

        if( IO_EXP_1.readRegister(0x00) == 0x0F //IODIRA
          && IO_EXP_1.readRegister(0x14) == 0xE0 //OLATA
          && IO_EXP_1.readRegister(0x01) == 0xFC //IODIRB
          && IO_EXP_1.readRegister(0x15) == 0x01 //OLATB
          && digitalRead(PHOTON_TEST_PIN) == 1 ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail:");
          Serial.printlnf("\tIODIRA: %d (%d)",IO_EXP_1.readRegister(0x00), 0x0F); //IODIRA
          Serial.printlnf("\tOLATA: %d (%d)",IO_EXP_1.readRegister(0x14), 0xE0); //OLATA
          Serial.printlnf("\tIODIRB: %d (%d)",IO_EXP_1.readRegister(0x01), 0xFC); //IODIRB
          Serial.printlnf("\tOLATB: %d (%d)",IO_EXP_1.readRegister(0x15), 0x01); //OLATB
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 9: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Configure Interrupts IOCON\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        // Reset MCP23017
        digitalWrite(I2C_RESET_PIN, LOW);
        delay(10);
        digitalWrite(I2C_RESET_PIN, HIGH);
        
        IO_EXP_1.configureInterrupts();
        setupTest = true;
        break;
      }
      if(!runTest) {
        if( IO_EXP_1.readRegister(0x0A) == 0x44 ){
          Serial.println("Success");
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.printlnf("Test Fail: %d", IO_EXP_1.readRegister(0x0A));
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }

    case 10: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Set Interupt Pin\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        // Reset MCP23017
        digitalWrite(I2C_RESET_PIN, LOW);
        delay(10);
        digitalWrite(I2C_RESET_PIN, HIGH);
        
        IO_EXP_1.configureInterrupts(1, 0, 1);
        IO_EXP_1.setMode(IOEXP_TEST_PIN, INPUT);
        IO_EXP_1.setInterrupt(IOEXP_TEST_PIN, FALLING);

        pinMode(PHOTON_TEST_PIN, OUTPUT);
        digitalWrite(PHOTON_TEST_PIN, HIGH);

        setupTest = true;
        break;
      }
      if(!runTest) {
        if(millis() - testTimer > 200){
          digitalWrite(PHOTON_TEST_PIN, LOW);
        }


        if(  IO_EXP_1.readRegister(0x04) == 0x80 //GPINTEN
          && IO_EXP_1.readRegister(0x06) == 0x80 //DEFVAL
          && IO_EXP_1.readRegister(0x08) == 0x80 //INTCON
          && IO_EXP_1.readRegister(0x0E) == 0x80 //INTF
          ){
            Serial.println("Success");
            runTest = true;
            testNumber++;
        } else if(millis() - testTimer > 1000){
          Serial.printlnf("Test Fail:\n\tGPINTEN: %d\n\tDEFVAL: %d\n\tINTCON: %d\n\tINTF: %d", IO_EXP_1.readRegister(0x04), IO_EXP_1.readRegister(0x06), IO_EXP_1.readRegister(0x08), IO_EXP_1.readRegister(0x0E));
          runTest = true;
          testNumber++;
        } else {
          Serial.printlnf("Photon Pin: %d", digitalRead(PHOTON_TEST_PIN));
        }
        delay(100);
      }
      break;
    }

    case 11: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Interrupt Mode\n****************\n", testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        // Reset MCP23017
        digitalWrite(I2C_RESET_PIN, LOW);
        delay(10);
        digitalWrite(I2C_RESET_PIN, HIGH);

        // Make sure IOEXP_TEST_PIN is HIGH
        pinMode(PHOTON_TEST_PIN, OUTPUT);
        IO_EXP_1.setModeNow(IOEXP_TEST_PIN, INPUT);
        digitalWrite(PHOTON_TEST_PIN, HIGH);
        IO_EXP_1.readPinNow(IOEXP_TEST_PIN);

        // Configure interrupt mode
        IO_EXP_1.configureInterrupts(1, 0, 1);
        IO_EXP_1.enableInterruptMode();

        // Set Photon Test Pin low
        digitalWrite(PHOTON_TEST_PIN, LOW);

        setupTest = true;
        break;
      }
      if(!runTest) {
        static bool enabledInterrupt = false;
        if(!enabledInterrupt && (millis() - testTimer > 200)){
          // Set PHOTON_TEST_PIN high, then enable interrupts
          digitalWrite(PHOTON_TEST_PIN, HIGH);
          IO_EXP_1.setInterrupt(IOEXP_TEST_PIN, CHANGE);
          enabledInterrupt = true;
        }

        static bool interruptTriggered = false;
        if(millis() - testTimer > 400){
          // Change PHOTON_TEST_PIN state to trigger interrupt
          digitalWrite(PHOTON_TEST_PIN, LOW);
          interruptTriggered = true;
        }

        if( IO_EXP_1.readPin(IOEXP_TEST_PIN) == 0 ){
          if(interruptTriggered){
            Serial.println("Success");
            runTest = true;
            testNumber++;
          } else{
            Serial.println("TEST FAIL: Triggered too early");
            runTest = true;
            testNumber++;
          }
        } else if(millis() - testTimer > 1000){
          Serial.println("Test Fail: Never Triggered");
          runTest = true;
          testNumber++;
        } else {
          Serial.printlnf("Photon Pin: %d", digitalRead(PHOTON_TEST_PIN));
        }
        delay(100);
      }
      break;
    }

    default:{
      static bool AllTestsComplete = false;
      if(!AllTestsComplete){
        Serial.println("\nAll Test Complete");
        AllTestsComplete = true;
        System.dfu();
      }
      break;
    }
      
  }

}