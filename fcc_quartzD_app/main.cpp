#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "nfc_t2t_lib.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_uart.h"
#include "app_fifo.h"
#include "nrf_error.h"
#include "nrfx_twim.h"
#include "nrf_drv_spi.h"
#include "nrfx_rtc.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_soc.h"
#include "app_timer.h"
#include "fds.h"
#include "ble.h"
#include "nrf_sdm.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_advdata.h"
#include "ble_nus.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_conn_params.h"
#include "nrf_ble_qwr.h"
#include "nrf_ble_gatt.h"
#include "ble_advertising.h"
#include "peer_manager.h"
#include "nrfx_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf_nvmc.h"
#include "nrf_uarte.h"
#include "arm_const_structs.h"

#include "boards.h"

#define UD_SER
#define CUT
#define PAR_CONFIG
//#define RESET
#define SAS //Scan,Adv,Sleep
//#define SLEEP

#define CLOCK_RESOULATION__ms     (1)

APP_TIMER_DEF(timer_id); //create a timer id
APP_TIMER_DEF(clock_id);

uint32_t millis_counter = 0;

void init_timer2(void);
void timer2_event_handler(void* p_context);

void gpiote_lora_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

//Set this to 1 for scanner/advertise relay, 0 is normal cellular upload
#define ADV_SCANNER_FW 0
//Set this to 1 to disable TCA9535 code for green tape
#define TINY_RELAY 0

static char ackMessage[2] = "A";

//BLE SCANNING
int ADV_W = 250;     // uint millisecond
int8_t txlevel = 7;
int TX_POWER[9] = {-40,-30,-20,-16,-12,-8,-4,0,4};

uint8_t Ble_Scn_SIG[2] = {0x59, 0x00};
uint8_t Ble_Scn_SIG_ext[2] = {0x59, 0x00};
uint8_t Ble_Scn_Tab[2] = {0x0D, 0x18};
uint8_t Ble_Scn_UUID[2] = {0x49, 0x54}; // hex of IT(Infrastructure Tape)
uint8_t Ble_Scn_UUID_DEL[2] = {0xDE, 0x1E}; // hex of IT(Infrastructure Tape)
uint8_t Ble_Scn_OTA[2] = {0xDF, 0x00};
uint8_t Ble_Scn_UUID_out[2] = {0xCA, 0xF0};

enum EIR
{
    L0,
    T0,
    V0,
    L1,
    T1,
    SIGH,
    SIGL,
    U0,
    U1,
    U2,
    U3,
    U4,
    U5,
    U6,
    U7,
    U8,
    U9,
    U10,
    U11,
    U12,
    U13,
    U14,
    U15,
}; // BLE EIR Enum

bool clearFlag = true;
uint8_t records = 0;
bool TapeConfig = true;
bool BLE_GW = false;
bool Drop_Flag = false;
bool B_gps = false;
bool B_lora = false;
bool B_cell = false;
bool Tape_Type = false;
uint8_t Accl_TH = 1;
bool connected;
float hrsbuff[20];
bool cut_flag = false;
bool reset_flag = false;
bool debugMode = false;
bool findtapeFlag = false;

#ifdef UD_SER
#define APP_BLE_CONN_CFG_TAG             1                                          /**< A tag identifying the SoftDevice BLE configuration. */
#define DEVICE_NAME                      "CONFIG_DUT"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE            BLE_UUID_TYPE_VENDOR_BEGIN                 /**< UUID type for the Nordic UART Service (vendor specific). */
#define APP_BLE_OBSERVER_PRIO            3                                          /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_ADV_INTERVAL                 64                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_DURATION                 18000                                      /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define MIN_CONN_INTERVAL                MSEC_TO_UNITS(20, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL                MSEC_TO_UNITS(75, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                    0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                 MSEC_TO_UNITS(4000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                      /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000)                     /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     3                                          /**< Number of attempts before giving up the connection parameter negotiation. */
#define DEAD_BEEF                        0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define UART_TX_BUF_SIZE                 256                                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                 256                                        /**< UART RX buffer size. */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */
static uint16_t     m_conn_handle          = BLE_CONN_HANDLE_INVALID;               /**< Handle of the current connection. */
static uint16_t     m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;          /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

static ble_uuid_t m_adv_uuids[]            =                                        /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};
#endif //UD_SER

#define APP_BLE_OBSERVER_PRIO 3                                                     //Application's BLE observer priority. You shouldn't need to modify this value.

uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
uint16_t length          = 4;//MAX LENGTH is 20
int SCAN_WINDOW         = 100;
int SCAN_INTERVAL       = SCAN_WINDOW+100;
int SCAN_DURATION       = 0x0000;
int configDuration      = 5000;                                                          //Timout when scanning. 0x0000 disables timeout.
int ackTime             = 5000;
int configTime          = 5000;
//------------------------------------------------------
// Multi Sleep time setup
//------------------------------------------------------
int relayTime[4]       = {2000,4000,8000,16000};
int advTime            = 1000;
int sleepTime          = 6000;
int scanTime[3]        = { 750, 500, 1000 };
int scanTimeVer        = 0;
int scanDuration       = 1000;
int timeVer            = 1;
//------------------------------------------------------
static int battTime = 900000;
static uint16_t waitTH = 3;
static ble_gap_scan_params_t const m_scan_params =
{
    .active = 1,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .scan_phys = BLE_GAP_PHY_1MBPS,
    .interval = (uint16_t)SCAN_INTERVAL,
    .window = (uint16_t)SCAN_WINDOW,
    .timeout = (uint16_t)SCAN_DURATION
};

static uint8_t m_scan_buffer_data[BLE_GAP_SCAN_BUFFER_MIN]; /**< buffer where advertising reports will be stored by the SoftDevice. */

static ble_data_t m_scan_buffer =
{
    m_scan_buffer_data,
    BLE_GAP_SCAN_BUFFER_MIN
};
void ble_scanner_init(void);
void scan_start(void);
void on_adv_report(ble_gap_evt_adv_report_t const *p_adv_report);
void ble_scanner_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
void ble_scanner_stack_init(void);
void init_scanning_data(void);
NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_scanner_evt_handler, NULL);

#define NON_CONNECTABLE_ADV_INTERVAL MSEC_TO_UNITS(ADV_W, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#define APP_COMPANY_IDENTIFIER 0x0059                                                                    /**< Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define APP_COMPANY_IDENTIFIER_EXT 0x0059                                                            /**< Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define MAIN_TAPE_ID_0 0x09
#define MAIN_TAPE_ID_1 0xFA
#define APP_BEACON_INFO_LENGTH 0x18

//BLE ADVERTISING
static ble_gap_adv_params_t m_adv_params;                                         /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET; /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];    /**< Buffer for storing an encoded advertising set. */
void ble_adv_init(void);
uint8_t *adv_address, *adv_data;
uint16_t adv_len = 0;
uint16_t data_len = 31;
int8_t rssi;
#define NUMBER_UNIQUE_ADDR 5
struct adv_seen
{
    uint8_t addr[6];
    int8_t rssi;
    int8_t temp;
    int8_t accel;
    uint8_t sorted;
};
struct adv_sent {
    uint8_t addr[2];
    struct adv_seen peers[NUMBER_UNIQUE_ADDR];
} main_tape;

