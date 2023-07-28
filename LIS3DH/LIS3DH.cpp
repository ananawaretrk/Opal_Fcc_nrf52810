#include "LIS3DH.h"
#include "eventflag_and_errors.h"
extern TwoWire Wire;

LIS3DH::LIS3DH()
{
}
/*!
 *  @brief  Setups the HW (reads coefficients values, etc.)
 *  @param  i2caddr
 *          i2c address (optional, fallback to default)
 *  @param  nWAI
 *          Who Am I register value - defaults to 0x33 (LIS3DH)
 *  @return true if successful
 */
bool LIS3DH::sleep(void){
  writeRegister8(LIS3DH_REG_CTRL1, 0x0000);
  return true;
}

bool LIS3DH::begin(uint8_t i2caddr, uint8_t wai) {
  _i2caddr = i2caddr;

  /* Check connection */
  uint8_t deviceid = readRegister8(LIS3DH_REG_WHOAMI);
  if (deviceid != wai)
    printf("LIS3DH returned wrong device id, %02x should be %02x\n",deviceid,wai);

  // enable all axes, normal mode
  //writeRegister8(LIS3DH_REG_CTRL1, 0x07);
  //writeRegister8(LIS3DH_REG_CTRL1, 0x04); // Z axis only
  
  // 400Hz rate
  //writeRegister8(0x1E,0x2F);
  //setDataRate(LIS3DH_DATARATE_400_HZ);
  writeRegister8(LIS3DH_REG_CTRL1,0x2F);
  
  //setting range - 2g
   writeRegister8(LIS3DH_REG_CTRL4, 0);  
  // High res & BDU enabled
  //writeRegister8(LIS3DH_REG_CTRL4, 0x88);

  // DRDY on INT1
  //writeRegister8(LIS3DH_REG_CTRL3, 0x10);

  // Turn on orientation config
  // writeRegister8(LIS3DH_REG_PL_CFG, 0x40);

  // enable adcs
  //writeRegister8(LIS3DH_REG_TEMPCFG, 0x80);


  return true;
}

/*!
 *  @brief  Get Device ID from LIS3DH_REG_WHOAMI
 *  @return WHO AM I value
 */
uint8_t LIS3DH::getDeviceID() {
  return readRegister8(LIS3DH_REG_WHOAMI);
}

/*!
 *  @brief  Check to see if new data available
 *  @return true if there is new data available, false otherwise
 */
bool LIS3DH::haveNewData() {
	// checking ZYXDA in REG_STATUS2 tells us if data available
 	return (readRegister8(LIS3DH_REG_STATUS2) & 0x8) >> 3;
}

/*!
 *  @brief  Reads x y z values at once
 */
void LIS3DH::read() {

    Wire.beginTransmission(_i2caddr);
    Wire.write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
    Wire.endTransmission();

    Wire.requestFrom(_i2caddr, 6);
    x = Wire.read();
    x |= ((uint16_t)Wire.read()) << 8;
    y = Wire.read();
    y |= ((uint16_t)Wire.read()) << 8;
    z = Wire.read();
    z |= ((uint16_t)Wire.read()) << 8;

    uint8_t range = getRange();
    uint16_t divider = 1;
    if (range == LIS3DH_RANGE_16_G)
      divider = 1365; // different sensitivity at 16g
    if (range == LIS3DH_RANGE_8_G)
      divider = 4096;
    if (range == LIS3DH_RANGE_4_G)
      divider = 8190;
    if (range == LIS3DH_RANGE_2_G)
      divider = 16380;

    x_g = (float)x / divider;
    y_g = (float)y / divider;
    z_g = (float)z / divider;
}

