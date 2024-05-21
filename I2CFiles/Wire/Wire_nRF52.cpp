/*
 * TWI/I2C library for nRF5x
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef NRF52840_XXAA

extern "C" {
#include <string.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
}
#include "Wire.h"

TwoWire::TwoWire(const nrfx_twim_t *i2c)
{
  this->_i2c = i2c;
  this->rx_index = 0;
  this->tx_index = 0;
  transmissionBegun = false;
}

void TwoWire::begin(void) {
  //Master Mode
  master = true;

 }

void TwoWire::begin(uint8_t address) {
  //Slave mode
  master = false;

}

void TwoWire::setClock(uint32_t baudrate) {
    nrfx_twim_disable(_i2c);
    uint32_t frequency;
    nrfx_twim_enable(_i2c);
}

void TwoWire::end() {
    nrfx_twim_disable(_i2c);
    nrfx_twim_uninit(_i2c);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, bool stopBit){
  rx_cnt = 0;
  rx_index = 0;
  int fail_count = 30000;
  if(quantity == 0){
    return 0;
  }
  i2c_xfer_done = false;
  i2c_xfer_error = false;
  nrfx_err_t err_code = nrfx_twim_rx(_i2c,address,rxBuffer,quantity);
  if (err_code == NRFX_SUCCESS){
    while(i2c_xfer_done == false && fail_count > 0){
      nrf_delay_ms(1);
      fail_count--;
    }
    if(i2c_xfer_done == true && i2c_xfer_error == false){
      rx_cnt = quantity;
    }else{
      printf("!!!ERROR: I2C Request From Timed out\n");
    }
  }else{
    printf("!!!ERROR: I2C Request From Error is %x\n",err_code);
  }
  return rx_cnt;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity){
  return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
  // save address of target and clear buffer
  txAddress = address;
  tx_index = 0;
  rx_index = 0;
  memset(txBuffer,0,32);
  memset(rxBuffer,0,32);
  transmissionBegun = true;
}

uint8_t TwoWire::endTransmission(bool nostopBit){
  transmissionBegun = false;
  int fail_count = 30000;
  i2c_xfer_error = false;
  i2c_xfer_done = false;
  nrfx_err_t err_code = nrfx_twim_tx(_i2c,txAddress,txBuffer,tx_index,false);
  if (err_code != NRFX_SUCCESS){
    printf("!!!ERROR: I2C End Transmission %x\n",err_code);
    tx_index = 0;
  }
  else{
    while(i2c_xfer_done == false && fail_count > 0){
      nrf_delay_ms(1);
      fail_count--;
    }
    if(i2c_xfer_done == false || i2c_xfer_error == true){
      tx_index = 0;
      printf("!!!ERROR: I2C End Transmission Timed out\n");
    }
  }
  return tx_index;
}

void TwoWire::sendClocks(){
/*    uint8_t i;

    printf("i2c failure, sendClocks called\n");
    //unconfigure i2c
    end();
    //config SCL as gpio
    nrf_gpio_cfg_output(_uc_pinSCL);
    //toggle SCL 16 times, 50kHz
    for(i=0;(i<16);i++)
    {
      nrf_gpio_pin_set(_uc_pinSCL);
      nrf_delay_us(10);
      nrf_gpio_pin_clear(_uc_pinSCL);
      nrf_delay_us(10);
    }
    //configure back to i2c
    begin();
*/
}

uint8_t TwoWire::endTransmission(){
  return endTransmission(false);
}

int TwoWire::available(void){
  return rx_cnt;
}

uint8_t TwoWire::write(uint8_t ucData){
  // No writing, without begun transmission or a full buffer
  if ( !transmissionBegun || tx_index >= 32 ){
    return 0 ;
  }
  txBuffer[tx_index++] = ucData;
  return tx_index ;
}

uint8_t TwoWire::write(const uint8_t *data, uint8_t quantity){
  for(uint8_t i = 0; i < quantity; ++i){
    txBuffer[i] = data[i];
  }
  tx_index = quantity;
  return quantity;
}


int TwoWire::read(void){
  if (rx_index == rx_cnt){
    return 0;
  }
  else{
    return rxBuffer[rx_index++];
  }
}

void TwoWire::onService(void){

}

#endif
