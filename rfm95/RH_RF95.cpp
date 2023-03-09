// RH_RF95.cpp
//
// Copyright (C) 2011 Mike McCauley
// $Id: RH_RF95.cpp,v 1.18 2018/01/06 23:50:45 mikem Exp $

#include <RH_RF95.h>

void gpiote_lora_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
extern void Lora_recieved_callback(uint8_t* buffer, uint8_t len);
extern volatile bool spi_xfer_done; 

// Interrupt vectors for the 3 Arduino interrupt pins
// Each interrupt can be handled by a different instance of RH_RF95, allowing you to have
// 2 or more LORAs per Arduino
//RH_RF95* RH_RF95::_deviceForInterrupt[RH_RF95_NUM_INTERRUPTS] = {0, 0, 0};
//uint8_t RH_RF95::_interruptCount = 0; // Index into _deviceForInterrupt for next device

// These are indexed by the values of ModemConfigChoice
// Stored in flash (program) memory to save SRAM
static const RH_RF95::ModemConfig MODEM_CONFIG_TABLE[] =
{
    //  1d,     1e,      26
    { 0x72,   0x74,    0x04}, // Bw125Cr45Sf128 (the chip default), AGC enabled
    { 0x92,   0x74,    0x04}, // Bw500Cr45Sf128, AGC enabled
    { 0x48,   0x94,    0x04}, // Bw31_25Cr48Sf512, AGC enabled
    { 0x78,   0xb4,    0x04}, // Bw125Cr48Sf2048, AGC enabled
    { 0x78,   0xc4,    0x0c}, // Bw125Cr48Sf4096, AGC enabled
    
};

RH_RF95::RH_RF95()
{

}

uint8_t RH_RF95::get_irq_flag()
{
    return spiRead(RH_RF95_REG_12_IRQ_FLAGS);
}
uint8_t RH_RF95::get_Mode()
{
    return spiRead(RH_RF95_REG_01_OP_MODE);
}

uint8_t RH_RF95::modem_Stat()
{
    return spiRead(RH_RF95_REG_18_MODEM_STAT);
}

bool RH_RF95::init(const nrf_drv_spi_t *spi_in, uint32_t pin)
{
     uint8_t mode;

     _spi = spi_in;
     _interruptPin = pin;
     _thisAddress = 0x7f;
     _promiscuous = true;
    _txHeaderTo = 0x7f;
    _txHeaderFrom = 0xff;
    _txHeaderId = 0xff;
    _txHeaderFlags = 0;
    _cad_timeout = 0;

    // Set sleep mode, so we can also set LORA mode:
    if (spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE) != NRF_SUCCESS)
    {
      printf("1st spi write failed, did not put chip into sleep and LORA mode\n");
      return false;
    }
    nrf_delay_ms(10); // Wait for sleep mode to take over from say, CAD
    // Check we are in sleep mode, with LORA set
    mode = spiRead(RH_RF95_REG_01_OP_MODE);
    printf("-----------LoRa Mode: %d\n", mode);
    //printf("Mode 0x80 %02x\n",mode);
    if ((mode & RH_RF95_LONG_RANGE_MODE) != RH_RF95_LONG_RANGE_MODE)
    {
	printf("Error: didn't go into Lora mode\n");
	return false;
    }
    if ((mode & RH_RF95_MODE_STDBY) != RH_RF95_MODE_STDBY)
        printf("Lora in sleep mode\n");
    else
        printf("Lora in Standby\n");

    //On Opal, DIO0 came up as '1' once. This means irq flag was set at poweron
    //I clear it here just in case.
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
 
    nrfx_gpiote_in_config_t config;                                               
    config.is_watcher = false;                        
    config.hi_accuracy = true; 
    config.skip_gpio_setup = false;
    config.pull = NRF_GPIO_PIN_NOPULL;                
    config.sense = NRF_GPIOTE_POLARITY_LOTOHI;        

    if (nrfx_gpiote_in_init(_interruptPin, &config, gpiote_lora_evt_handler) != NRFX_SUCCESS)
      printf("gpiote in init failed for Lora SPI\n");
    nrfx_gpiote_in_event_enable(_interruptPin,true);
 
    // Set up 256 byte FIFO
    // We configure so that we can use the entire 256 byte FIFO for either receive
    // or transmit, but not both at the same time
    if (spiWrite(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR, 0) != NRF_SUCCESS)
      printf("Set TX address to 0 failed\n");
    if (spiWrite(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR, 0) != NRF_SUCCESS)
      printf("Set Rx adress to 0 failed\n");

    // Packet format is preamble + explicit-header + payload + crc
    // Explicit Header Mode
    // payload is TO + FROM + ID + FLAGS + message data
    // RX mode is implmented with RXCONTINUOUS
    // max message data length is 255 - 4 = 251 octets

    //Idle mode is standby, not sleep
    setModeIdle();

    // Set up default configuration
    // No Sync Words in LORA mode.
    setModemConfig(Bw125Cr45Sf128); // Radio default
    /*if (spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1, 0x82) != NRF_SUCCESS)
      printf("Set modem config 1 failed\n");
    if (spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2, 0xA4) != NRF_SUCCESS)
      printf("Set modem config 2 failed\n");
    if (spiWrite(RH_RF95_REG_26_MODEM_CONFIG3, 0x04) != NRF_SUCCESS)
      printf("Set modem config 3 failed\n");
    if (spiWrite(RH_RF95_REG_0C_LNA, 0x23) != NRF_SUCCESS)
      printf("Set modem config 3 failed\n");
    if (spiWrite(RH_RF95_REG_61_AGC_REF, 0x19) != NRF_SUCCESS)
      printf("Set modem config 3 failed\n");
    if (spiWrite(RH_RF95_REG_62_AGC_THRESH1, 0x0C) != NRF_SUCCESS)
      printf("Set modem config 3 failed\n");
    if (spiWrite(RH_RF95_REG_63_AGC_THRESH2, 0x4B) != NRF_SUCCESS)
      printf("Set modem config 3 failed\n");*/
        //setModemConfig(Bw500Cr45Sf128); 
    setPreambleLength(8); // Default is 8
    // An innocuous ISM frequency, same as RF22's