static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = NULL,
        .len = 0
    },
};

uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] = {0x49, 0x54};

#define NUMBER_OF_TAGS 1
#define NUMBER_OF_LOGS 4
#define SLEEP_TIME_SEC 60
#define MAX_ID_BUF_LENGTH 15
#define TIME_STRING_LENGTH 25

int32_t volatile temp;
struct tagRec
{
    uint32_t subID;
    uint8_t addr[MAX_ID_BUF_LENGTH];
    int8_t rssi;
    int Lorarssi;
    uint16_t accTrigCount;
    int tempRec;
    float subBat;
    uint16_t counter;
    float latt;
    float lont;
    //uint32_t currentTime;
    uint16_t millisTime;
};
typedef struct logRec
{
    uint16_t logNo;
    float clat;
    float clon;
    float glat;
    float glon;
    int numberOfScanned;
    char timeStr[TIME_STRING_LENGTH];
    char relayId[MAX_ID_BUF_LENGTH];
    float relayBat;
    tagRec scanLog[NUMBER_OF_TAGS];
} logRec_t;
logRec_t TapeLog[NUMBER_OF_LOGS];

//AK25 struct tagRec tags[NUMBER_OF_LOGS][NUMBER_OF_TAGS];
static char idBuf[MAX_ID_BUF_LENGTH];
static char idString[MAX_ID_BUF_LENGTH];

//const nrf_drv_timer_t WAKEUP_TIMER = NRF_DRV_TIMER_INSTANCE(0);
static uint32_t sleep_time_ms;

//Global variables
int comm_freq;
float lat = 0.0;
float lon = 0.0;
int rtcCountTimeout = 0;
int timeroutPeriod = SLEEP_TIME_SEC;
int16_t adcResult[5] = {0};
float prescise_result = 0;
volatile int timerFlag = 0;
int globalSendCounter = 0;
int logCounterf = 0;
int logCountert = 0;
int timeoutFlagb = 0;
int timeoutFlagc = 0;
int sleepflag = 0;
//AK int noSendCounter = 1;
int noSendCounter = 0;
int last_noSendCounter = 0;
int tempperiod = 1;
int last_tempperiod;
uint32_t bestSubID;
int bestLorarssi;
bool sendFlag = false;
uint32_t r = 0;
int LoggingCount = 0;
int NumberOfLogsPerSend = 0;
bool setupFlag = true;
bool gprsSetupFlag = true;
bool OTA = false;
uint8_t MODE = 0;
uint16_t batteryV = 0;
int planeEventCounter = 0;
uint32_t millis_init = 0;
uint32_t appUnixtime = 1564951366;
uint32_t tapeUnixtime = 1652;
int8_t rssiTH = -100;
int8_t rssi_threshold = -70;
int ble_scan_duration = 10000;
int ble_adv_duration = 3000;
int gps_scan_duration = 30000;
bool ota_flag = false;

//airplane shutoff var
int16_t acc_x, acc_y, acc_z;
uint8_t acc_threshold_H = 238; // thresold value for acc (0.212/0.00781)
uint8_t acc_threshold_L = 225; // thresold value for acc (0.212/0.00781)
uint8_t acc_threshold = 254;     // thresold value for acc (0.212/0.00781)
uint8_t acc_th_duration = 254; // duration in millisecond
int acc_sum_threshold = 511;
bool shutoffFlag_a = false;
bool shutonFlag_a = false;
bool shutoffFlag_p = false;
bool shutonFlag_p = false;
bool airplane_landed = false;
bool in_airplane = false;
int lastPressure = 0;
int pressureGradientThreshold_T = -60000; //-35
int pressureGradientThreshold_L = 60000;    //50
int sum = 0;
int accelThreshold = 3;
int time_c = 0;
int time_p = 0;
bool mode_flag = true;
uint8_t bno_initialized = 0;

bool oxy_scanned = false;
bool low_voltage = false;
#define RADIO_LENGTH_LENGTH_FIELD     (8UL) /**< Length on air of the LENGTH field. */
#define RADIO_MAX_PAYLOAD_LEN         256     /**< Maximum radio RX or TX payload. */
uint8_t m_tx_packet[RADIO_MAX_PAYLOAD_LEN];                                                                             /**< Buffer for the radio TX packet. */
uint8_t    mode                       = RADIO_MODE_MODE_Ble_1Mbit;    /**< Radio mode. Data rate and modulation. */
uint8_t    txpower                    = RADIO_TXPOWER_TXPOWER_0dBm; /**< Radio output power. */
uint8_t    channel                    = 40;


void check_mode();
void mode_check();
void shutoffChecker();
void shutonChecker();

#define adcToBat(x) ((x * 0.6f / 1024.0f) * 2.0f * 6.0f)

//function prototypes
void ud_advertising_start(bool erase_bonds);
void application_timers_start(void);
void application_timers_stop(void);
void advertising_init(void);
void advertising_start(void);
void zeroTags(int tagy);
bool checkZeros(void);
void wdt_init(int seconds);
void wdt_reset();
void init_timer();
void start_timer(int miliTimeout);
void timer_event_handler(void *p_context);
void stop_timer();
void systemSleep();
void setupSensors();
void random_pwr_fix();
void callmainwithbyte(uint8_t byte);
void get_ble_mac(void);
int checkIdInLog(uint8_t inArray[], int testlog);
int compareArrays(uint8_t a[], uint8_t b[], int n);
bool uploadData();
void bleScan(int timeOutMillis);
void bleAdv(int timeOutMillis);
void wakeFromSleep();
void goToSleep();
void goToSleepAirSense();
void createTimeString();
void shutoffChecker();
void clearBit(int n);
int checkBit(int n);
void setBit(int n);
void setVerBits(int V);
void batteryCheck();

uint8_t *hex_decode(char *in, size_t len, uint8_t *out);
void prepare_sleep();
void prepare_wake();
void radio_disable(void);
void radio_config();
void sel_txpower(int tx_level);
void radio_with_data(bool flag);
void modulation(void);
void tapeDiagnosis(void);


uint32_t millis(void)
{
    return millis_counter;
}

void timer2_event_handler(void* p_context)
{
    (void)p_context;
    millis_counter++;
}

/**@brief Handler for shutdown preparation.
 */
bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    uint32_t err_code;

    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
            printf("NRF_PWR_MGMT_EVT_PREPARE_SYSOFF");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            printf("NRF_PWR_MGMT_EVT_PREPARE_WAKEUP");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            printf("Entering DFU, you may have to fix this code");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_RESET:
            printf("NRF_PWR_MGMT_EVT_PREPARE_RESET");
            break;
    }

    err_code = app_timer_stop_all();
    APP_ERROR_CHECK(err_code);

    return true;
}
/**@brief Register application shutdown handler with priority 0. */
NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, 0);

