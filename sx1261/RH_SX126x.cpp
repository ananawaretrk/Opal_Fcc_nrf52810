#include <string.h>
#include "nrfx_gpiote.h"

#include "RH_SX126x.h"

#define LORA_BUSY      20

static bool ImageCalibrated = false;
static bool isInitialised = false;
static RadioOperatingModes_t OperatingMode = MODE_MAX;

extern volatile bool spi_xfer_done;
extern void Lora_recieved_callback(uint8_t* buffer, uint8_t len);

RH_SX126x::RH_SX126x(void)
{
}

bool RH_SX126x::init(const nrf_drv_spi_t* spi_ins, uint32_t pin)
{
    _spi = spi_ins;
    _interruptPin = pin;
    (void)_interruptPin;

    //Set header
    _header[0] = 0x7F;
    _header[1] = 0xFF;
    _header[2] = 0xFF;
    _header[3] = 0x00;

    _Pparams.PacketType = PACKET_TYPE_LORA;
    _Pparams.Params.LoRa.PreambleLength = 8;
    _Pparams.Params.LoRa.HeaderType = LORA_PACKET_EXPLICIT;
    _Pparams.Params.LoRa.PayloadLength = MAX_PAYLOAD_LEN;
    _Pparams.Params.LoRa.CrcMode = LORA_CRC_ON;
    _Pparams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;

    _Mparams.PacketType = PACKET_TYPE_LORA;
    _Mparams.Params.LoRa.SpreadingFactor = LORA_SF7;
    _Mparams.Params.LoRa.Bandwidth = LORA_BW_500;
    _Mparams.Params.LoRa.CodingRate = LORA_CR_4_5;
    _Mparams.Params.LoRa.LowDatarateOptimize = 0;

    _rxContinuous = true;

    setModeIdle();

    // Clear all interrupts
    RadioIrqMasks_t irq_masks = IRQ_RADIO_ALL;
    spiWriteCommand(RADIO_CLR_IRQSTATUS, (uint8_t *)&irq_masks, 1);

    // WORKAROUND
    // Set up Lora Busy Pin
    nrfx_gpiote_in_config_t pin_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    ret_code_t result = nrfx_gpiote_in_init(LORA_BUSY, &pin_config, NULL);
    while(nrf_gpio_pin_read(LORA_BUSY));
    // WORKAROUND

    // Use DC DC regulator
    RadioRegulatorMode_t mode = USE_DCDC;
    spiWriteCommand(RADIO_SET_REGULATORMODE, (uint8_t *)&mode, 1 );

    // set base address for Rx and Tx
    uint8_t buf[2];
    buf[0] = 0x00; // txBaseAddress
    buf[1] = 0x00; // rxBaseAddress
    spiWriteCommand(RADIO_SET_BUFFERBASEADDRESS, buf, 2);

    // Set dio2 as antenna switch control
    setDio2AsRfSwitchCtrl(true);
    setDio3AsTcxoCtrl(TCXO_CTRL_2_2V, 320);

    setDioIrqParams(IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE);

    // Set Packet Type to LORA
    RadioPacketTypes_t packetType = PACKET_TYPE_LORA;
    spiWriteCommand(RADIO_SET_PACKETTYPE, (uint8_t *)&packetType, 1);

    setModulationParams(&_Mparams);
    setPacketParams(&_Pparams);

    return true;
}

void RH_SX126x::Wakeup(void)
{
    uint8_t buf[2];

    buf[0] = RADIO_GET_STATUS;
    buf[1] = 0x00;
    //spiWriteCommand(RADIO_SET_RX, buf, 2);
    uint8_t m_tx_buf[32] = {0};
    uint8_t m_rx_buf[32] = {0};
    uint8_t m_tx_len     = 2 + 1;
    uint8_t m_rx_len     = 2 + 1;
    uint32_t result = 0;

    m_tx_buf[0] = RADIO_SET_RX;
    m_tx_buf[1] = RADIO_GET_STATUS;
    m_tx_buf[2] = 0;
    //memcpy((m_tx_buf + 1), buf, 2);

    nrf_gpio_pin_clear(4);
    nrf_delay_ms(1);

    spi_xfer_done = false;
    ret_code_t err_code = nrf_drv_spi_transfer(_spi, m_tx_buf, 3, m_rx_buf, 3);
    APP_ERROR_CHECK(err_code);
    if (err_code != NRF_SUCCESS) {
      printf("send command fail\n");
    }

    while(spi_xfer_done == false);

    waitOnBusy();
}


