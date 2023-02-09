/*
 * TWI/I2C library for mRF5x
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

#ifndef TwoWire_h
#define TwoWire_h

#include "nrf.h"
#include "nrfx_twim.h"

extern volatile bool i2c_xfer_done;
extern volatile bool i2c_xfer_error; 

class TwoWire
{
  public:
    TwoWire(const nrfx_twim_t *p_twim);

    void begin(uint8_t);
    void begin();
    void end();
    void setClock(uint32_t);
    void sendClocks();
    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool stopBit);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint8_t address, uint8_t quantity, bool stopBit);
    uint8_t requestFrom(uint8_t address, uint8_t quantity);

    uint8_t write(uint8_t data);
    uint8_t write(const uint8_t * data, uint8_t quantity);
    int read(void);

    void onReceive(void(*)(int));
    void onRequest(void(*)(void));
    void onService(void);
    int available(void);

  private:
    const nrfx_twim_t *_i2c;

    bool master;
    bool receiving;
    bool transmissionBegun;
    bool suspended;
    uint8_t txAddress, rx_index, rx_cnt, tx_index;

    // RX Buffer
    uint8_t rxBuffer[32];

    // TX buffer
    uint8_t txBuffer[32];


    // Callback user functions
    void (*onRequestCallback)(void);
    void (*onReceiveCallback)(int);
};


#endif