#if EMERALD == 1
    setFrequency(915.0);
    //setFrequency(915.0);
#else
    setFrequency(924);
    //printf("Set frequency to 915Mhz\n");
#endif
    // Tx Power level
#if EMERALD == 1 || OPAL == 1
    setTxPower(23,false);           //use PA_BOOST
    printf("Tx power 20 PA_BOOST\n");
#else
    setTxPower(23,false);        //module, use PA_BOOST
    printf("Initial Tx power: 23\n");
#endif      
      
   // if (spiWrite(RH_RF95_REG_0C_LNA, 0x23) != NRF_SUCCESS)
     // printf("Set to 0x23 failed\n");
    return true;
}


// C++ level interrupt handler for this instance
// LORA is unusual in that it has several interrupt lines, and not a single, combined one.
// On MiniWirelessLoRa, only one of the several interrupt lines (DI0) from the RFM95 is usefuly 
// connnected to the processor.
// We use this to get RxDone and TxDone interrupts
void RH_RF95::handleInterrupt()
{
    //printf("inside hanling interrupts\n");
    uint8_t i;
    // Read the interrupt register
    uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
    // Read the RegHopChannel register to check if CRC presence is signalled
    // in the header. If not it might be a stray (noise) packet.*
    uint8_t crc_present = spiRead(RH_RF95_REG_1C_HOP_CHANNEL);

    if (_mode == RHModeRx
	&& ((irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
	    | !(crc_present & RH_RF95_RX_PAYLOAD_CRC_IS_ON)))
    //    if (_mode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
    {
        printf("Receive error %02x\n",irq_flags);
	_rxBad++;
    }
    else if (_mode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
    {
	// Have received a packet
	uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);
        #ifdef test_print
        printf("len %d ptr %02x\n",len,spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
        #endif //test_print
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
	 
        //printf("len %d, Rssi %d, Freq err %d\n",len,_lastRssi,frequencyError());
	// We have received a message.
	validateRxBuf(); 
	if (0) //_rxBufValid)
        {
	    setModeIdle(); // Got one
            Lora_recieved_callback(_buf,len);
            clearRxBuf();
        }
    }
    else if (_mode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
    {
	_txGood++;
        printf("Tx Done\n");
	setModeIdle();
    }
    else if (_mode == RHModeCad && irq_flags & RH_RF95_CAD_DONE)
    {
        _cad = irq_flags & RH_RF95_CAD_DETECTED;
        setModeIdle();
    }
    // Sigh: on some processors, for some unknown reason, doing this only once does not actually
    // clear the radio's interrupt flag. So we do it twice. Why?
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
}

// Check whether the latest received message is complete and uncorrupted
#define RH_BROADCAST_ADDRESS 0xff
void RH_RF95::validateRxBuf()
{
    if (_bufLen < 4){
    printf("too short\n");
	return; // Too short to be a real message
      }
    // Extract the 4 headers
    _rxHeaderTo    = _buf[0];
    _rxHeaderFrom  = _buf[1];
    _rxHeaderId    = _buf[2];
    _rxHeaderFlags = _buf[3];
    if (_promiscuous ||
	_rxHeaderTo == _thisAddress ||
	_rxHeaderTo == RH_BROADCAST_ADDRESS)
    {
	_rxGood++;
	_rxBufValid = true;
    }
}

bool RH_RF95::available()
{
    uint8_t mode;
//    printf("_mode: %d, RHModeTx = %d\n", _mode, RHModeTx);
//    nrf_delay_ms(500);
    if (_mode == RHModeTx)
	return false;
    setModeRx();
    return _rxBufValid; // Will be set by the interrupt handler when a good message is received
}

void RH_RF95::clearRxBuf()
{
     _rxBufValid = false;
    _bufLen = 0;
}

bool RH_RF95::recv(uint8_t* buf, uint8_t* len)
{

//printf("inside rcv function\n");
    uint16_t i;

    if (!available())
	return false;
    if (buf && len)
    {

	// Skip the 4 headers that are at the beginning of the rxBuf
	//if (*len > _bufLen-RH_RF95_HEADER_LEN) {
          *len = _bufLen-RH_RF95_HEADER_LEN;
        //} else {
        //  *len = _bufLen;
        //}
	memcpy(buf, _buf+RH_RF95_HEADER_LEN, *len); 
  
//        for(i=0;(i<*len);i++)
//            printf("%c",buf[i]);
//        printf("\n");

    }
    clearRxBuf(); // This message accepted and cleared
    return true;
}

bool RH_RF95::waitPacketSent()
{
    while (_mode == RHModeTx)
	; // Wait for any previous transmit to finish
    return true;
}

bool RH_RF95::checkPacketSent()
{
    if (_mode != RHModeTx)
      return true;
    else
      return false;
}

bool RH_RF95::waitCAD()
{
    if (!_cad_timeout)
	return true;

    // Wait for any channel activity to finish or timeout
    // Sophisticated DCF function...
    // DCF : BackoffTime = random() x aSlotTime
    // 100 - 1000 ms
    // 10 sec timeout
    nrf_delay_ms(10);
    return true;
}

bool RH_RF95::send(const uint8_t* data, uint8_t len)
{
    if (len > RH_RF95_MAX_MESSAGE_LEN)
	return false;
    waitPacketSent(); // Make sure we dont interrupt an outgoing message
    setModeIdle();
    //if (!waitCAD()) 
	//return false;  // Check channel activity

    // Position at the beginning of the FIFO
    spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);
    // The headers
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderTo);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderFrom);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderId);
    spiWrite(RH_RF95_REG_00_FIFO, _txHeaderFlags);
    // The message data
    spiBurstWrite(RH_RF95_REG_00_FIFO, data, len);
    spiWrite(RH_RF95_REG_22_PAYLOAD_LENGTH, len + RH_RF95_HEADER_LEN);
    setModeTx(); // Start the transmitter
    // when Tx is done, interruptHandler will fire and radio mode will return to STANDBY
    return true;
}