void LIS3DH::readHLIS() {

    Wire.beginTransmission(_i2caddr);
    Wire.write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
    Wire.endTransmission();

    Wire.requestFrom(_i2caddr, 6);
    x = Wire.read();
    x |= ((uint16_t)Wire.read()) << 8;
    y = Wire.read();
    y |= ((uint16_t)Wire.read()) << 8;
    z = Wire.read();
    z |= ((uint16_t)Wire.read()) << 8;

    uint16_t scale_max = 100;
  //if (range == H3LIS331_RANGE_100_G)
  //  scale_max = 100;
  //if (range == H3LIS331_RANGE_200_G)
  //  scale_max = 200;
  //if (range == H3LIS331_RANGE_400_G)
  //  scale_max = 400;

  float lsb_value = 2 * scale_max * (float)1 / 4096;

  x_g = ((float)x * lsb_value);
  y_g = ((float)y * lsb_value);
  z_g = ((float)z * lsb_value);
}


/*!
 *  @brief  Read the auxilary ADC
 *  @param  adc
 *          adc index. possible values (1, 2, 3).
 *  @return auxilary ADC value
 */
int16_t LIS3DH::readADC(uint8_t adc) {
  if ((adc < 1) || (adc > 3))
    return 0;
  uint16_t value;

  adc--;

  uint8_t reg = LIS3DH_REG_OUTADC1_L + adc * 2;

  // i2c
  Wire.beginTransmission(_i2caddr);
  Wire.write(reg | 0x80); // 0x80 for autoincrement
  Wire.endTransmission();
  Wire.requestFrom(_i2caddr, 2);
  value = Wire.read();
  value |= ((uint16_t)Wire.read()) << 8;

  return value;
}

/*!
 *   @brief  Set INT to output for single or double click
 *   @param  c
 *					 0 = turn off I1_CLICK
 *           1 = turn on all axes & singletap
 *					 2 = turn on all axes & doubletap
 *   @param  clickthresh
 *           CLICK threshold value
 *   @param  timelimit
 *           sets time limit (default 10)
 *   @param  timelatency
 *   				 sets time latency (default 20)
 *   @param  timewindow
 *   				 sets time window (default 255)
 */
void LIS3DH::setClick(uint8_t c, uint8_t clickthresh,
                               uint8_t timelimit, uint8_t timelatency,
                               uint8_t timewindow) {
  if (!c) {
    // disable int
    uint8_t r = readRegister8(LIS3DH_REG_CTRL3);
    r &= ~(0x80); // turn off I1_CLICK
    writeRegister8(LIS3DH_REG_CTRL3, r);
    writeRegister8(LIS3DH_REG_CLICKCFG, 0);
    return;
  }
  // else...

  writeRegister8(LIS3DH_REG_CTRL3, 0x80); // turn on int1 click
  writeRegister8(LIS3DH_REG_CTRL5, 0x08); // latch interrupt on int1

  if (c == 1)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x15); // turn on all axes & singletap
  if (c == 2)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x2A); // turn on all axes & doubletap

  writeRegister8(LIS3DH_REG_CLICKTHS, clickthresh);    // arbitrary
  writeRegister8(LIS3DH_REG_TIMELIMIT, timelimit);     // arbitrary
  writeRegister8(LIS3DH_REG_TIMELATENCY, timelatency); // arbitrary
  writeRegister8(LIS3DH_REG_TIMEWINDOW, timewindow);   // arbitrary
}

/*!
 *   @brief  Get uint8_t for single or double click
 *   @return register LIS3DH_REG_CLICKSRC
 */
uint8_t LIS3DH::getClick() 
{
  return readRegister8(LIS3DH_REG_CLICKSRC);
}

/*!
 *   @brief  Sets the g range for the accelerometer
 *   @param  range
 *           range value
 */
void LIS3DH::setRange(lis3dh_range_t range) {
  uint8_t r = readRegister8(LIS3DH_REG_CTRL4);
  r &= ~(0x30);
  r |= range << 4;
  writeRegister8(LIS3DH_REG_CTRL4, r);
}

/*!
 *  @brief  Gets the g range for the accelerometer
 *  @return Returns g range value
 */
