/***************************************************************************
  This is a library for the TCA9535 GPIO expander


  Written by Aaron Storrs.
 ***************************************************************************/
#ifndef __TCA9535_H__
#define __TCA9535_H__
/*
#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
*/
#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define TCA9535_ADDRESS                (0x20)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    enum {
        TCA9535_INPUT_REG0 		            = 0x00,		// Input status register
        TCA9535_INPUT_REG1 		            = 0x01,		// Input status register
        TCA9535_OUTPUT_REG0		            = 0x02,		// Output register to change state of output BIT set to 1, output set HIGH
        TCA9535_OUTPUT_REG1		            = 0x03,		// Output register to change state of output BIT set to 1, output set HIGH
        TCA9535_POLARITY_REG0                       = 0x04,		// Polarity inversion register. BIT '1' inverts input polarity of register 0x00
        TCA9535_POLARITY_REG1                       = 0x05,		// Polarity inversion register. BIT '1' inverts input polarity of register 0x00
        TCA9535_CONFIG_REG0		            = 0x06,		// Configuration register. BIT = '1' sets port to input BIT = '0' sets port to output
        TCA9535_CONFIG_REG1		            = 0x07		// Configuration register. BIT = '1' sets port to input BIT = '0' sets port to output
        
    };

/*=========================================================================*/




class TCA9535 {
    public:
    
        enum pin_setting {
            INPUTa   = 0x01,
            OUTPUTa  = 0x00
        };
        
        enum pin_output {
            ON   = 0x00,
            OFF  = 0x01
        };
    
        // constructors
        TCA9535(void);
		
        bool begin(void);
        bool begin(TwoWire *theWire);
        bool begin(uint8_t addr);
        bool begin(uint8_t addr, TwoWire *theWire);
        bool init(void);


        void setPin(uint8_t pinNumber, pin_setting pinSetting);
        void writePin(uint8_t pinNumber, pin_output pinSetting);

        uint16_t readOutputs(void);
        bool readInputs(uint16_t pinNumber);
        uint16_t readConfiguration(void);
        uint16_t readAllInputs(void);
        uint16_t readPolarity(void);
        void      write8(uint8_t reg, uint8_t value);
        
    private:
        TwoWire *_wire;
        
        uint8_t   read8(uint8_t reg);
        uint8_t   _i2caddr;
        int32_t   _sensorID;
        int32_t   t_fine;

        int8_t _cs, _mosi, _miso, _sck;

};

#endif
