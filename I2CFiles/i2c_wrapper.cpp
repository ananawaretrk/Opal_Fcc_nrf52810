#include "i2c_wrapper.h"

volatile bool i2c_xfer_done;
volatile bool i2c_xfer_error;

static bool i2c_init_status = false;
static void I2CEventHandler(nrfx_twim_evt_t const* p_event, void* p_context) {
  switch (p_event->type){
  case NRFX_TWIM_EVT_DONE:         ///< Transfer completed event.
    break;
  case NRFX_TWIM_EVT_ADDRESS_NACK: ///< Error event: NACK received after sending the address.
    printf("!!!WARN: I2C Wrapper addr nack!\n");
    i2c_xfer_error = true;
    break;
  case NRFX_TWIM_EVT_DATA_NACK:     ///< Error event: NACK received after sending a data byte.
    printf("!!!WARN: I2C Wrapper data nack!\n");
    i2c_xfer_error = true;
    break;
  }
  i2c_xfer_done = true;
}

I2CWrapper::I2CWrapper(uint32_t sda, uint32_t scl, uint8_t priority, nrf_twim_frequency_t frequency){
  i2c_sda = sda;
  i2c_scl = scl;
  i2c_priority = priority;
  i2c_frequency = frequency;
  i2c = NRFX_TWIM_INSTANCE(I2C_INSTANCE);
}

void I2CWrapper::InitializeI2C(){
  //nrfx_twim_uninit(&i2c);
  if(i2c_init_status)
    return;
  nrfx_twim_config_t i2c_config;
  i2c_config.scl = i2c_scl;
  i2c_config.sda = i2c_sda;
  i2c_config.interrupt_priority = i2c_priority;
  i2c_config.hold_bus_uninit = false;
  i2c_config.frequency = i2c_frequency;
  APP_ERROR_CHECK(nrfx_twim_init(&i2c, &i2c_config, I2CEventHandler, NULL));
  nrfx_twim_enable(&i2c);
  i2c_init_status = true;
}

void I2CWrapper::DeInitializeI2C(){
  if(!i2c_init_status)
    return;
  nrfx_twim_uninit(&i2c);
  i2c_init_status = false;
}

nrfx_twim_t* I2CWrapper::GetI2CInstance(){
  return &i2c;
}


void I2CWrapper::Initialize_MCPI2C(int sda, int scl){
  if(i2c_init_status)
    return;
  nrfx_twim_config_t i2c_config;
  i2c_config.scl = scl;
  i2c_config.sda = sda;
  i2c_config.interrupt_priority = i2c_priority;
  i2c_config.hold_bus_uninit = false;
  i2c_config.frequency = i2c_frequency;
  APP_ERROR_CHECK(nrfx_twim_init(&i2c, &i2c_config, I2CEventHandler, NULL));
  nrfx_twim_enable(&i2c);
  i2c_init_status = true;
}


void I2CWrapper::I2CScanner() {
  ret_code_t err_code;
  uint8_t address;
  uint8_t sample_data;
  bool detected_device = false;

  for (address = 1; address <= 127; address++) {
    err_code = nrfx_twim_tx(&i2c, address, &sample_data, 0, false);
    if (err_code == NRF_SUCCESS) {
      detected_device = true;
      printf("I2C device detected at address 0x%x.", address << 1);
    }
  }
  if (!detected_device) {
    printf("No I2C device was found.");
  }
}