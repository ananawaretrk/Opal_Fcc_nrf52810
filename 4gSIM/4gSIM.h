/*************************************************************************
* nrf9160 7/10/2020
*************************************************************************/
#ifndef NRF_NB_H
#define NRF_NB_H

#include "Arduino.h"
#include "app_uart.h"
#include "app_fifo.h"
#include "nrf_delay.h" // if commented then NRFX_SUCCESS error


#define  ERROR_CODE 0
#define CELL_BUFFER_SIZE 1024

typedef struct {
  uint16_t mnc;
  uint16_t mcc;
  uint32_t cellid;
  uint16_t loc;
} Nordic_Towers;

//typedef struct {
//    uint16_t nday;
//    uint16_t nmonth;
//    uint16_t nyear;
//    uint16_t nhour;
//    uint16_t nmin;
//    uint16_t nsec;
//} Nordic_TimeStamp;

// Structure and initialization of CW frequencies
typedef struct txFrequency_structure{
    int startFrequency;
    int endFrequency;
    int returnValue;
} txFrequency_data_type;

txFrequency_data_type static const txFrequencies[] = {
  {1920, 1980, 1},
  {1850, 1910, 2},
  {1710, 1785, 3},
  {1710, 1755, 4},
  {824,  849,  5},
  {880,  915,  8},
  {699,  716,  12},
  {777,  787,  13},
  {815,  830,  18},
  {830,  845,  19},
  {832,  862,  20},
  {1850, 1915, 25},
  {814,  849,  26},
  {703,  748,  28},
  {1710, 1780, 66}
};

typedef struct rxFrequency_structure {
    int startFrequency;
    int endFrequency;
    int returnValue;
}rxFrequency_data_type;

rxFrequency_data_type static const rxFrequencies[] = {
  {2110, 2170, 1},
  {1930, 1990, 2},
  {1805, 1880, 3},
  {2110, 2155, 4},
  {869,  894,  5},
  {925,  960,  8},
  {729,  746,  12},
  {746,  756,  13},
  {875,  890,  18},
  {830,  845,  19},
  {791,  821,  20},
  {1930, 1995, 25},
  {859,  894,  26},
  {758,  803,  28},
  {2110, 2200, 66}
};



//Parameters of neighboring cell towers
typedef struct {
    uint16_t earfcn;
    uint16_t phy_cell_id;
    uint16_t rsrp;
    uint16_t rsrq;
    uint16_t time_diff;
} NTower;

//Combining all the cell data
typedef struct {
    bool status;
    char cell_id[15];
    char plmn[15];
    char tac[15];
    uint16_t timing_adv;
    uint16_t curr_earfcn;
    uint16_t phy_cell_id;
    uint16_t rsrp;
    uint16_t rsrq;
    uint16_t meas_time;
    NTower tower[10];
} MultiCellData;

typedef struct {
    float lat;
    float lon;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} Nordic_TimeStamp;

class nbiot {
public:
    char cell_buffer[CELL_BUFFER_SIZE];
    void Init(uint32_t uart_Rx, uint32_t uart_Tx, 
                    void (*timer_handler_ptr)(uint32_t milisec_timeout), volatile int* p_timer_flag);
    void UnInitialize();
    void SerialInit(uint32_t uart_Rx, uint32_t uart_Tx);
    void GetConfig();
    void MCT();
    void SetConfig();
    int CheckConnection();
    void GPSSetup(); /* TODO: Fix Implementation*/
    void GPSCheck(); /* TODO: Fix Implementation*/
    void Sleep();
    bool CheckTCPConnection();
    void OpenUDPSocket();
    void OpenTCPSocket();
    void CloseSocket();
    void BindTCPPort(const char *port); /* USE when acting as TCP Server or UDP*/
    bool ConnectTcpServer(const char *ip, const char *port);
    void SendTcpData(const char *data, const char *endpoint);
    int  RecvEPOData(const char *characters, const char *timeout, char *nbiottcpbuffer);
    int  RecvTcpData(const char *characters, const char *timeout, char *nbiottcpbuffer);
    void SendData(const char * cmddata);/* TODO: Fix Implementation */
    void SendDataIp(const char *ip, const char *port, const char *cmddata); /* TODO: Fix Implementation */
    void RecvData();
    void RecvDataIp(const char *ip, const char *port, const char *len, const char *timeout, char *nbiotbuffer);
    bool GetFullSignalQuality(float *rsrq, float *rsrp);
    int  GetSignalQuality();
    //Receive data for all nearby cell towers
    //int getMultiTower();
    //int getMultiTower();
    //int getMultiTower();
    int GetMultiCellTower(char *arr);
    bool GetCellTime(Nordic_TimeStamp *ts1);
    bool GetCellId(Nordic_Towers *list);
    bool SendCommand(const char* cmd, unsigned int timeout = 20000, const char* expected = 0);
    int  RecvOTAData(const char *characters, const char *timeout, char *nbiottcpbuffer);
    void GetCellFailParameters(uint8_t* cfail_stat, uint8_t* cfail_cause_type, uint16_t* cfail_rej_cause);
    int CalculateTxBand(int value);
    int CalculateRxBand(int value);
    void CwFunction(int txrx, int enable, int frequency, int power, bool modulated);

    bool SIM_SERIAL_available(){
      if(app_uart_fifo_length() > 0){return true;}
      else {return false;}
    }

    void SIM_SERIAL_writeln(const char * str){
      unsigned int len = strlen(str);
      for (int i = 0; i < len; i++) {while(app_uart_put(str[i]) != NRFX_SUCCESS);}
      while(app_uart_put('\n') != NRF_SUCCESS);
      nrf_delay_ms(1000);
    }

    void SIM_SERIAL_write(const char * str){
      unsigned int len = strlen(str);
      for (int i = 0; i < len; i++) {while(app_uart_put(str[i]) != NRFX_SUCCESS);}
      nrf_delay_ms(100);
    }

    uint8_t SIM_SERIAL_read(){
      uint8_t ch;
      while(app_uart_get(&ch) != NRFX_SUCCESS);
      return ch;
    }
    
private:
    byte checkbuffer(const char* expected1, const char* expected2 = 0, unsigned int timeout = 2000);
    void purgeSerial();
    byte m_bytesRecv;
    uint32_t m_checkTimer;
};


#endif // NRF_NB_H