void on_adv_report(ble_gap_evt_adv_report_t const *p_adv_report)
{
    volatile uint8_t i, j, k;
    int status = 0;
    adv_address = (uint8_t *)p_adv_report->peer_addr.addr;
    adv_data = (uint8_t *)p_adv_report->data.p_data;
    adv_len = (uint16_t)p_adv_report->data.len;
    //printf("\nFROM: %02x:%02x:%02x:%02x:%02x:%02x %d\n", adv_address[0], adv_address[1], adv_address[2], adv_address[3], adv_address[4], adv_address[5], p_adv_report->rssi);

    if(!debugMode)
    {
        if (adv_data[SIGH] == Ble_Scn_SIG_ext[0] && adv_data[SIGL] == Ble_Scn_SIG_ext[1])
        {
            if (adv_data[7] == Ble_Scn_UUID[0] && adv_data[8] == Ble_Scn_UUID[1] && adv_len == data_len)
            {
                status = -1;

                if (status == -1)
                {
                    clearFlag = false;
                    printf("\nFROM: %02x:%02x:%02x:%02x:%02x:%02x %d\n", adv_address[0], adv_address[1], adv_address[2], adv_address[3], adv_address[4], adv_address[5], p_adv_report->rssi);
                    //Copy Address
                    for (int i = 5; i >= 0; i--)
                    {
                        TapeLog[0].scanLog[records].addr[i] = adv_address[i];
                    }

                    setBit(records);
                    TapeLog[0].scanLog[records].rssi = adv_data[9];

                    printf("Rssi:%d\n",TapeLog[0].scanLog[records].rssi);
                    //Copy Battery
                    TapeLog[0].scanLog[records].subBat = (float)(adv_data[10]);;
                    printf("BATT:%f\n",TapeLog[0].scanLog[records].subBat);

                    records = (records + 1) % NUMBER_OF_TAGS;
                }
                else
                {
                    setBit(status);
                    for(int j=records; j<NUMBER_OF_TAGS;j++)
                    {
                        TapeLog[0].scanLog[j].counter = 0;
                    }
                }
            }
        }
    }
    else
    {
        if (adv_data[7] == Ble_Scn_UUID[0] && adv_data[8] == Ble_Scn_UUID[1])
        {
            if(checkIdInLog(adv_address, 0) == -1)
            {
                printf("\nFROM: %02x:%02x:%02x:%02x:%02x:%02x %d\n", adv_address[0], adv_address[1], adv_address[2], adv_address[3], adv_address[4], adv_address[5], p_adv_report->rssi);
                //Copy Address
                for (int i = 5; i >= 0; i--)
                    TapeLog[0].scanLog[records].addr[i] = adv_address[i];

                //Copy Rssi
                TapeLog[0].scanLog[records].rssi = p_adv_report->rssi;
                printf("Rssi:%d\n",TapeLog[0].scanLog[records].rssi);

                //Copy Battery
                TapeLog[0].scanLog[records].subBat = (float)(adv_data[10]);;
                printf("BATT:%f\n",TapeLog[0].scanLog[records].subBat);

                records = (records + 1) % NUMBER_OF_TAGS;

            }
        }
    }

    scan_start();
    return;
}

void ble_scanner_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;

    if (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT)
    {
        on_adv_report(&p_gap_evt->params.adv_report);
    }
}

void ble_scanner_stack_init(void)
{
    uint32_t ram_start = 0;
    uint32_t error_code;

    error_code = nrf_sdh_enable_request();
    error_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    error_code = nrf_sdh_ble_enable(&ram_start);
}


void timer_event_handler(void *p_context)
{
#ifdef TEST
    printf("app timeout\n");
#endif //TEST
    timerFlag = 1;
    if (sleepflag == 1)
    {
        if (noSendCounter <= 0)
        {
            timeoutFlagb = 1;
            sleepflag = 0;

#ifdef MULTIPLE_SCAN_PER_SEND
            if (LoggingCount >= NumberOfLogsPerSend)
            {
                LoggingCount = 0;
                sendFlag = true;
            }
            else
            {
                LoggingCount++;
            }
#else // MULTIPLE_SCAN_PER_SEND
            sendFlag = true;
#endif // MULTIPLE_SCAN_PER_SEND
        }
        else
        {
            start_timer(1000 * timeroutPeriod);
            noSendCounter--;
            printf("skipped sending %d\n", noSendCounter);
            sendFlag = false;
        }
    }
}

void start_timer(int32_t miliTimeout)
{
    //We have 10 timer instances we can use, we only use 1, this re-creates it each time
    app_timer_create(&timer_id, APP_TIMER_MODE_SINGLE_SHOT, timer_event_handler); // can also use the single shot mode
    app_timer_start(timer_id, APP_TIMER_TICKS(miliTimeout), NULL);                                //e.g. for 2000 ms
    timerFlag = 0;
}

void init_timer2(void)
{
    app_timer_create(&clock_id, APP_TIMER_MODE_REPEATED, timer2_event_handler);
    app_timer_start(clock_id, APP_TIMER_TICKS(CLOCK_RESOULATION__ms), NULL);
}

void advertising_init(void)
{
    uint32_t err_code;
    ble_advdata_t advdata;
    uint8_t flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    ble_advdata_manuf_data_t manuf_specific_data;

#if ADV_SCANNER_FW == 1
    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER_EXT;
#else
    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;
#endif
    manuf_specific_data.data.p_data = (uint8_t *)m_beacon_info;
    manuf_specific_data.data.size = APP_BEACON_INFO_LENGTH;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type = BLE_ADVDATA_NO_NAME;
    advdata.flags = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr = NULL; // Undirected advertisement.
    m_adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.duration = 0; // Never time out.

    ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
}

void advertising_start(void)
{
    sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
}

void advertising_stop(void)
{
    sd_ble_gap_adv_stop(m_adv_handle);
}

void ble_adv_stack_init(void)
{
    uint32_t ram_start = 0;
    uint32_t ret;

    ret = nrf_sdh_enable_request();
    //printf("req ret = %x\n", ret);
    nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    //printf("ram address    = %x\n", ram_start);
    ret = nrf_sdh_ble_enable(&ram_start);
    //printf("en ret = %x\n", ret);
}

void ble_adv_init(void)
{
    ble_adv_stack_init();
    advertising_init();
    advertising_start();
}

void ble_scanner_init(void)
{
    ble_scanner_stack_init();
    scan_start();
}

void datasend()
{
    memset(data_array,0,sizeof(data_array));
    data_array[0] = 'A';
    data_array[1] = 'C';
    data_array[2] = 'K';
    data_array[3] = '1';
    ble_nus_data_send(&m_nus, data_array, &length, m_conn_handle);
    printf("Notified with ACK\n");
}