void RH_SX126x::checkBusy(void) 
{
    if(( OperatingMode == MODE_SLEEP ) || ( OperatingMode == MODE_RX_DC ))
    {
        Wakeup();
    }

    waitOnBusy();
}

void RH_SX126x::waitOnBusy(void) 
{
    while(nrf_gpio_pin_read(LORA_BUSY) == 1);
}

void RH_SX126x::spiReadCommand(RadioCommands_t cmd, uint8_t* dest, uint8_t len)
{
    checkBusy();

    uint8_t m_tx_buf[32] = {0};
    uint8_t m_rx_buf[32] = {0};
    uint8_t m_tx_len     = len + 2;
    uint8_t m_rx_len     = len + 2;

    m_tx_buf[0] = cmd;
    m_tx_buf[1] = 0x00;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);

    memcpy(dest, (unsigned char *)(m_rx_buf + 2), len);

    waitOnBusy();
}

void RH_SX126x::spiReadAddr(uint16_t addr, uint8_t* dest, uint8_t len)
{
    
    checkBusy();

    uint8_t m_tx_buf[32] = {0};
    uint8_t m_rx_buf[32] = {0};
    uint8_t m_tx_len     = len + 4;
    uint8_t m_rx_len     = len + 4;

    m_tx_buf[0] = RADIO_READ_REGISTER;
    m_tx_buf[1] = (( addr & 0xFF00 ) >> 8 );
    m_tx_buf[2] = ( addr & 0x00FF );
    m_tx_buf[3] = 0x00;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);

    memcpy(dest, m_rx_buf, len);

    waitOnBusy();
}

uint8_t RH_SX126x::spiReadReg(uint16_t reg)
{
    uint32_t result;
    uint8_t tx_byte[5];
    uint8_t rx_byte[5];

    checkBusy();

    tx_byte[0] = RADIO_READ_REGISTER;
    tx_byte[1] = ( reg & 0xFF00 ) >> 8 ;
    tx_byte[2] = ( reg & 0x00FF );
    tx_byte[3] = 0;
    tx_byte[4] = 0;
    
    spi_xfer_done = false;
    result = nrf_drv_spi_transfer(_spi, tx_byte, 5, rx_byte, 5);
    
    if (result != NRF_SUCCESS)
    {
        return 0;
    }

    while(spi_xfer_done == false) ;

    waitOnBusy();

   return rx_byte[4];
}

void RH_SX126x::spiReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    uint8_t m_tx_buf[255] = {0};
    uint8_t m_rx_buf[255] = {0};
    uint8_t m_tx_len      = size + 3;
    uint8_t m_rx_len      = size + 3;

    checkBusy();

    m_tx_buf[0] = RADIO_READ_BUFFER;
    m_tx_buf[1] = offset;
    m_tx_buf[2] = 0x00;

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);

    memcpy(buffer, (m_rx_buf + 3), size);

    waitOnBusy();
}

uint32_t RH_SX126x::spiWriteCommand(RadioCommands_t cmd, const uint8_t* src, uint8_t len)
{
    uint8_t m_tx_buf[32] = {0};
    uint8_t m_rx_buf[32] = {0};
    uint8_t m_tx_len     = len + 1;
    uint8_t m_rx_len     = len + 1;
    uint32_t result = 0;

    checkBusy();

    m_tx_buf[0] = cmd;
    memcpy((m_tx_buf + 1), src, len);

    spi_xfer_done = false;
    ret_code_t err_code = nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len);
    APP_ERROR_CHECK(err_code);
    if (err_code != NRF_SUCCESS)
    {
        printf("send command fail\n");
        return err_code;
    }

    while(spi_xfer_done == false);

    if(cmd != RADIO_SET_SLEEP)
    {
      waitOnBusy();
    }

    return err_code;
}