bool RH_RF95::printRegisters()
{
#ifdef RH_HAVE_SERIAL
    uint8_t registers[] = { 0x01, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x014, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

    uint8_t i;
    for (i = 0; i < sizeof(registers); i++)
    {
	Serial.print(registers[i], HEX);
	Serial.print(": ");
	Serial.println(spiRead(registers[i]), HEX);
    }
#endif
    return true;
}

uint8_t RH_RF95::maxMessageLength()
{
    return RH_RF95_MAX_MESSAGE_LEN;
}

bool RH_RF95::setFrequency(float centre)
{
    // Frf = FRF / FSTEP
    uint32_t frf = (centre * 1000000.0) / RH_RF95_FSTEP;
    if (spiWrite(RH_RF95_REG_06_FRF_MSB, (frf >> 16) & 0xff) != NRF_SUCCESS)
       printf("freq 1 fail\n");
    if (spiWrite(RH_RF95_REG_07_FRF_MID, (frf >> 8) & 0xff) != NRF_SUCCESS)
       printf("freq 2 fail\n");
    if (spiWrite(RH_RF95_REG_08_FRF_LSB, frf & 0xff) != NRF_SUCCESS)
       printf("freq 3 fail\n");
    _usingHFport = (centre >= 779.0);

    printf("Lora frequency set to %f\n",centre);
    return true;
}

void RH_RF95::setModeIdle()
{
    if (_mode != RHModeIdle)
    {
	if (spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_STDBY) != NRF_SUCCESS)
          printf("SPI standby fail\n");
	_mode = RHModeIdle;
    }
}