#ifdef UD_SER
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    printf("Softdevice asserted!\n");
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void nus_data_handler(ble_nus_evt_t * p_evt)
{
    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;
        printf("\nReceived data (%d bytes) from BLE NUS: ",p_evt->params.rx_data.length);
        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            printf("%02X:",p_evt->params.rx_data.p_data[i]);
        }
        printf("\n");

        if(p_evt->params.rx_data.p_data[2] == 1)
        {
            OTA = true;

            NRF_POWER->GPREGRET = 0xB1;
            nrf_delay_ms(100);
            NVIC_SystemReset();
        }

        advTime = (int)((int)(p_evt->params.rx_data.p_data[6])*1000);

        txlevel = p_evt->params.rx_data.p_data[7];

        scanDuration = (int)((int)(p_evt->params.rx_data.p_data[9])*1000);
        MODE = p_evt->params.rx_data.p_data[10];

        channel = (p_evt->params.rx_data.p_data[11]);
        datasend();
    }
}

void gap_params_init(void)
{
    ble_gap_conn_params_t     gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    sd_ble_gap_device_name_set(&sec_mode,(const uint8_t *)DEVICE_NAME,strlen(DEVICE_NAME));

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency         = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout    = CONN_SUP_TIMEOUT;

    sd_ble_gap_ppcp_set(&gap_conn_params);
}


void services_init(void)
{
    ble_nus_init_t         nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};
    // Initialize Queued Write Module.
    qwr_init.error_handler = NULL;

    nrf_ble_qwr_init(&m_qwr, &qwr_init);
    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    ble_nus_init(&m_nus,&nus_init);
}


void conn_params_evt_handler(ble_conn_params_evt_t * p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
    }
}

void conn_params_init(void)
{
    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = conn_params_evt_handler;
    cp_init.error_handler                  = NULL;
    ble_conn_params_init(&cp_init);
}

void ble_ud_stack_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
#ifdef TEST
            printf("Connected.\n");
#endif
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            connected = true;
            break;

    case BLE_GAP_EVT_DISCONNECTED:
#ifdef TEST
            printf("Disconnected, reason %d.\n",p_ble_evt->evt.gap_evt.params.disconnected.reason);
#endif
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
            connected = false;
            timerFlag = 1;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
#ifdef TEST
            printf("PHY update request.\n");
#endif
            ble_gap_phys_t phys;
            phys.rx_phys = BLE_GAP_PHY_AUTO;
            phys.tx_phys = BLE_GAP_PHY_AUTO;
            sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        break;

        case BLE_GATTC_EVT_TIMEOUT:
#ifdef TEST
            printf("GATT Client Timeout.\n");
#endif
            sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
#ifdef TEST
            printf("GATT Server Timeout.\n");
#endif
            sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            break;

        default:
            break;
    }
}

void scan_start(void)
{
    (void)sd_ble_gap_scan_stop();
    sd_ble_gap_scan_start(&m_scan_params, &m_scan_buffer);
}


void ble_ud_stack_init(void)
{
    uint32_t err_code;

    nrf_sdh_enable_request();
    uint32_t ram_start = 0;

    if (nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start) != NRF_SUCCESS)
        printf("ud fail\n");

    // Enable BLE stack.
    if (nrf_sdh_ble_enable(&ram_start) != NRF_SUCCESS)
    {
        printf("ud memory fail\n");
    }

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_ud_stack_evt_handler, NULL);
}

void ud_advertising_init(void)
{
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type                    = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags                            = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;     // BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids    = m_adv_uuids;

    init.config.ble_adv_fast_enabled    = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout    = APP_ADV_DURATION;
    init.evt_handler = NULL;
    ble_advertising_init(&m_advertising, &init);
    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);

}

void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        printf("Data len is set to 0x%X(%d)\n", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }

    printf("ATT MTU exchange completed. central 0x%x peripheral 0x%x\n",
                                p_gatt->att_mtu_desired_central,
                                p_gatt->att_mtu_desired_periph);
}


void gatt_init(void)
{
    nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
}

void ud_advertising_start(void)
{
    ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
}
#endif //UD_SER


void ud_init(void)
{
#ifdef UD_SER
    ble_ud_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    ud_advertising_init();
    conn_params_init();
#ifdef TEST
    printf("UD_Init...Done...!\n");
#endif
    //application_timers_start();
    ud_advertising_start();
    connected = false;
    printf("UD_Start...!\n");
#endif //UD_SER
}

void sel_txpower(int tx_level)
{
    switch(tx_level)
    {
        case 0:
            txpower = RADIO_TXPOWER_TXPOWER_Neg4dBm; /**< Radio output power. */
        break;

        case 1:
            txpower = RADIO_TXPOWER_TXPOWER_Neg8dBm; /**< Radio output power. */
        break;

        case 2:
            txpower = RADIO_TXPOWER_TXPOWER_Neg12dBm; /**< Radio output power. */
        break;

        case 3:
            txpower = RADIO_TXPOWER_TXPOWER_Neg16dBm; /**< Radio output power. */
        break;

        case 4:
            txpower = RADIO_TXPOWER_TXPOWER_Neg20dBm; /**< Radio output power. */
        break;

        case 5:
            txpower = RADIO_TXPOWER_TXPOWER_Neg30dBm; /**< Radio output power. */
        break;

        case 6:
            txpower = RADIO_TXPOWER_TXPOWER_Neg40dBm; /**< Radio output power. */
        break;

        case 7:
            txpower = RADIO_TXPOWER_TXPOWER_0dBm; /**< Radio output power. */
        break;

        case 8:
            txpower = RADIO_TXPOWER_TXPOWER_Pos4dBm; /**< Radio output power. */
        break;

        default:
            txpower = RADIO_TXPOWER_TXPOWER_0dBm; /**< Radio output power. */
        break;
    }
}

void radio_disable(void)
{
    NRF_RADIO->SHORTS = 0;
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_TXEN = 0;
    NRF_RADIO->TASKS_STOP = 1;
    NRF_RADIO->TASKS_DISABLE = 1;
    NRF_RADIO->EVENTS_END = 1U;

    while (NRF_RADIO->EVENTS_DISABLED == 0)
    {
            // Do nothing.
    }
    NRF_RADIO->EVENTS_DISABLED = 0;
}