void RH_SX126x::spiWriteAddr(uint16_t addr, const uint8_t* src, uint8_t len)
{
    uint8_t m_tx_buf[32] = {0};
    uint8_t m_rx_buf[32] = {0};
    uint8_t m_tx_len     = len + 3;
    uint8_t m_rx_len     = len + 3;

    checkBusy();

    m_tx_buf[0] = RADIO_WRITE_REGISTER;
    m_tx_buf[1] = (( addr & 0xFF00 ) >> 8 );
    m_tx_buf[2] = ( addr & 0x00FF );
    memcpy((m_tx_buf + 3), src, len);

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, NULL, 0));
    while(spi_xfer_done == false);

    waitOnBusy();
}

void RH_SX126x::spiWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    uint8_t m_tx_buf[257] = {0};
    uint8_t m_rx_buf[257] = {0};
    uint8_t m_tx_len     = size + 2;
    uint8_t m_rx_len     = size + 2;

    checkBusy();

    m_tx_buf[0] = RADIO_WRITE_BUFFER;
    m_tx_buf[1] = offset;
    memcpy((m_tx_buf + 2), buffer, size);

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(_spi, m_tx_buf, m_tx_len, m_rx_buf, m_rx_len));
    while(spi_xfer_done == false);

    waitOnBusy();
}

bool RH_SX126x::available(void)
{
    if(OperatingMode == MODE_SLEEP)
    {
        setModeIdle();
    }

    if (OperatingMode == MODE_TX)
    {
        return false;
    }

    setModeRx();
    return _rxBufValid; // Will be set by the interrupt handler when a good message is received
}

