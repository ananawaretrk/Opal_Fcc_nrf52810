/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include "Arduino.h"
#include <Wire.h>
#include <nrf_delay.h>
#include "TCA9535.h"
#include "eventflag_and_errors.h"


extern TwoWire Wire;
/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/
TCA9535::TCA9535(){}


bool TCA9535::init(void)
{
  return true;
}
/**************************************************************************/
/*!
    @brief  Initialise sensor with given parameters / settings
*/
/**************************************************************************/
bool TCA9535::begin(TwoWire *theWire)
{
	_wire = theWire;
	_i2caddr = TCA9535_ADDRESS;
	return init();
}

bool TCA9535::begin(uint8_t addr)
{
	_i2caddr = addr;
	_wire = &Wire;
	return init();
}

bool TCA9535::begin(uint8_t addr, TwoWire *theWire)
{
        _i2caddr = addr;
	_wire = theWire;
	return init();
}

bool TCA9535::begin(void)
{
        _i2caddr = TCA9535_ADDRESS;
	_wire = &Wire;
	return init();
}





/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C or SPI
*/
/**************************************************************************/
void TCA9535::write8(byte reg, byte value) {
        _wire -> beginTransmission((uint8_t)_i2caddr);
        _wire -> write((uint8_t)reg);
        _wire -> write((uint8_t)value);
        int ret = _wire -> endTransmission();
        if(ret == 0){
          printf("!!!WARN: TCA I2C Error\n");
          SetErrorMask(TCA_ERROR_MASK);
        }
}


/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C or SPI
*/
/**************************************************************************/
uint8_t TCA9535::read8(byte reg) {
    uint8_t value;
    
        _wire -> beginTransmission((uint8_t)_i2caddr);
        _wire -> write((uint8_t)reg);
        int ret = _wire -> endTransmission();
        if(ret == 0){
          printf("!!!WARN: TCA I2C Error\n");
          SetErrorMask(TCA_ERROR_MASK);
        }
        ret = _wire -> requestFrom((uint8_t)_i2caddr, (byte)1);
        if(ret == 0){
          printf("!!!WARN: TCA I2C Error\n");
          SetErrorMask(TCA_ERROR_MASK);
        }
        value = _wire -> read();
    return value;
}





/**************************************************************************/
/*!
    Sets a pin to input or output  
    @param  pinNumber     which pin is being configured
    @param  pinSetting    wether its an input or output
*/
/**************************************************************************/
void TCA9535::setPin(uint8_t pinNumber, pin_setting pinSetting)
{
    uint8_t curRegisterBits;
    if(pinNumber < 8) {
        curRegisterBits = read8(TCA9535_CONFIG_REG0);
        if(pinSetting == OUTPUTa) {
            write8(TCA9535_CONFIG_REG0, curRegisterBits & ~(1 << pinNumber));
        } else {
            write8(TCA9535_CONFIG_REG0, curRegisterBits | (1 << pinNumber));
        }
    } else {
        curRegisterBits = read8(TCA9535_CONFIG_REG1);
        if(pinSetting == OUTPUTa) {
            write8(TCA9535_CONFIG_REG1, curRegisterBits & ~(1 << (pinNumber-8)));
        } else {
            write8(TCA9535_CONFIG_REG1, curRegisterBits | (1 << (pinNumber-8)));
        }
    }

}

/**************************************************************************/
/*!
    Sets a pin to input or output  
    @param  pinNumber     which pin is being configured
    @param  pinSetting    wether its on or off
*/
/**************************************************************************/
void TCA9535::writePin(uint8_t pinNumber, pin_output pinSetting)
{
    uint8_t curRegisterBits;
    if(pinNumber < 8) {
        curRegisterBits = read8(TCA9535_OUTPUT_REG0);
        if(pinSetting == OFF) {
            write8(TCA9535_OUTPUT_REG0, curRegisterBits & (uint8_t)~(1 << pinNumber));
        } else {
            write8(TCA9535_OUTPUT_REG0, curRegisterBits | (uint8_t)(1 << pinNumber));
        }
    } else {
        curRegisterBits = read8(TCA9535_OUTPUT_REG1);
        if(pinSetting == OFF) {
            write8(TCA9535_OUTPUT_REG1, curRegisterBits & (uint8_t)~(1 << (pinNumber-8)));
        } else {
            //Serial.print("on");
            write8(TCA9535_OUTPUT_REG1, curRegisterBits | (uint8_t)(1 << (pinNumber-8)));
        }
    }
}

/**************************************************************************/
/*!
    reads the current output pins state in one 16 bit block with gpio 16 being the highest bit
    @return  uint16_t     current output pin configurations
*/
/**************************************************************************/
uint16_t TCA9535::readOutputs(void)
{
        uint16_t curRegisterBits = (read8(TCA9535_OUTPUT_REG1) << 8) | read8(TCA9535_OUTPUT_REG0);
        return curRegisterBits;

}

/**************************************************************************/
/*!
    reads the current pins input or output state
    @return  uint16_t     current pin configurations
*/
/**************************************************************************/
uint16_t TCA9535::readConfiguration(void)
{
        uint16_t curRegisterBits = ((uint16_t)(read8(TCA9535_CONFIG_REG1) << 8)) | read8(TCA9535_CONFIG_REG0);
        return curRegisterBits;

}

uint16_t TCA9535::readPolarity(void)
{
        uint16_t curRegisterBits = ((uint16_t)(read8(TCA9535_POLARITY_REG1) << 8)) | read8(TCA9535_POLARITY_REG0);
        return curRegisterBits;

}

/**************************************************************************/
/*!
    reads the current pins input or output state
    @return  uint16_t     current pin configurations
*/
/**************************************************************************/
uint16_t TCA9535::readAllInputs(void)
{
        uint16_t curRegisterBits = (read8(TCA9535_INPUT_REG1) << 8) | read8(TCA9535_INPUT_REG0);
        return curRegisterBits;

}

/**************************************************************************/
/*!
    reads a selected input pin
    @return  bool     returns true if bit is high and false if low
*/
/**************************************************************************/
bool TCA9535::readInputs(uint16_t pinNumber)
{
        uint8_t tempa;
        if(pinNumber < 8) {
          tempa = (read8(TCA9535_INPUT_REG0) & (0x1 << pinNumber));
        } else {
          //Serial.println(pinNumber-8);
          tempa = (read8(TCA9535_INPUT_REG1) & (0x1 << (pinNumber-8)));
          printf("%d",tempa);
        }
        if(tempa > 0) {
          return true;
        } else {
          return false;
        }

}