void radio_config(void)
{
    NRF_RADIO->POWER = 1;
    NRF_RADIO->SHORTS    = RADIO_SHORTS_READY_START_Msk;
    sel_txpower(txlevel);
    NRF_RADIO->TXPOWER = (txpower << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->MODE        = (mode << RADIO_MODE_MODE_Pos);
    NRF_RADIO->FREQUENCY = channel;
    NRF_RADIO->TASKS_TXEN = 1;
}


void modulation(void)
{
    mode = RADIO_MODE_MODE_Ble_2Mbit;

    // Reset Radio ramp-up time.
    NRF_RADIO->MODECNF0 &= (~RADIO_MODECNF0_RU_Msk);

    // Packet configuration:
    // Bit 25: 1 Whitening enabled
    // Bit 24: 1 Big endian,
    // 4-byte base address length (5-byte full address length),
    // 0-byte static length, max 255-byte payload.
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos) |
                      (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos) |
                      (4UL << RADIO_PCNF1_BALEN_Pos) |
                      (0UL << RADIO_PCNF1_STATLEN_Pos) |
                      ((sizeof(m_tx_packet) - 1) << RADIO_PCNF1_MAXLEN_Pos);
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Disabled << RADIO_CRCCNF_LEN_Pos);

    // Set the device address 0 to use when transmitting
    NRF_RADIO->TXADDRESS   = 0x00UL;
    // Enable the device address 0 to use to select which addresses to receive
    NRF_RADIO->RXADDRESSES = 0x01UL;

    //TRANSMIT_PATTERN_11001100:
    NRF_RADIO->PREFIX0 = 0xCC;
    NRF_RADIO->BASE0     = 0xCCCCCCCC;

    NRF_RADIO->PCNF0 = (0UL << RADIO_PCNF0_S1LEN_Pos) |
                       (0UL << RADIO_PCNF0_S0LEN_Pos) |
                       (RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos) |
                       (RADIO_LENGTH_LENGTH_FIELD << RADIO_PCNF0_LFLEN_Pos);

    m_tx_packet[0] = sizeof(m_tx_packet) - 1;

    // Fill payload with random data.
    for (uint8_t i = 0; i < sizeof(m_tx_packet) - 1; i++)
    {
          m_tx_packet[i + 1] = 0xCC;
    }

    NRF_RADIO->PACKETPTR = (uint32_t)m_tx_packet;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_START_Msk;

    sel_txpower(txlevel);
    NRF_RADIO->TXPOWER    = (txpower << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->MODE       = (mode << RADIO_MODE_MODE_Pos);
    NRF_RADIO->FREQUENCY  = channel;
    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->TASKS_TXEN = 1;

    while (NRF_RADIO->EVENTS_END == 0U)
    {
        // wait
    }
}


void beaconConfig(int Configtime)
{
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    printf("Beacon Config\n");
    m_beacon_info[0] = 0x43; //C
    m_beacon_info[1] = 0x46; //F
    ble_adv_stack_init();
    advertising_init();
    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
    advertising_start();
    start_timer(Configtime);

    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }

    stop_timer();
}

void beaconACK(int ACKtime)
{
    advertising_stop();
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    printf("Beacon ACK\n");
    m_beacon_info[0] = 0xAA;
    m_beacon_info[1] = 0x00;

    if(Tape_Type)
    {
        m_beacon_info[1] = 0x01;
    }

    advertising_init();
    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
    advertising_start();
    start_timer(ACKtime);

    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }

    stop_timer();
}

void tapeFind(void)
{
#ifdef RESET
    while(!reset_flag)
    {
      memset(m_beacon_info,0,sizeof(m_beacon_info));
      m_beacon_info[0] = 0x52; //R
      m_beacon_info[1] = 0x58; //W

      for(int i=0;i<NUMBER_OF_TAGS;i++)
      {
        m_beacon_info[2+(i*3)] = TapeLog[0].scanLog[i].addr[0]; //addr[5] is 1st byte
        m_beacon_info[2+(i*3)+1] = TapeLog[0].scanLog[i].millisTime>>8;
        m_beacon_info[2+(i*3)+2] = TapeLog[0].scanLog[i].millisTime;
        printf("MacID:%02X, Timestamp:%02X:%02X\n",m_beacon_info[2+(i*3)],m_beacon_info[2+(i*3)+1],m_beacon_info[2+(i*3)+2]);
      }

      TapeLog[1].scanLog[0].millisTime = (millis() - millis_init)/1000;
      m_beacon_info[20] = TapeLog[1].scanLog[0].counter;
      m_beacon_info[21] = TapeLog[1].scanLog[0].millisTime>>8;
      m_beacon_info[22] = TapeLog[1].scanLog[0].millisTime;

      m_beacon_info[23] = (uint8_t)(adcToBat(adcResult[3])*10);
      printf("TimeFlag:%02X, ParcelTime:%02X:%02X, TapeBattV:%02X\n",m_beacon_info[20],m_beacon_info[21],m_beacon_info[22],m_beacon_info[23]);
    }
#else
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    m_beacon_info[0] = 0x52; //R
    m_beacon_info[1] = 0x58; //W

    for(int i=0;i<NUMBER_OF_TAGS;i++)
    {
      m_beacon_info[2+(i*3)] = TapeLog[0].scanLog[i].addr[0]; //addr[5] is 1st byte
      m_beacon_info[2+(i*3)+1] = TapeLog[0].scanLog[i].rssi;
      m_beacon_info[2+(i*3)+2] = (uint8_t)(TapeLog[0].scanLog[i].subBat);
      printf("MacID:%02X, Rssi:%02X, BattV:%02X\n",m_beacon_info[2+(i*3)],m_beacon_info[2+(i*3)+1],m_beacon_info[2+(i*3)+2]);
    }

    if(!clearFlag)
    {
      TapeLog[1].scanLog[0].millisTime = (millis() - millis_init)/1000;
    }

    m_beacon_info[20] = TapeLog[1].scanLog[0].counter;
    m_beacon_info[21] = TapeLog[1].scanLog[0].millisTime>>8;
    m_beacon_info[22] = TapeLog[1].scanLog[0].millisTime;

    m_beacon_info[23] = (uint8_t)(adcToBat(adcResult[3])*10);
    printf("TimeFlag:%02X, ParcelTime:%02X:%02X, TapeBattV:%02X\n",m_beacon_info[20],m_beacon_info[21],m_beacon_info[22],m_beacon_info[23]);
#endif //RESET
}

void infrastructureTape(void)
{
#ifdef RESET
    if(!reset_flag)
    {
        advertising_stop();
        memset(m_beacon_info,0,sizeof(m_beacon_info));
        m_beacon_info[0] = 0x49; //I
        m_beacon_info[1] = 0x54; //T
        m_beacon_info[2] = rssiTH; //T

        m_beacon_info[3] = (uint8_t)(adcToBat(adcResult[3])*10);
        printf("RssiTH:%02X, BattV:%02X\n",m_beacon_info[2],m_beacon_info[3]);
        advertising_init();
        sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
        advertising_start();
        random_pwr_fix();
        start_timer(battTime);

        while(!timerFlag && !reset_flag)
        {
            nrf_pwr_mgmt_run();
        }

        stop_timer();
    }
    else
    {
        printf("sleep\n");
        advertising_stop();
        random_pwr_fix();

        while(reset_flag)
        {
            nrf_pwr_mgmt_run();
        }
    }
#else
    advertising_stop();
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    m_beacon_info[0] = 0x49; //I
    m_beacon_info[1] = 0x54; //T
    m_beacon_info[2] = rssiTH; //T

    m_beacon_info[3] = (uint8_t)(adcToBat(adcResult[3])*10);
    printf("RssiTH:%02X, BattV:%02X\n",m_beacon_info[2],m_beacon_info[3]);
    advertising_init();
    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL, TX_POWER[txlevel]);
    advertising_start();
    random_pwr_fix();
    start_timer(battTime);

    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }
    stop_timer();