bool RH_SX126x::checkPacketSent()
{
    if (OperatingMode != MODE_TX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool RH_SX126x::recv(uint8_t* buf, uint8_t len)
{
    if (!available())
    {
        return false;
    }

    if (_rxPayload && _rxPayloadSize)
    {
        len = _rxPayloadSize-HEADER_LEN;
        memcpy(buf, _rxPayload+HEADER_LEN, len);

        for(uint16_t i=0; (i<len); i++)
        {
            printf("%c",buf[i]);
            printf("\n");
        }
    }

    clearRxBuf(); // This message accepted and cleared
    return true;
}

void RH_SX126x::clearRxBuf(void)
{
    _rxBufValid = false;
    _rxPayloadSize = 0;
}

void RH_SX126x::validateRxBuf()
{
    if (_rxPayloadSize < 4)
    {
        return; // Too short to be a real message
    }

    if(memcmp(_header, _rxPayload, 4) == 0)
    {
        _rxGood++;
        _rxBufValid = true;
    }
    else
    {
        uint8_t buf[2];
        buf[0] = 0x00; // txBaseAddress
        buf[1] = 0x00; // rxBaseAddress
        spiWriteCommand(RADIO_SET_BUFFERBASEADDRESS, buf, 2 );
    }
}

bool RH_SX126x::send(uint8_t* data, uint8_t len)
{
    if(len > (MAX_PAYLOAD_LEN - HEADER_LEN))
    {
        return false;
    }

    if(OperatingMode != MODE_STDBY_RC)
    {
        setModeIdle();
    }

    uint8_t packetType[] = {PACKET_TYPE_LORA};
    spiWriteCommand(RADIO_SET_PACKETTYPE, packetType, 1);

    //Append header
    if(_Pparams.Params.LoRa.HeaderType == LORA_PACKET_EXPLICIT)
    {
        uint8_t dataWHeader[MAX_PAYLOAD_LEN];
        memset(dataWHeader, 0, MAX_PAYLOAD_LEN);
        memcpy(dataWHeader, _header, 4);
        memcpy(dataWHeader+4, data, len);
        spiWriteBuffer(0x00, dataWHeader, (len + HEADER_LEN));
        _Pparams.Params.LoRa.PayloadLength = (len + HEADER_LEN);
    }
    else
    {
        spiWriteBuffer(0x00, data, len);
        _Pparams.Params.LoRa.PayloadLength = len;
    }

    nrf_delay_ms(1);

    setModulationParams(&_Mparams);

    nrf_delay_ms(1);

    setPacketParams(&_Pparams);

    nrf_delay_ms(1);

    setDioIrqParams(IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT, IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                    IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    nrf_delay_ms(1);

    setModeTx();

    return true;
}

void RH_SX126x::calibrateImage( float freq )
{
    uint8_t calFreq[2];

    if( freq > 900000000 )
    {
        calFreq[0] = 0xE1;
        calFreq[1] = 0xE9;
    }
    else if( freq > 850000000 )
    {
        calFreq[0] = 0xD7;
        calFreq[1] = 0xD8;
    }
    else if( freq > 770000000 )
    {
        calFreq[0] = 0xC1;
        calFreq[1] = 0xC5;
    }
    else if( freq > 460000000 )
    {
        calFreq[0] = 0x75;
        calFreq[1] = 0x81;
    }
    else if( freq > 425000000 )
    {
        calFreq[0] = 0x6B;
        calFreq[1] = 0x6F;
    }

    spiWriteCommand(RADIO_CALIBRATEIMAGE, calFreq, sizeof(calFreq));
}

void RH_SX126x::setFrequency(float frequency)
{
    uint8_t buf[4];
    uint32_t freq = 0;

    if( ImageCalibrated == false )
    {
        calibrateImage( frequency * 1000000.0 );
        ImageCalibrated = true;
    }

    freq = ( uint32_t )( (( double )frequency * 1000000.0) / ( double )FREQ_STEP );
    buf[0] = ( uint8_t )( ( freq >> 24 ) & 0xFF );
    buf[1] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( freq & 0xFF );
    spiWriteCommand(RADIO_SET_RFFREQUENCY, buf, sizeof(buf));
}

void RH_SX126x::setModeIdle(void)
{
    OperatingMode = MODE_STDBY_RC;

    RadioStandbyModes_t standbyConfig = STDBY_RC;
    spiWriteCommand(RADIO_SET_STANDBY, ( uint8_t* )&standbyConfig, 1);
}

void RH_SX126x::setModeRx(void)
{
    uint8_t buf[3];
    uint32_t timeout = 0;

    if(OperatingMode != MODE_RX)
    {
        OperatingMode = MODE_RX;

        buf[0] = 0x00; // txBaseAddress
        buf[1] = 0x00; // rxBaseAddress
        spiWriteCommand(RADIO_SET_BUFFERBASEADDRESS, buf, 2 );

        setDioIrqParams(IRQ_RX_DONE, IRQ_RX_DONE, IRQ_RADIO_NONE, IRQ_RADIO_NONE);

        buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
        buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
        buf[2] = ( uint8_t )( timeout & 0xFF );

        spiWriteCommand(RADIO_SET_RX, buf, sizeof(buf));
    }
}

void RH_SX126x::setModeTx(void)
{
    OperatingMode = MODE_TX;
    uint8_t buf[3];
    uint32_t timeout = 0;

    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );

    spiWriteCommand(RADIO_SET_TX, buf, sizeof(buf));
}

void RH_SX126x::setTxContinuousWave(void)
{
    OperatingMode = MODE_TX;
    spiWriteCommand(RADIO_SET_TXCONTINUOUSWAVE, NULL, 0);
}

void RH_SX126x::setTxInfinitePreamble(void)
{
    OperatingMode = MODE_TX;
    spiWriteCommand(RADIO_SET_TXCONTINUOUSPREAMBLE, NULL, 0);
}

void RH_SX126x::setPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut)
{
    uint8_t buf[4];

    buf[0] = paDutyCycle;
    buf[1] = hpMax;
    buf[2] = deviceSel;
    buf[3] = paLut;

    spiWriteCommand( RADIO_SET_PACONFIG, buf, sizeof(buf));
}

void RH_SX126x::setTxPower(int8_t power, bool sx1261_chip)
{
    uint8_t buf[2];

    if( sx1261_chip )
    {
      //SX1261
      printf("SX1261 CHIP SELECTED\n");
      setPaConfig(0x06, 0x00, 0x01, 0x01);
      if (14 > power < -17) {
        power = 14;
      }
      
      // Set max current to 60mA
      buf[0] = 0x18;
      buf[1] = 0x0;
      spiWriteAddr(REG_OCP, buf, sizeof(buf));
    }
    else
    {
      //SX1262
      printf("SX1262 CHIP SELECTED\n");
      setPaConfig(0x04, 0x07, 0x00, 0x01);
      if (22 > power < -9) {
        power = 22;
      }
      // Set max current to 140mA
      buf[0] = 0x38;
      buf[1] = 0x0;
      spiWriteAddr(REG_OCP, buf, sizeof(buf));
    }

    printf("LoRa power: %d\n", power);
    buf[0] = power;
    buf[1] = RADIO_RAMP_40_US;
    spiWriteCommand(RADIO_SET_TXPARAMS, buf, sizeof(buf));
}

void RH_SX126x::sleep(void)
{
    OperatingMode = MODE_SLEEP;
    SleepParams_t sleepConfig =
    {
        .Fields =
        {
            .WakeUpRTC = 0,
            .Reset     = 0,
            .WarmStart = 1,
        }
    };

    spiWriteCommand(RADIO_SET_SLEEP, &sleepConfig.Value, sizeof(sleepConfig));
    OperatingMode = MODE_SLEEP;
}

void RH_SX126x::setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask)
{
    uint8_t buf[8];

    buf[0] = (uint8_t)(( irqMask >> 8 ) & 0x00FF );
    buf[1] = (uint8_t)(irqMask & 0x00FF );
    buf[2] = (uint8_t)(( dio1Mask >> 8 ) & 0x00FF );
    buf[3] = (uint8_t)(dio1Mask & 0x00FF );
    buf[4] = (uint8_t)(( dio2Mask >> 8 ) & 0x00FF );
    buf[5] = (uint8_t)(dio2Mask & 0x00FF );
    buf[6] = (uint8_t)(( dio3Mask >> 8 ) & 0x00FF );
    buf[7] = (uint8_t)(dio3Mask & 0x00FF );

    spiWriteCommand(RADIO_CFG_DIOIRQ, buf, sizeof(buf));
}