lis3dh_range_t LIS3DH::getRange() {
  /* Read the data format register to preserve bits */
  return (lis3dh_range_t)((readRegister8(LIS3DH_REG_CTRL4) >> 4) & 0x03);
}

/*!
 *  @brief  Sets the data rate for the LIS3DH (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void LIS3DH::setDataRate(lis3dh_dataRate_t dataRate) {
  uint8_t ctl1 = readRegister8(LIS3DH_REG_CTRL1);
  ctl1 &= ~(0xF0); // mask off bits
  ctl1 |= (dataRate << 4);
  writeRegister8(LIS3DH_REG_CTRL1, ctl1);
}

/*!
 *   @brief  Gets the data rate for the LIS3DH (controls power consumption)
 *   @return Returns Data Rate value
 */
uint8_t LIS3DH::getDataRate() {
  return (lis3dh_dataRate_t)((readRegister8(LIS3DH_REG_CTRL1) >> 4) & 0x0F);
}


/*!
 *  @brief  Writes 8-bits to the specified destination register
 *  @param  reg
 *          register address
 *  @param  value
 *          value that will be written into selected register
 */
void LIS3DH::writeRegister8(uint8_t reg, uint8_t value) 
{
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    int ret = Wire.endTransmission();
    if(ret == 0){
      printf("!!!WARN: LIS3DH I2C Error\n");
      SetErrorMask(ACC_ERROR_MASK);
    }
}

/*!
 *  @brief  Reads 8-bits from the specified register
 *  @param  reg
 *          register address
 *  @return read value
 */
uint8_t LIS3DH::readRegister8(uint8_t reg) {
    uint8_t value;

    Wire.beginTransmission(_i2caddr);
    Wire.write((uint8_t)reg);
    int ret = Wire.endTransmission();
    if(ret == 0){
      printf("!!!WARN: LIS3DH I2C Error\n");
      SetErrorMask(ACC_ERROR_MASK);
    }

    ret = Wire.requestFrom(_i2caddr, 1);
    if(ret == 0){
      printf("!!!WARN: LIS3DH I2C Error\n");
      SetErrorMask(ACC_ERROR_MASK);
    }
    value = Wire.read();
    return value;
}

void LIS3DH::controlInt(bool onoff,int threshold, int duration)
{
  if(onoff){
  //writeRegister8(LIS3DH_REG_INT1CFG,flags);
  // From app note: STM AN3308
  // low power mode with 10 Hz sampling
    writeRegister8(LIS3DH_REG_CTRL1,0x2F);
  //filter disabled
    writeRegister8(LIS3DH_REG_CTRL2,0);
  //int1 pin used
    writeRegister8(LIS3DH_REG_CTRL3,0x40);
  
  //setting range - 4g
    writeRegister8(LIS3DH_REG_CTRL4, 0X10);
      
  // latch interrupt signal
    writeRegister8(LIS3DH_REG_CTRL5, 0x08);
    //writeRegister8(LIS3DH_REG_CTRL5, 0x00);
  //setting threshold - 500 mg
    if(threshold >= 32 && threshold <= 4096){
      uint8_t mask = 0;
      mask = (threshold / 32) - 1;
      printf("Acc Threshold setting Mask is %x\n",mask);
      writeRegister8(LIS3DH_REG_INT1THS,mask);
    }
  //duration before interrupt trigger
    writeRegister8(LIS3DH_REG_INT1DUR,duration);
    
   //setup interrupt along x,y,z axes
   //writeRegister8(LIS3DH_REG_INT1CFG,0x8A);
   writeRegister8(LIS3DH_REG_INT1CFG,0xAA);
   //printf("int reg contents 0x%X\n",readRegister8(LIS3DH_REG_INT1CFG));
   }else{
    //disable interrupt
    writeRegister8(LIS3DH_REG_INT1CFG,0);
   }
}