#endif
}

void relayTape(void)
{
    static uint16_t extra_sleep = 0;
    uint8_t i;

#ifdef RESET
    while(!reset_flag)
    {
        advertising_stop();

        for(int i=0;i<NUMBER_OF_TAGS;i++)
        {
            printf("Tags%d:%d\n",i,TapeLog[0].scanLog[i].counter);

            if(TapeLog[0].scanLog[i].counter>3)
            {
                clearBit(i);
            }

            TapeLog[0].scanLog[i].counter++;
        }

        bleScan(scanDuration);
        memset(m_beacon_info,0,sizeof(m_beacon_info));
        m_beacon_info[0] = 0x52; //R
        m_beacon_info[1] = 0x58; //W

        for(int i=0;i<NUMBER_OF_TAGS;i++)
        {
            m_beacon_info[2+(i*3)] = TapeLog[0].scanLog[i].addr[0]; //addr[5] is 1st byte
            m_beacon_info[2+(i*3)+1] = TapeLog[0].scanLog[i].millisTime>>8;
            m_beacon_info[2+(i*3)+2] = TapeLog[0].scanLog[i].millisTime;
            printf("MacID:%02X, Timestamp:%02X:%02X\n",m_beacon_info[2+(i*3)],m_beacon_info[2+(i*3)+1],m_beacon_info[2+(i*3)+2]);
        }

        TapeLog[1].scanLog[0].millisTime = (millis() - millis_init)/1000;
        m_beacon_info[20] = TapeLog[1].scanLog[0].counter;
        m_beacon_info[21] = TapeLog[1].scanLog[0].millisTime>>8;
        m_beacon_info[22] = TapeLog[1].scanLog[0].millisTime;

        m_beacon_info[23] = (uint8_t)(adcToBat(adcResult[3])*10);
        printf("TimeFlag:%02X, ParcelTime:%02X:%02X, TapeBattV:%02X\n",m_beacon_info[20],m_beacon_info[21],m_beacon_info[22],m_beacon_info[23]);
        advertising_init();
        sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
        advertising_start();
        start_timer(relayTime[timeVer]);

        while(!timerFlag)
        {
            nrf_pwr_mgmt_run();
        }

        stop_timer();
    }

    printf("sleep\n");
    advertising_stop();
    random_pwr_fix();

    while(reset_flag)
    {
        nrf_pwr_mgmt_run();
    }

#else
    bleScan(scanDuration);

    for(int i=0;i<NUMBER_OF_TAGS;i++)
    {
        printf("Tags%d:%d\n",i,TapeLog[0].scanLog[i].counter);
        if(TapeLog[0].scanLog[i].counter>waitTH)
        {
            clearBit(i);
        }
        TapeLog[0].scanLog[i].counter++;
    }

    if(!findtapeFlag)
    {
        memset(m_beacon_info,0,sizeof(m_beacon_info));
        m_beacon_info[0] = 0x52; //R
        m_beacon_info[1] = 0x58; //W

        for(int i=0;i<NUMBER_OF_TAGS;i++)
        {
            m_beacon_info[2+(i*3)] = TapeLog[0].scanLog[i].rssi;
            m_beacon_info[2+(i*3)+1] = TapeLog[0].scanLog[i].millisTime>>8;
            m_beacon_info[2+(i*3)+2] = TapeLog[0].scanLog[i].millisTime;
            printf("MacID:%02X, Timestamp:%02X:%02X\n",m_beacon_info[2+(i*3)],m_beacon_info[2+(i*3)+1],m_beacon_info[2+(i*3)+2]);
        }

        if(!clearFlag)
        {
            TapeLog[1].scanLog[0].millisTime = (millis() - millis_init)/1000;
        }

        m_beacon_info[20] = TapeLog[1].scanLog[0].counter;
        m_beacon_info[21] = TapeLog[1].scanLog[0].millisTime>>8;
        m_beacon_info[22] = TapeLog[1].scanLog[0].millisTime;
        m_beacon_info[23] = (uint8_t)(adcToBat(adcResult[3])*10);
        printf("TimeFlag:%02X, ParcelTime:%02X:%02X, TapeBattV:%02X\n",m_beacon_info[20],m_beacon_info[21],m_beacon_info[22],m_beacon_info[23]);
    }
    else
    {
        tapeFind();
    }

    ble_adv_stack_init();
    advertising_init();
    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, NULL, TX_POWER[txlevel]);
    advertising_start();

#ifdef SAS
    start_timer(advTime);

    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }

    stop_timer();
    advertising_stop();
#endif //SAS

#ifdef SLEEP
    prepare_sleep();
    start_timer(sleepTime);
    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }
    stop_timer();
    prepare_wake();
#endif //SLEEP
#endif
}

void prepare_sleep()
{
    nrf_sdh_disable_request();
}

void prepare_wake()
{
}

void tapeDiagnosis(void)
{
    prepare_sleep();
    prepare_wake();
    NRF_RNG->TASKS_START = 1;
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0){ } // Do nothing.

    radio_disable();

    if(debugMode)
    {
        while(1)
        {
            radio_config();
            printf("Radio Trasmit\n");
            start_timer(advTime);

            while(!timerFlag)
            {
                nrf_pwr_mgmt_run();
            }

            stop_timer();

            radio_disable();
            printf("Radio Sleep\n");
            start_timer(scanDuration);

            while(!timerFlag)
            {
                nrf_pwr_mgmt_run();
            }

            stop_timer();
        }
    }
    else
    {
        radio_config();
        printf("Radio Continue Trasmit\n");

        while(1)
        {
            nrf_pwr_mgmt_run();
        }
    }
}

void radio_with_data(bool flag)
{
    prepare_sleep();
    prepare_wake();
    NRF_RNG->TASKS_START = 1;
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0){ } // Do nothing.

    radio_disable();

    if(flag)
    {
        while(1)
        {
            modulation();
            printf("Radio Trasmit\n");
            start_timer(advTime);
            while(!timerFlag)
            {
                nrf_pwr_mgmt_run();
            }
            stop_timer();

            radio_disable();
            printf("Radio Sleep\n");
            start_timer(scanDuration);
            while(!timerFlag)
            {
                nrf_pwr_mgmt_run();
            }

            stop_timer();
        }
    }
    else
    {
        modulation();
        printf("Radio Continue Trasmit\n");

        while(1)
        {
            nrf_pwr_mgmt_run();
        }
    }
}