void RH_SX126x::setPacketParams(PacketParams_t *packetParams)
{
    uint8_t buf[6] = {0x00};

    buf[0] = ( packetParams->Params.LoRa.PreambleLength >> 8 ) & 0xFF;
    buf[1] = packetParams->Params.LoRa.PreambleLength;
    buf[2] = packetParams->Params.LoRa.HeaderType;
    buf[3] = packetParams->Params.LoRa.PayloadLength;
    buf[4] = packetParams->Params.LoRa.CrcMode;
    buf[5] = packetParams->Params.LoRa.InvertIQ;

    spiWriteCommand(RADIO_SET_PACKETPARAMS, buf, sizeof(buf));
}

void RH_SX126x::setModulationParams(ModulationParams_t *modulationParams)
{
    uint8_t buf[4] = {0x00};

    buf[0] = modulationParams->Params.LoRa.SpreadingFactor;
    buf[1] = modulationParams->Params.LoRa.Bandwidth;
    buf[2] = modulationParams->Params.LoRa.CodingRate;
    buf[3] = modulationParams->Params.LoRa.LowDatarateOptimize;

    spiWriteCommand(RADIO_SET_MODULATIONPARAMS, buf, 4);
}

void RH_SX126x::enableTCXO(void)
{
    OperatingMode = MODE_STDBY_XOSC;

    RadioStandbyModes_t standbyConfig = STDBY_XOSC;
    spiWriteCommand(RADIO_SET_STANDBY, ( uint8_t* )&standbyConfig, 1);
}

int32_t RH_SX126x::frequencyError(void)
{
	return _packetStatus.Params.LoRa.FreqError;
}

int16_t RH_SX126x::lastRssi(void)
{
	return _packetStatus.Params.LoRa.RssiPkt;
}

void RH_SX126x::setCADTimeout(unsigned long cad_timeout)
{
    spiWriteCommand(RADIO_SET_CAD, NULL, 0);
}