bool RH_RF95::sleep()
{
    if (_mode != RHModeSleep)
    {
	spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP);
	_mode = RHModeSleep;
    }
    return true;
}

void RH_RF95::setModeRx()
{
    if (_mode != RHModeRx)
    {
	if (spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_RXCONTINUOUS) != NRF_SUCCESS)
          printf("Failed to start continuous read\n");
	if (spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x00) != NRF_SUCCESS) // Interrupt on RxDone
          printf("Failed to set mapping\n"); 
        _mode = RHModeRx;
    }
}

void RH_RF95::setModeTx()
{
    uint8_t dio_reg;
    if (_mode != RHModeTx)
    {
	spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_TX);
	if (spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x40) != NRF_SUCCESS) // Interrupt on TxDone
          printf("Failed to set Tx Done DIO0 mapping\n");
        _mode = RHModeTx;
    }
}

void RH_RF95::setTxPower(int8_t power, bool useRFO)
{
    // Sigh, different behaviours depending on whther the module use PA_BOOST or the RFO pin
    // for the transmitter output
    if (useRFO)
    {
	if (power > 14)
	    power = 14;
	if (power < -1)
	    power = -1;
	spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_MAX_POWER | (power + 1));
    }
    else
    {
	if (power > 23)
	    power = 23;
	if (power < 5)
	    power = 5;

	// For RH_RF95_PA_DAC_ENABLE, manual says '+20dBm on PA_BOOST when OutputPower=0xf'
	// RH_RF95_PA_DAC_ENABLE actually adds about 3dBm to all power levels. We will us it
	// for 21, 22 and 23dBm
	if (power > 20)
	{
	    spiWrite(RH_RF95_REG_4D_PA_DAC, 0x87);
	    power -= 3;
	}
	else
	{
	    spiWrite(RH_RF95_REG_4D_PA_DAC, RH_RF95_PA_DAC_DISABLE);
	}

	// RFM95/96/97/98 does not have RFO pins connected to anything. Only PA_BOOST
	// pin is connected, so must use PA_BOOST
	// Pout = 2 + OutputPower.
	// The documentation is pretty confusing on this topic: PaSelect says the max power is 20dBm,
	// but OutputPower claims it would be 17dBm.
	// My measurements show 20dBm is correct
	spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_PA_SELECT | (power-5));
        spiWrite(RH_RF95_REG_0B_OCP,0xFF);
    }
}

// Sets registers from a canned modem configuration structure
void RH_RF95::setModemRegisters(const ModemConfig* config)
{
    if (spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1,config->reg_1d) != NRF_SUCCESS)
      printf("modem 1 fail\n");
    if (spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2,config->reg_1e) != NRF_SUCCESS)
      printf("modem 2 fail\n");
    if (spiWrite(RH_RF95_REG_26_MODEM_CONFIG3,config->reg_26) != NRF_SUCCESS)
      printf("modem 3 fail\n");
}

// Set one of the canned FSK Modem configs
// Returns true if its a valid choice
bool RH_RF95::setModemConfig(ModemConfigChoice index)
{
    if (index > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
        return false;

    ModemConfig cfg;
    memcpy(&cfg, &MODEM_CONFIG_TABLE[index], sizeof(RH_RF95::ModemConfig));
    setModemRegisters(&cfg);

    return true;
}

void RH_RF95::setPreambleLength(uint16_t bytes)
{
    spiWrite(RH_RF95_REG_20_PREAMBLE_MSB, bytes >> 8);
    spiWrite(RH_RF95_REG_21_PREAMBLE_LSB, bytes & 0xff);
}

bool RH_RF95::isChannelActive()
{
    // Set mode RHModeCad
    if (_mode != RHModeCad)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_CAD);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x80); // Interrupt on CadDone
        _mode = RHModeCad;
    }

    while (_mode == RHModeCad) ;

    return _cad;
}

