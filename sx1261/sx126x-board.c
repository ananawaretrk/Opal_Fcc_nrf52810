#include <string.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"

#include "sx126x.h"
#include "sx126x-board.h"

/*!
 * Antenna switch GPIO pins objects
 */
uint8_t AntPow;
uint8_t DeviceSel;

#define SPI_INSTANCE  0
static const    nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool spi_xfer_done = false;

SX126x_t SX126x = 
{
    .Reset  = 27,
    .BUSY   = 26,
    .DIO1   = 6,
};

void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context);

void SX126xIoInit( void )
{
    nrf_gpio_cfg_output(SX126x.Reset);
    nrf_gpio_cfg_input(SX126x.BUSY, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(SX126x.DIO1, NRF_GPIO_PIN_NOPULL);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = 22;
    spi_config.miso_pin = 24;
    spi_config.mosi_pin = 23;
    spi_config.sck_pin  = 25;
    spi_config.frequency = NRF_DRV_SPI_FREQ_250K;

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    SX126xReset();
}

void SX126xIoDeInit( void )
{
    nrf_gpio_cfg_default(SX126x.Reset);
    nrf_gpio_cfg_default(SX126x.BUSY);
    nrf_gpio_cfg_default(SX126x.DIO1);
}

void SX126xReset( void )
{
    nrf_delay_ms( 1 );
    nrf_gpio_pin_clear(SX126x.Reset);
    nrf_delay_ms( 2 );
    nrf_gpio_pin_set(SX126x.Reset);
    nrf_delay_ms( 1 );
}

void SX126xWakeup( void )
{
    uint8_t  m_tx_buf[2] = {RADIO_GET_STATUS, 0x0};
    uint8_t  m_rx_buf[2] = {0};
    uint8_t  m_tx_len = sizeof(m_tx_buf);
    uint8_t  m_rx_len = sizeof(m_rx_buf);

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t  m_tx_buf[32];
    uint8_t  m_rx_buf[32] = {0};

    m_tx_buf[0] = command;
    memcpy(&m_tx_buf[1], buffer, size);
    uint8_t  m_tx_len = size + 1;
    uint8_t  m_rx_len = size + 1;
    
    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);
}

void SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t m_tx_buf[] = {command, 0x0};
    uint8_t m_rx_buf[32];
    uint8_t m_tx_len = sizeof(m_tx_buf);
    uint8_t m_rx_len = size + 1;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);
    memcpy(&m_rx_buf[1], buffer, size);
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    uint8_t  m_tx_buf[32];
    uint8_t  m_rx_buf[32] = {0};

    m_tx_buf[0] = RADIO_WRITE_REGISTER;
    m_tx_buf[1] = (( address & 0xFF00 ) >> 8 );
    m_tx_buf[2] = ( address & 0x00FF );
    memcpy(&m_tx_buf[3], buffer, size);

    uint8_t m_tx_len    = size + 0x3;
    uint8_t m_rx_len    = m_tx_len;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_tx_len, m_rx_buf, size));
    while(spi_xfer_done == false);
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    uint8_t m_tx_buf[8];
    uint8_t m_tx_len    = 0x4;

    m_tx_buf[0] = RADIO_WRITE_REGISTER;
    m_tx_buf[1] = (( address & 0xFF00 ) >> 8 );
    m_tx_buf[2] = ( address & 0x00FF );
    m_tx_buf[3] = 0x00;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_tx_len, buffer, size));
    while(spi_xfer_done == false);
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    spi_xfer_done = true;
}