void RH_SX126x::handleInterrupt(void)
{

    //printf("status irq = %02X\n", getStatus());
    //printf("error irq = %d\n", getDeviceErrors());

    //nrf_delay_ms(50);

    uint16_t irqRegs = getIrqStatus();
    clearIrqStatus(IRQ_RADIO_ALL);

    printf("irq = %d\n", irqRegs);

    if ((OperatingMode == MODE_TX) && ((irqRegs & IRQ_TX_DONE) == IRQ_TX_DONE))
    {
        //TimerStop(&TxTimeoutTimer);
	//!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
	//SX126xSetOperatingMode(MODE_STDBY_RC);
        //setModeIdle();
        OperatingMode = MODE_STDBY_RC;
        printf("Tx Done\n");
//	if ((RadioEvents != NULL) && (RadioEvents->TxDone != NULL))
//	{
//          RadioEvents->TxDone();
//	}
    }
    else if ((OperatingMode == MODE_RX) && ((irqRegs & IRQ_RX_DONE) == IRQ_RX_DONE))
    {

        uint8_t size;

        OperatingMode = MODE_STDBY_RC;

        //setModeIdle();

        if (_rxContinuous == false)
	{
          //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
          setModeIdle();

          // WORKAROUND - Implicit Header Mode Timeout Behavior, see DS_SX1261-2_V1.2 datasheet chapter 15.3
          // RegRtcControl = @address 0x0902
          //SX126xWriteRegister(0x0902, 0x00);
          uint8_t tsetsrs = 0x00;
          spiWriteAddr(0x0889, &tsetsrs, 1);
          // RegEventMask = @address 0x0944
          //SX126xWriteRegister(0x0944, SX126xReadRegister(0x0944) | (1 << 1));
          tsetsrs = (spiReadReg(0x0944) | (1 << 1));
          spiWriteAddr(0x0889, &tsetsrs, 1);
          // WORKAROUND END
        }

	memset(_rxPayload, 0, 255);

        if((irqRegs & IRQ_CRC_ERROR) != IRQ_CRC_ERROR)
        {
          getPayload(_rxPayload, &size, 255);
          getPacketStatus(&_packetStatus);

          printf("len %d, Rssi %d, Freq err %d\n", size, lastRssi(), frequencyError());

          validateRxBuf();

          //_rxBufValid = true;
        }


	/*// Have received a packet
	uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);
        printf("len %d ptr %02x\n",len,spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
	// Reset the fifo read ptr to the beginning of the packet
	spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

	spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
        _bufLen = len;
	spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

	// Remember the last signal to noise ratio, LORA mode
	// Per page 111, SX1276/77/78/79 datasheet
	_lastSNR = (int8_t)spiRead(RH_RF95_REG_19_PKT_SNR_VALUE) / 4;

	// Remember the RSSI of this packet, LORA mode
	// this is according to the doc, but is it really correct?
	// weakest receiveable signals are reported RSSI at about -66
	_lastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE);
	// Adjust the RSSI, datasheet page 87
	if (_lastSNR < 0)
	    _lastRssi = _lastRssi + _lastSNR;
	else
	    _lastRssi = (int)_lastRssi * 16 / 15;
	if (_usingHFport)
	    _lastRssi -= 157;
	else
	    _lastRssi -= 164;

        printf("len %d, Rssi %d, Freq err %d\n",len,_lastRssi,frequencyError());
	// We have received a message.
	validateRxBuf();
	if (0) //_rxBufValid)
        {
	    setModeIdle(); // Got one
            Lora_recieved_callback(_buf,len);
            clearRxBuf();
        }*/
    }


}

void RH_SX126x::setDio2AsRfSwitchCtrl(uint8_t enable)
{
    spiWriteCommand(RADIO_SET_RFSWITCHMODE, &enable, 1);
}

void RH_SX126x::setDio3AsTcxoCtrl( RadioTcxoCtrlVoltage_t tcxoVoltage, uint32_t timeout )
{
    uint8_t buf[4];

    buf[0] = tcxoVoltage & 0x07;
    buf[1] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( timeout & 0xFF );

    spiWriteCommand( RADIO_SET_TCXOMODE, buf, 4 );
}