void RH_RF95::enableTCXO()
{
    while ((spiRead(RH_RF95_REG_4B_TCXO) & RH_RF95_TCXO_TCXO_INPUT_ON) != RH_RF95_TCXO_TCXO_INPUT_ON)
    {
	sleep();
	spiWrite(RH_RF95_REG_4B_TCXO, (spiRead(RH_RF95_REG_4B_TCXO) | RH_RF95_TCXO_TCXO_INPUT_ON));
    } 
}

// From section 4.1.5 of SX1276/77/78/79
// Ferror = FreqError * 2**24 * BW / Fxtal / 500
int32_t RH_RF95::frequencyError()
{
    int32_t freqerror = 0;

    // Convert 2.5 bytes (5 nibbles, 20 bits) to 32 bit signed int
    // Caution: some C compilers make errors with eg:
    // freqerror = spiRead(RH_RF95_REG_28_FEI_MSB) << 16
    // so we go more carefully.
    freqerror = spiRead(RH_RF95_REG_28_FEI_MSB);
    freqerror <<= 8;
    freqerror |= spiRead(RH_RF95_REG_29_FEI_MID);
    freqerror <<= 8;
    freqerror |= spiRead(RH_RF95_REG_2A_FEI_LSB);
    // Sign extension into top 3 nibbles
    if (freqerror & 0x80000)
	freqerror |= 0xfff00000;

    int32_t error = 0; // In hertz
    float bw_tab[] = {7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250, 500};
    uint8_t bwindex = spiRead(RH_RF95_REG_1D_MODEM_CONFIG1) >> 4;
    if (bwindex < (sizeof(bw_tab) / sizeof(float)))
	error = (float)freqerror * bw_tab[bwindex] * ((float)(1L << 24) / (float)RH_RF95_FXOSC / 500.0);
    // else not defined

    return error;
}

int16_t RH_RF95::lastSNR()
{
    return _lastSNR;
}

int16_t RH_RF95::lastRssi()
{
    return _lastRssi;
}

uint8_t RH_RF95::spiRead(uint8_t reg)
{
   uint32_t result;
   uint8_t tx_byte[2], rx_byte[2];

   tx_byte[0] = reg;
   tx_byte[1] = 0;
   spi_xfer_done = false;
   result = nrf_drv_spi_transfer(_spi, tx_byte, 2, rx_byte, 2);
   if (result != NRF_SUCCESS)
      return 0;

   while(spi_xfer_done == false) ;

   return rx_byte[1];
}

//not used but here just in case
uint8_t RH_RF95::reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

uint32_t RH_RF95::spiWrite(uint8_t reg, uint8_t val)
{
//printf("spi write\n");
   uint32_t result;
   uint8_t tx_byte[2], rx_byte[2];

   tx_byte[0] = reg | 0x80;
   tx_byte[1] = val;
   spi_xfer_done = false;
   result = nrf_drv_spi_transfer(_spi, tx_byte, 2, rx_byte, 2);
   if (result != NRF_SUCCESS)
      return result;
   while(spi_xfer_done == false) ;
   return result;
}

uint8_t RH_RF95::spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
   uint32_t result;
   static uint8_t tx_byte[255], rx_byte[255];

   tx_byte[0] = reg;
   spi_xfer_done = false;
   ++len;
   result = nrf_drv_spi_transfer(_spi, tx_byte, len, rx_byte, len);
   if (result != NRF_SUCCESS)
      return 0;
   while(spi_xfer_done == false) ;
   memcpy(dest,&rx_byte[1],len);

   return len;
}

uint8_t RH_RF95::spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
  uint32_t result;
  uint8_t tx_byte[255], rx_byte[255];

  tx_byte[0] = reg | 0x80;
  memcpy(&tx_byte[1],src,len);
  spi_xfer_done = false;
  result = nrf_drv_spi_transfer(_spi, tx_byte, len+1, rx_byte, len+1);
  if (result != NRF_SUCCESS)
    return 0;

  while(spi_xfer_done == false) ;
  return len;
}

uint8_t RH_RF95::mode()
{
    return _mode;
}

void RH_RF95::setMode(RHMode mode)
{
    _mode = mode;
}

uint16_t RH_RF95::rxBad()
{
    return _rxBad;
}

uint16_t RH_RF95::rxGood()
{
    return _rxGood;
}

uint16_t RH_RF95::txGood()
{
    return _txGood;
}

void RH_RF95::setCADTimeout(unsigned long cad_timeout)
{
    _cad_timeout = cad_timeout;
}
