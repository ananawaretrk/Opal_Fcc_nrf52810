#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include "nrfx_twim.h"
#define I2C_INSTANCE 1

class I2CWrapper {
private:
  uint32_t i2c_sda;
  uint32_t i2c_scl;
  uint8_t  i2c_priority;
  nrf_twim_frequency_t i2c_frequency;
  nrfx_twim_t i2c;
  I2CWrapper();

public:
  I2CWrapper(uint32_t sda, uint32_t scl, uint8_t priority, nrf_twim_frequency_t frequency = NRF_TWIM_FREQ_100K);
  void InitializeI2C();
  void DeInitializeI2C();
  void I2CScanner();
  nrfx_twim_t* GetI2CInstance();
  void Initialize_MCPI2C(int sda, int scl);
};


#endif