uint16_t RH_SX126x::getIrqStatus(void)
{
    uint8_t irqStatus[2];

    spiReadCommand(RADIO_GET_IRQSTATUS, irqStatus, 2);
    return (irqStatus[0] << 8) | irqStatus[1];
}

void RH_SX126x::clearIrqStatus(uint16_t irq)
{
    uint8_t buf[2];

    buf[0] = (uint8_t)(((uint16_t)irq >> 8) & 0x00FF);
    buf[1] = (uint8_t)((uint16_t)irq & 0x00FF);
    spiWriteCommand(RADIO_CLR_IRQSTATUS, buf, 2);
}

uint8_t RH_SX126x::getStatus(void)
{
    uint8_t sysStatus[1];

    spiReadCommand(RADIO_GET_STATUS, sysStatus, 1);
    return sysStatus[0];
}

void RH_SX126x::setSyncWord(void)
{
    uint8_t sysStatus[2];

    sysStatus[0] = 0x55;
    sysStatus[1] = 0x55;

    spiWriteAddr(REG_LR_SYNCWORD, sysStatus, 2);
}

uint16_t RH_SX126x::getDeviceErrors(void)
{
    uint16_t error;

    spiReadCommand(RADIO_GET_ERROR, (uint8_t *)&error, 2);
    return error;
}

uint8_t RH_SX126x::getPayload( uint8_t *buffer, uint8_t *size,  uint8_t maxSize )
{
    uint8_t offset = 0;

    getRxBufferStatus( size, &offset );
    //*size = offset;
    if( *size > maxSize )
    {
        return 1;
    }

    _rxPayloadSize = *size;
    spiReadBuffer( offset, buffer, *size );
    return 0;
}

void RH_SX126x::getRxBufferStatus( uint8_t *payloadLength, uint8_t *rxStartBufferPointer )
{
    uint8_t status[2];

    spiReadCommand( RADIO_GET_RXBUFFERSTATUS, status, 2 );

    // In case of LORA fixed header, the payloadLength is obtained by reading
    // the register REG_LR_PAYLOADLENGTH
    if( ( _Mparams.PacketType == PACKET_TYPE_LORA ) && ( spiReadReg( REG_LR_PACKETPARAMS ) >> 7 == 1 ) && ( _Pparams.Params.LoRa.HeaderType ==  LORA_PACKET_IMPLICIT ) )
    {
        *payloadLength = spiReadReg( REG_LR_PAYLOADLENGTH );
    }
    else
    {
        *payloadLength = status[0];
    }
    *rxStartBufferPointer = status[1];
}

void RH_SX126x::getPacketStatus( PacketStatus_t *pktStatus )
{
    uint8_t status[3];

    spiReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

    pktStatus->packetType = _Mparams.PacketType;
    switch( pktStatus->packetType )
    {
        case PACKET_TYPE_GFSK:
            pktStatus->Params.Gfsk.RxStatus = status[0];
            pktStatus->Params.Gfsk.RssiSync = -status[1] / 2;
            pktStatus->Params.Gfsk.RssiAvg = -status[2] / 2;
            pktStatus->Params.Gfsk.FreqError = 0;
            break;

        case PACKET_TYPE_LORA:
            pktStatus->Params.LoRa.RssiPkt = -status[0] / 2;
            ( status[1] < 128 ) ? ( pktStatus->Params.LoRa.SnrPkt = status[1] / 4 ) : ( pktStatus->Params.LoRa.SnrPkt = ( ( status[1] - 256 ) /4 ) );
            pktStatus->Params.LoRa.SignalRssiPkt = -status[2] / 2;
            pktStatus->Params.LoRa.FreqError = 0;
            break;

        default:
        case PACKET_TYPE_NONE:
            // In that specific case, we set everything in the pktStatus to zeros
            // and reset the packet type accordingly
            memset( pktStatus, 0, sizeof( PacketStatus_t ) );
            pktStatus->packetType = PACKET_TYPE_NONE;
            break;
    }
}