void bufferclearTape(void)
{
#ifdef RESET
    if(!reset_flag)
    {
        advertising_stop();
        memset(m_beacon_info,0,sizeof(m_beacon_info));
        m_beacon_info[0] = 0xDE; //I
        m_beacon_info[1] = 0x1E; //T
        m_beacon_info[2] = rssiTH; //T

        m_beacon_info[3] = (uint8_t)(adcToBat(adcResult[3])*100);
        printf("RssiTH:%02X, BattV:%02X\n",m_beacon_info[2],m_beacon_info[3]);
        advertising_init();
        sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
        advertising_start();
        random_pwr_fix();
        start_timer(battTime);

        while(!timerFlag && !reset_flag)
        {
            nrf_pwr_mgmt_run();
        }

        stop_timer();
    }
    else
    {
        printf("sleep\n");
        advertising_stop();
        random_pwr_fix();

        while(reset_flag)
        {
            nrf_pwr_mgmt_run();
        }
    }
#else
    advertising_stop();
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    m_beacon_info[0] = 0xDE; //I
    m_beacon_info[1] = 0x1E; //T
    m_beacon_info[2] = rssiTH; //T

    m_beacon_info[3] = (uint8_t)(adcToBat(adcResult[3])*100);
    printf("RssiTH:%02X, BattV:%02X\n",m_beacon_info[2],m_beacon_info[3]);
    advertising_init();
    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,TX_POWER[txlevel]);
    advertising_start();
    random_pwr_fix();
    start_timer(battTime);

    while(!timerFlag)
    {
        nrf_pwr_mgmt_run();
    }

    stop_timer();
#endif
}

void setConfig(int setTime)
{
    millis_init = millis();
    printf("millis %d\n",millis_init);

    ud_init();
    start_timer(setTime);

    printf("power mgmt-> entring\n");
    while(!timerFlag || connected)
    {
        nrf_pwr_mgmt_run();
    }
    printf("power mgmt-> exiting\n");

    sd_ble_gap_adv_stop(m_advertising.adv_handle);
    nrf_delay_ms(1000);
    nrf_sdh_disable_request();
    nrf_delay_ms(1000);
    stop_timer();

    if(txlevel > 8)
    {
        txlevel = 7;
    }

    if(channel < 0 || channel > 100)
    {
        channel = 17;
    }

    printf("OTA: %d\n",           OTA);
    printf("advTime: %d\n",       advTime);
    printf("txlevel: %d\n",       txlevel);
    printf("scanDuration: %d\n",  scanDuration);
    printf("MODE: %d\n",          MODE);
    printf("channel: %d\n",       channel);

    if(OTA)
    {
        NRF_POWER->GPREGRET = 0xB1;
        //sd_power_gpregret_set(0,0xB1);
        nrf_delay_ms(100);
        NVIC_SystemReset();
    }

    ble_adv_stack_init();
}

#ifdef RESET
void mode_change(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    reset_flag = !reset_flag;
}

void mode_init()
{
    nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t mode_config;
    mode_config.sense = NRF_GPIOTE_POLARITY_HITOLO;
    mode_config.pull = NRF_GPIO_PIN_NOPULL;
    nrf_drv_gpiote_in_init(6, &mode_config, mode_change);
    nrf_drv_gpiote_in_event_enable(6, true);
}
#endif //RESET

void config_init()
{
    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);
    nrf_pwr_mgmt_init();
}

void get_ble_mac(void)
{
    uint8_t devId[6];
    char idString[64];

    devId[0] = (NRF_FICR->DEVICEADDR[1] >> 8) | 0xC0;
    devId[1] = (NRF_FICR->DEVICEADDR[1] & 0xFF);
    devId[2] = ((NRF_FICR->DEVICEADDR[0] >> 24) & 0xFF);
    devId[3] = ((NRF_FICR->DEVICEADDR[0] >> 16) & 0xFF);
    devId[4] = ((NRF_FICR->DEVICEADDR[0] >> 8) & 0xFF);
    devId[5] = (NRF_FICR->DEVICEADDR[0] & 0xFF);
    sprintf(idString, "MAC = %02X:%02X:%02X:%02X:%02X:%02X\n", devId[0], devId[1], devId[2], devId[3], devId[4], devId[5]);
    printf("%s", idString);
}

void setVerBits(int V)
{
#ifdef SAS
    printf("Scan: %d\tAdv: %d\tSleep: %d\n",scanDuration,advTime,sleepTime);
#else
    printf("Scan: %d\tAdv: %d\n",scanDuration,sleepTime);
#endif //SAS

    switch(V)
    {
        case 0:
            clearBit(7);
            clearBit(6);
            break;
        case 1:
            clearBit(7);
            setBit(6);
            break;
        case 2:
            setBit(7);
            clearBit(6);
            break;
        case 3:
            setBit(7);
            setBit(6);
            break;
        default:
            clearBit(7);
            clearBit(6);
            break;
    }
}

void get_hw_ver(void)
{
    uint32_t info = NRF_FICR->INFO.VARIANT;
    uint8_t *p = (uint8_t *)&info;
    printf ("nRF%x Variant: %c%c%c%c\n",NRF_FICR->INFO.PART, p[3], p[2], p[1], p[0]);
    printf ("package: %x\n",NRF_FICR->INFO.PACKAGE);
    printf ("DevID: %x%x\n", NRF_FICR->DEVICEID[0],NRF_FICR->DEVICEID[1]);
}

#ifdef CUT
void cut_change(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    cut_flag = !cut_flag;
}

void cut_init()
{
    nrf_drv_gpiote_in_config_t cut_config;
    cut_config.sense = NRF_GPIOTE_POLARITY_HITOLO; //GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    cut_config.pull = NRF_GPIO_PIN_NOPULL;
    nrf_drv_gpiote_in_init(13, &cut_config, cut_change);
    nrf_drv_gpiote_in_event_enable(13, true);
}
#endif //CUT


int main(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    config_init();
    get_ble_mac();
    get_hw_ver();
    init_timer();
    init_timer2();

#ifdef LORA_TEST
    lora_radio_enable();
    lora_radio_configure();

    lora_radio_cw_mode_w_data();
    while(1);
#endif

#ifdef PAR_CONFIG
    setConfig(120000);
#else
    ble_adv_stack_init();
#endif //PAR_CONFIG

#ifdef SAS
    //For test:
    //1 - 8s (750ms scan, 1s adv, sleep 6.25s)
    //2 - 8s (500ms scan, 1s adv, sleep 6.5s)
    //3 - 4s (750ms scan, 1s adv, sleep 2.25s)
    //4 - 4s (500ms scan, 1s adv, sleep 2.5s)
    //5 - 2s (1s scan, 1s adv, 0 sleep)
    //scanTimeVer = 0; //750
    //scanTimeVer = 1; //500
    //scanTimeVer = 2; //1000
    timeVer = 0; //2000
    //scanDuration = scanTime[scanTimeVer];
    //sleepTime = relayTime[timeVer] - scanDuration - advTime;
    //printf("scan %d adv %d sleep %d total %d\n", scanDuration, advTime, sleepTime, (scanDuration + advTime + sleepTime));
    //printf("Check total = %d\n", relayTime[timeVer]);
#else
    sleepTime = relayTime[timeVer] - scanDuration;
#endif //SAS

    setVerBits(timeVer);

    while(1)
    {
        switch(MODE)
        {
            case 0:
                printf("Sleep ON\n");
                random_pwr_fix();
                prepare_sleep();

                while(1)
                {
                    nrf_pwr_mgmt_run();
                }
            break;

            case 1:
                printf("Parcel Turned ON\n");
                debugMode = false;

                while(1)
                {
                    relayTape();
                }
            break;

            case 2:
                printf("CW Mode without modulation ON\n");

                if(advTime == 0 && scanDuration == 0)
                {
                    debugMode = false;
                }
                else
                {
                    debugMode = true;
                }

                while(1)
                {
                    tapeDiagnosis();
                }
            break;

            case 3:
                printf("CW Mode with modulation ON\n");

                if(advTime == 0 && scanDuration == 0)
                {
                    while(1)
                    {
                        radio_with_data(false);
                    }
                }
                else
                {
                    while(1)
                    {
                        radio_with_data(true);
                    }
                }
            break;

            default:
                printf("Sleep ON\n");
                random_pwr_fix();
                prepare_sleep();

                while(1)
                {
                    nrf_pwr_mgmt_run();
                }
            break;
        }
    }
}