void LIS3DH::enableFilteredIntHLIS(){
  writeRegister8(LIS3DH_REG_CTRL1,0xC7);
  writeRegister8(LIS3DH_REG_CTRL2,0x94);
  writeRegister8(LIS3DH_REG_CTRL3,0x04);
  writeRegister8(LIS3DH_REG_CTRL4, 0X00); 
  writeRegister8(LIS3DH_REG_CTRL5, 0x08);
  writeRegister8(LIS3DH_REG_INT1THS,0x01);
  writeRegister8(LIS3DH_REG_INT1CFG,0x2A);
}
void LIS3DH::controlFilteredRelativeInt(bool onoff,int threshold, int duration)
{
  if(onoff){
    writeRegister8(LIS3DH_REG_CTRL1,0x57);
    writeRegister8(LIS3DH_REG_CTRL2,0x09);
    writeRegister8(LIS3DH_REG_CTRL3,0x40);
    writeRegister8(LIS3DH_REG_CTRL4, 0X00); // Setting Max Res as 2G
    writeRegister8(LIS3DH_REG_CTRL5, 0x08); // Interrupt Latched
    if(threshold >= 16 && threshold <= 2048){
      uint8_t mask = 0;
      mask = (threshold / 16) - 1;
      printf("Acc Threshold setting Mask is %x\n",mask);
      writeRegister8(LIS3DH_REG_INT1THS,mask);
    }
    readRegister8(LIS3DH_REG_REFERENCE);
    writeRegister8(LIS3DH_REG_INT1DUR,duration);
    writeRegister8(LIS3DH_REG_INT1CFG,0x2A);
   }else{
    //disable interrupt
    writeRegister8(LIS3DH_REG_INT1CFG,0);
   }
}

 

void LIS3DH::controlmotionInt(bool onoff,int threshold)
{
  //writeRegister8(LIS3DH_REG_CTRL3,onoff<<6);

  //if(!onoff){ // disable interrupt and exit
  //return;
  //}
  
  //// latch interrupt until serviced
  // writeRegister8(LIS3DH_REG_CTRL5,0x08);
   
  // lis3dh_range_t currRange = getRange();

  // if(threshold > currRange)
  //  threshold = currRange;
  
  //// set threshold to trigger
  // writeRegister8(LIS3DH_REG_INT1THS, 0x7f & threshold);
  //// set duration to trigger after
  // writeRegister8(LIS3DH_REG_INT1DUR,duration & 0x7f);
  //// set modes
   
   if(onoff){
  //writeRegister8(LIS3DH_REG_INT1CFG,flags);
  // From app note: STM AN3308
  // low power mode with 10 Hz sampling
    writeRegister8(LIS3DH_REG_CTRL1,0x2F);
  //filter disabled
    writeRegister8(LIS3DH_REG_CTRL2,0x00);
  //int1 pin used
    writeRegister8(LIS3DH_REG_CTRL3,0x40);
  //setting range - 2g
    writeRegister8(LIS3DH_REG_CTRL4, 0);  
  // latch interrupt signal
    writeRegister8(LIS3DH_REG_CTRL5, 0x08);
  //setting threshold - 500 mg
    writeRegister8(LIS3DH_REG_INT1THS, threshold);
  //duration before interrupt trigger
    writeRegister8(LIS3DH_REG_INT1DUR,2);
  //setup interrupt along x and y axes
    writeRegister8(LIS3DH_REG_INT1CFG,0x0A);
   }else{
    //disable interrupt
    writeRegister8(LIS3DH_REG_INT1CFG,0);
   }
}

uint8_t LIS3DH::serviceInt()
{
  //uint8_t datactrl = readRegister8(LIS3DH_REG_INT1CFG);
  //printf("verifying contents at LIS3DH_REG_INT1CFG 0x%X\n",datactrl);
  return readRegister8(LIS3DH_REG_INT1SRC);
}

uint8_t LIS3DH::serviceReferenceInt()
{
  uint8_t refReg = readRegister8(LIS3DH_REG_REFERENCE);
  return readRegister8(LIS3DH_REG_INT1SRC);
}