/*
Battery Level, 50% battery voltage on pin PO_29/AIN5
4.3V-2.75V/2 = 2.15V-1.355V/6 = 0.358-0.22V     1024=0.6V, 0=0V
*/
int checkBit(int n)
{
    return ((TapeLog[1].scanLog[0].counter >> n) & 1);
}
void clearBit(int n)
{
    if(checkBit(n) == 1)
    {
        TapeLog[1].scanLog[0].counter &= ~(1 << n);
        TapeLog[0].scanLog[n].millisTime = (millis() - millis_init)/1000;
    }
    TapeLog[0].scanLog[n].counter = 0;
}

void setBit(int n)
{
    if(checkBit(n) == 0)
    {
        TapeLog[1].scanLog[0].counter |= 1 << n;
        TapeLog[0].scanLog[n].millisTime = (millis() - millis_init)/1000;
    }
    TapeLog[0].scanLog[n].counter = 0;
}

void wdt_init(int seconds)
{
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos); //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.    b) Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CRV = seconds * 32768;                                                                                                                                                                        //ca 3 sek. timout
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;                                                                                                                                                                 //Enable reload register 0

    // Enable WDT interrupt:
    NVIC_EnableIRQ(WDT_IRQn);
    NRF_WDT->INTENSET = WDT_INTENSET_TIMEOUT_Msk;

    NRF_WDT->TASKS_START = 1; //Start the Watchdog timer
}

void wdt_reset()
{
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

bool checkZeros(void)
{
    for(int i=1;i<=(data_len-11);i++)
    {
        if(adv_data[adv_len-i] != 0)
        {
            return false;
        }
    }
    return true;
}

void zeroTags(int tagy)
{
    records = 0;
    millis_init = 0;
    memset(m_beacon_info,0,sizeof(m_beacon_info));
    TapeLog[1].scanLog[0].millisTime = 0;
    TapeLog[1].scanLog[0].counter = 0;
    clearFlag = true;

    for (int k = 0; k < NUMBER_OF_TAGS; k++)
    {
        TapeLog[tagy].scanLog[k].subID = 0;
        TapeLog[tagy].scanLog[k].tempRec = 0;
        TapeLog[tagy].scanLog[k].Lorarssi = 0;
        TapeLog[tagy].scanLog[k].subBat = 0;
        TapeLog[tagy].scanLog[k].millisTime = 0;
        TapeLog[tagy].scanLog[k].counter = 0;
        memset(TapeLog[tagy].scanLog[k].addr, 0, MAX_ID_BUF_LENGTH);

        TapeLog[tagy].glat = -1;
        TapeLog[tagy].glon = -1;
        TapeLog[tagy].logNo = 0;
        TapeLog[tagy].relayBat = 0;
        memset(TapeLog[tagy].relayId, 0, MAX_ID_BUF_LENGTH);
    }
}

int checkIdInLog(uint8_t inArray[], int testlog)
{
    for (int k = 0; k < NUMBER_OF_TAGS; k++)
    {
        if (compareArrays(inArray, TapeLog[testlog].scanLog[k].addr, 6))
        {
            return k;
        }
    }
    return -1;
}

int compareArrays(uint8_t a[], uint8_t b[], int n)
{
    for (int ii = 0; ii < n; ii++)
    {
        if (a[ii] != b[ii])
        {
            return 0;
        }
    }

    return 1;
}

void scan_stop(void)
{
    sd_ble_gap_scan_stop();
}


//This supports 10 timers, we currently use just 1 called timer_id
//It uses rtc1, so watch out for making collisions by using rtc1 for something else
void init_timer(void)
{
    app_timer_init();
}


uint32_t read_timer(void)
{
    return app_timer_cnt_get();
}

void stop_timer()
{
#ifdef TEST
    //printf("Stopping Timer\n");
#endif
    app_timer_stop(timer_id);
    timerFlag = 0;
}

void bleAdv(int timeOutMillis)
{

#ifdef TEST
    printf("BLE Adv Start\n");
#endif //TEST

    ble_adv_init();
    start_timer(timeOutMillis);
    while (!timerFlag)
    {
        nrf_pwr_mgmt_run(); //nrf_delay
    }
    advertising_stop();
    nrf_sdh_disable_request();
    stop_timer();

#ifdef TEST
    printf("BLE Adv Stop\n");
#endif //TEST

    nrf_delay_ms(50);
}

void wakeFromSleep()
{
    sleepflag = 0;
    wdt_reset();
    nrf_delay_ms(100);
    stop_timer();
#ifdef TEST
    printf("Woke up\n");
#endif //TEST
}

void goToSleep()
{
    sendFlag = false;
    timeoutFlagb = 0;
    sleepflag = 1;
}

uint8_t *hex_decode(char *in, size_t len, uint8_t *out)
{
    unsigned int i, t, hn, ln;

    for (t = 0, i = 0; i < len; i += 2, ++t)
    {
        hn = in[i] > '9' ? (in[i] | 32) - 'a' + 10 : in[i] - '0';
        ln = in[i + 1] > '9' ? (in[i + 1] | 32) - 'a' + 10 : in[i + 1] - '0';

        out[t] = (hn << 4) | ln;
    }
    return out;
}

void bleScan(int timeOutMillis)
{
#ifdef TEST
    printf("BLE Scan Start %d\n",timeOutMillis);
#endif //TEST
    BLE_GW = false;
    Drop_Flag = false;
    B_gps = false;
    B_gps = false;
    B_lora = false;
    B_cell = false;
    //records = 0;
    oxy_scanned = false;
    ble_scanner_init();
    start_timer(timeOutMillis);
    while (!timerFlag && (records < NUMBER_OF_TAGS))
        nrf_pwr_mgmt_run();
    scan_stop();
    stop_timer();
    nrf_sdh_disable_request();

#ifdef TEST
    printf("BLE Scan Stop\n");
#endif //TEST
}

void random_pwr_fix()
{
}
