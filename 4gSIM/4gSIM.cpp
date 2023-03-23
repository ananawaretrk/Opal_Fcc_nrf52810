/*************************************************************************
* nrf9160 7/10/2020
*************************************************************************/

#include "4gSIM.h"
#include "app_uart.h"
#include "app_fifo.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 1024                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1024                         /**< UART RX buffer size. */

uint8_t N_err;
MultiCellData mCellData;

static const char* apn_responses[] = {"\"globaldata.iot\"", "\"ibasis.iot\"", "\"hologram\"", "\"broadband\"", "\"m2m.telus.iot\"","\"teal\"","\"m2m.com.attz\""};

void (*cell_timer_handler_ptr)(uint32_t milisec_timeout) = NULL;
volatile int* p_cell_timer_flag = NULL;

extern void callmainwithbyte(uint8_t byte);

void N_handle(app_uart_evt_t * p_event){
    uint32_t err_code;
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR){
        printf("!!! WARN: NBIoT Comm error\n");
        //APP_ERROR_HANDLER(p_event->data.error_communication);
    }else if (p_event->evt_type == APP_UART_FIFO_ERROR){
        printf("!!! WARN: NBIoT Fifo error\n");
        //APP_ERROR_HANDLER(p_event->data.error_code);
    }
    else if (p_event->evt_type == APP_UART_DATA_READY){
    }
}

void nbiot::Init(uint32_t uart_Rx, uint32_t uart_Tx, void (*timer_handler_ptr)(uint32_t milisec_timeout), volatile int* p_timer_flag){
  SerialInit(uart_Rx, uart_Tx); 
  app_uart_flush();
  cell_timer_handler_ptr = timer_handler_ptr;
  p_cell_timer_flag      = p_timer_flag;
}

void nbiot::SerialInit(uint32_t uart_Rx, uint32_t uart_Tx){
    const app_uart_comm_params_t SIM = {uart_Rx, uart_Tx, 10, 11, APP_UART_FLOW_CONTROL_DISABLED, false, NRF_UART_BAUDRATE_115200};
    //app_uart_flush();
    //app_uart_close();
    APP_UART_FIFO_INIT(&SIM,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       N_handle,
                       APP_IRQ_PRIORITY_HIGHEST,
                       N_err);
    printf("NBIoT Serial module initialized\r\n");                                      
}

void nbiot::UnInitialize(){
    printf("sending AT+CFUN=0\n");
    SendCommand("AT+CFUN=0\r\n");
    printf("Reply = %s\n",cell_buffer);
    app_uart_flush();
    app_uart_close();
}
  
void nbiot::GetConfig(){
  printf("sending AT+CFUN?\n");
  SendCommand("AT+CFUN?\r\n");
  printf("Reply = %s\n",cell_buffer);
  nrf_delay_ms(10);
  printf("sending  AT%HWVERSION\n");
  SendCommand("AT%HWVERSION\r\n");
  printf("NRF9160 HW Version is = %s\n",cell_buffer);
  nrf_delay_ms(10);
  SendCommand("AT%XSYSTEMMODE?\r\n");
  printf("NRF9160 SYSTEM MODE is = %s\n",cell_buffer);
}

void nbiot::MCT(){
  printf("sending AT%NCELLMEAS\n");
  SendCommand("AT%NCELLMEAS\r\n");
  printf("Reply = %s\n",cell_buffer);
}

bool nbiot::CheckTCPConnection(){
  int ret = 1;
  printf("sending AT#XTCPCONN?\n");
  SendCommand("AT#XTCPCONN?\r\n");
  printf("Reply = %s\n",cell_buffer);
  char cmp_string[30];
  memset(cmp_string, 0 ,30);
  strncpy(cmp_string,"+XTCPCONN: 1\r\nOK\r",30);
  ret = strncmp(cell_buffer,cmp_string,30);
  return !ret;
}

int nbiot::CheckConnection(){
  int ret = -1;
  bool command_status = false;
  printf("sending AT+CGDCONT?\n");
  command_status = SendCommand("AT+CGDCONT?\r\n");
  printf("Reply = %s\n",cell_buffer);

  if(command_status == true && strlen(cell_buffer)){
    char *pch;
    pch = strtok((char*)cell_buffer, ",");
    pch = strtok(NULL, ",");// Neglect the first set and save second set in pch
    pch = strtok(NULL, ","); // go to third set
    if(pch != NULL){
        printf("NBIoT Connected to Network: %s\n", pch);
        uint8_t no_of_networks = sizeof(apn_responses)/sizeof(apn_responses[0]);
        for(int i = 0; i < no_of_networks; i++){
          ret = strcmp(pch,apn_responses[i]);
          if(ret == 0){
            return ret;
          }
        }
    }
  }
  return -1;
}
  
void nbiot::GPSSetup(){
  printf("sending AT+CFUN=0\n");
  SendCommand("AT+CFUN=0\r\n");
  printf("Reply = %s\n",cell_buffer);

  //printf("sending AT+CEREG=2\n");
  //sendCommand("AT+CEREG=2\r\n");
  //printf("Reply = %s\n",cell_buffer);

  //printf("sending AT%%XSYSTEMMODE=1,0,1,0\n");
  //sendCommand("AT%XSYSTEMMODE=1,0,1,0\r\n");
  //printf("Reply = %s\n",cell_buffer);

  //printf("sending AT%%XMAGPIO=1,0,0,1,1,1574,1577\n");
  //sendCommand("AT%XMAGPIO=1,0,0,1,1,1574,1577\r\n");
  //printf("Reply = %s\n",cell_buffer);

  //printf("sending AT%%XCOEX0=1,1,1565,1586\n");
  //sendCommand("AT%XCOEX0=1,1,1565,1586\r\n");
  //printf("Reply = %s\n",cell_buffer);

  printf("sending AT+CFUN=1\n");
  SendCommand("AT+CFUN=1\r\n");
  printf("Reply = %s\n",cell_buffer);

}

void nbiot::GPSCheck(){
  printf("sending AT#XGPS=1,3\n");
  SendCommand("AT#XGPS=1,3\r\n");
  printf("Reply = %s\n",cell_buffer);

}

void nbiot::Sleep(){
  printf("sending AT+CFUN=0\n");
  SendCommand("AT+CFUN=0\r\n");
  printf("Reply = %s\n",cell_buffer);

}

void nbiot::OpenUDPSocket(){
  printf("sending AT#XSOCKET=1,2\n");
  SendCommand("AT#XSOCKET=1,2\r\n");
  printf("Reply = %s\n",cell_buffer);
}

void nbiot::OpenTCPSocket(){
  printf("sending AT#XSOCKET=1,1\n");
  SendCommand("AT#XSOCKET=1,1\r\n");
  printf("Reply = %s\n",cell_buffer);
}

void nbiot::CloseSocket(){
  printf("sending AT#XSOCKET=0\n");
  printf("Closing Socket\n");
  SendCommand("AT#XSOCKET=0\r\n");
  printf("Reply = %s\n",cell_buffer);
}

void nbiot::BindTCPPort(const char *port){
  char temp_port[30];
  memset(temp_port, 0, 30);
  snprintf(temp_port,30,"AT#XBIND=%s\r\n", port);
  printf("sending %s", temp_port);
  SendCommand(temp_port);
  printf("Reply = %s\n",cell_buffer);
}

bool nbiot::ConnectTcpServer(const char *tip, const char *tport){
  char connect_cmd[220];
  memset(connect_cmd, 0, 220);
  snprintf(connect_cmd,220,"AT#XTCPCONN=\"%s\",%s\r\n", tip, tport);
  if(SendCommand(connect_cmd) == false){
      printf("Couldn't connect to TCP Server Reply = %s\n",cell_buffer);
      return false;
  }
  printf("Connected to TCP Server Reply = %s\n",cell_buffer);
  return true;
}


void nbiot::SetConfig(){

  printf("sending AT\%XSYSTEMMODE=1,1,0,0\n");
  SendCommand("AT%XSYSTEMMODE=1,1,0,0\r\n");
  printf("Reply = %s\n",cell_buffer);

  nrf_delay_ms(10);
  printf("sending AT+CFUN=1\n");
  SendCommand("AT+CFUN=1\r\n");
  printf("Reply = %s\n",cell_buffer);

  nrf_delay_ms(10);
  printf("sending AT+CRSM\n");
  SendCommand("AT+CRSM=176,12258,0,0,10\r\n");
  printf("Reply is = %s\n",cell_buffer);
  
  nrf_delay_ms(10);
  printf("sending AT\%XICCID\n");
  SendCommand("AT%XICCID\r\n");
  printf("SIM ICCID is = %s\n",cell_buffer);

  nrf_delay_ms(10);
  SendCommand("AT%XSYSTEMMODE?\r\n");
  printf("NRF9160 SYSTEM MODE is = %s\n",cell_buffer);

  nrf_delay_ms(10);
  SendCommand("AT%XDEEPSEARCH=1\r\n");
  printf("NRF9160 XDEEPSEARCH is = %s\n",cell_buffer);
}

void nbiot::SendData(const char *cmddata){
  char temp_data[CELL_BUFFER_SIZE] = {0};
  memset(cell_buffer, 0, CELL_BUFFER_SIZE);
  //sprintf(temp_data, "AT#XUDPSENDTO=GET /prox%s HTTP/1.1\r\nHost: trk-ing-pilot.azure-api.net\r\nConnection: keep-alive\r\n\r\n", cmddata); 
  //sprintf(temp_data, "AT#XUDPSENDTO=\"trk-ing-pilot.azure-api.net\",2442,\"%s\"", cmddata);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XUDPSENDTO=\"50.116.9.199\",2442,\"%s\"\r\n", cmddata); 
  printf("---> NBIoT sending udp data: %s", temp_data);
  SendCommand(temp_data);
  printf("Reply = %s\n",cell_buffer);
}

void nbiot::SendDataIp(const char *ip, const char *port, const char *cmddata){
  char temp_data[CELL_BUFFER_SIZE] = {0};
  memset(cell_buffer, 0, CELL_BUFFER_SIZE);
  //sprintf(temp_data, "AT#XUDPSENDTO=GET /prox%s HTTP/1.1\r\nHost: trk-ing-pilot.azure-api.net\r\nConnection: keep-alive\r\n\r\n", cmddata); 
  //sprintf(temp_data, "AT#XUDPSENDTO=\"trk-ing-pilot.azure-api.net\",2442,\"%s\"", cmddata);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XUDPSENDTO=\"%s\",%s,\"%s\"\r\n",ip,port,cmddata); 
  printf("---> NBIoT sending udp data: %s", temp_data);
  SendCommand(temp_data);
  printf("Reply = %s\n",cell_buffer);
  
}

void nbiot::SendTcpData(const char *data, const char *endpoint){
  char temp_data[CELL_BUFFER_SIZE];
  memset(temp_data, 0, CELL_BUFFER_SIZE);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XTCPSEND=\"%s%s\"\r\n", endpoint, data);
  printf("---> NBIoT sending tcp data = %s", temp_data);
  nrf_delay_ms(200);
  SendCommand(temp_data);
  printf("Reply = %s\n",cell_buffer);
  
}

int nbiot::RecvEPOData(const char *characters, const char *timeout, char *nbiottcpbuffer){
  char temp_data[CELL_BUFFER_SIZE];
  memset(temp_data, 0, CELL_BUFFER_SIZE);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XTCPRECV=%s,%s\r\n",characters, timeout);
  SendCommand(temp_data);
  char *p =  cell_buffer + 11; //strtok(cell_buffer, "#XTCPRECV: ");
  for(int i = 0; i < strlen(p) - 3; i++){
      if(p[i] == '#' && p[i + 1] == '#' && p[i + 1] == '#'){
        p[i - 1] = '\0';
        break;
      }
  }
  //printf("Receive TCP Data Len is %d Reply = %s\n\n",strnlen(p,CELL_BUFFER_SIZE),p);
  memcpy(nbiottcpbuffer, p, strlen(p));
  p = strtok(NULL, "\n");
  return atoi(p);
}

int nbiot::RecvTcpData(const char *characters, const char *timeout, char *nbiottcpbuffer){
  char temp_data[CELL_BUFFER_SIZE];
  memset(temp_data, 0, CELL_BUFFER_SIZE);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XTCPRECV=%s,%s\r\n",characters, timeout);
  SendCommand(temp_data);
  printf("Receive TCP Data Len is %d Reply = %s\n\n",strnlen(cell_buffer,CELL_BUFFER_SIZE),cell_buffer);
  char *p = strtok(cell_buffer, "#XTCPRECV: ");
  memcpy(nbiottcpbuffer, p, strlen(p));
  p = strtok(NULL, "\n");
  return atoi(p);
}

void nbiot::RecvData(){
  memset(cell_buffer, 0, CELL_BUFFER_SIZE );
  printf("Receiving UDP data from Cloud\n");
  SendCommand("AT#XUDPRECVFROM=\"50.116.9.199\",2443,100,10\r\n");
  printf("Received Reply = %s\n",cell_buffer);
}

void nbiot::RecvDataIp(const char *ip, const char *port, const char *len, const char *timeout, char *nbiotbuffer){
  char at_string[CELL_BUFFER_SIZE] = {0};
  memset(cell_buffer, 0, CELL_BUFFER_SIZE );  
  printf("Receiving UDP data from Cloud\n");
  snprintf(at_string,CELL_BUFFER_SIZE,"AT#XUDPRECVFROM=\"%s\",%s,%s,%s", ip,port,len,timeout);
  SendCommand(at_string);
  printf("Received Reply = %s\n",cell_buffer);
  memcpy(nbiotbuffer, cell_buffer, strlen(cell_buffer));
}

bool nbiot::GetFullSignalQuality(float *rsrq, float *rsrp){
  memset(cell_buffer, 0, CELL_BUFFER_SIZE);
  SendCommand("AT+CESQ\r\n");
  char *pch = strtok(cell_buffer, ",");
  if (pch != NULL){
    pch = strtok(NULL, ",");
    if (pch != NULL){
      pch = strtok(NULL, ",");
      if (pch != NULL){
        pch = strtok(NULL, ",");
        if (pch != NULL){
          pch = strtok(NULL, ",");
          if (pch != NULL){
            *rsrq = atof(pch);
            *rsrq = (*rsrq / 2) - 19.5;
             pch = strtok(NULL, ",");
             if (pch != NULL){
              *rsrp = atof(pch) - 140;
             }else{
               return false;
              }
          }else{ 
            return false;
          }
        }else{
            return false;
        }
      }else{
            return false;
      }
    }else{
      return false;
    }
  }else{
    return false;
  }
  return true;
}

int nbiot::GetSignalQuality() {
  int signalRssi = 0;
  memset(cell_buffer, 0, CELL_BUFFER_SIZE );
  SendCommand("AT+CESQ\r\n");
  char *pch = strtok(cell_buffer, ",");
  if(pch){
     pch = strtok(NULL,",");
     if(pch){
        pch = strtok(NULL,",");
        if(pch){
          pch = strtok(NULL,",");
          if(pch){
            pch = strtok(NULL,",");
            if(pch){
              pch = strtok(NULL,",");
              if(pch){
                printf("Signal Quality is %s\n",pch);
                signalRssi = atoi(pch) - 140;
              }
            }
          }
        }
     }
  }
  return signalRssi;
}


int nbiot::GetMultiCellTower(char *arr)
{
    int count = 0;
    int i=0;
    SendCommand("AT%NCELLMEAS\r\n");          
    printf("NCELLMEAS returned = %s\n", cell_buffer);
    if(strstr(cell_buffer, "ERROR") != NULL){
      return -1;
    }
    while (SIM_SERIAL_available() && i < CELL_BUFFER_SIZE - 1) {
      uint8_t c = SIM_SERIAL_read();
      arr[i++] = c;
    }
    if(i > 14 && arr[14] == '0'){
     count = 1;
    }
    //printf("buffer[0] = %c%c%c%c \n", buffer[0],buffer[1],buffer[2],buffer[3]);
    /*char* pch;
    pch = strtok(buffer,",");
    mCellData.status = pch;
    if(mCellData.status == NRF_SUCCESS){
      pch = strtok(buffer,",");
      if(pch!=NULL){
        strcpy(mCellData.cell_id, pch);
        pch = strtok(buffer,",");
        if(pch!=NULL){
          strcpy(mCellData.plmn, pch);
          pch = strtok(buffer,",");
          if(pch!=NULL){
            strcpy(mCellData.tac, pch);
            pch = strtok(buffer,",");
            if(pch!=NULL){
              mCellData.timing_adv = atoi(pch);
              pch = strtok(buffer,",");
              if(pch!=NULL){
                mCellData.curr_earfcn = atoi(pch);
                pch = strtok(buffer,",");
                if(pch!=NULL){
                  mCellData.phy_cell_id = atoi(pch);
                  pch = strtok(buffer,",");
                  if(pch!=NULL){
                    mCellData.rsrp = atoi(pch);
                    pch = strtok(buffer,",");
                    if(pch!=NULL){
                      mCellData.rsrq = atoi(pch);
                      pch = strtok(buffer,",");
                      if(pch!=NULL){
                        mCellData.meas_time = atoi(pch);
                        count++;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      else{
        return 0;
      }
      
      for(int i=1; i<=10; i++){
        pch = strtok(buffer,",");
        if(pch!=NULL){
          mCellData.tower[i].earfcn = atoi(pch);
          pch = strtok(buffer,",");
          if(pch!=NULL){
            mCellData.tower[i].phy_cell_id = atoi(pch);
            pch = strtok(buffer,",");
            if(pch!=NULL){
              mCellData.tower[i].rsrp = atoi(pch);
              pch = strtok(buffer,",");
              if(pch!=NULL){
                mCellData.tower[i].rsrq = atoi(pch);
                pch = strtok(buffer,",");
                if(pch!=NULL){
                  mCellData.tower[i].time_diff = atoi(pch);
                  count++;
                }
              }
            }
          }
        }
        else{
          break;
        }
      }
    } */
    return count;
}

bool nbiot::GetCellTime(Nordic_TimeStamp *ts1) {
  char *pch, nHr[3], nMin[3], nSec[3], nY[3], nM[3], nD[3];
  memset(nHr,0,3);
  memset(nMin,0,3);
  memset(nSec,0,3);
  memset(nY,0,3);
  memset(nM,0,3);
  memset(nD,0,3);
  bool command_status = SendCommand("AT+CCLK?\r\n");
  printf("CCLK returned = %s\n", cell_buffer);
  pch = strtok((char *)cell_buffer, "\"");
  if(pch!=NULL){
    pch = strtok(NULL, "/");
    strcpy(nY,pch);
  }else{
    return false;
  }
  if(pch!=NULL){
    pch = strtok(NULL, "/");
    strcpy(nM,pch);
  }else{
    return false;
  }
  if(pch!=NULL){
    pch = strtok(NULL, ",");
    strcpy(nD,pch);
  }else{
    return false;
  }
  if(pch!=NULL){
    pch = strtok(NULL, ":");
    strcpy(nHr,pch);
  }else{
    return false;
  }
  if(pch!=NULL){
    pch = strtok(NULL, ":");
    strcpy(nMin,pch);
  }else{
    return false;
  }
  if(pch!=NULL){
    pch = strtok(NULL, "-");
    strcpy(nSec,pch);
  }else{
    return false;
  }
  printf("Cell Returned Time: %s:%s:%s, Date: %s\\%s\\%s\n", nHr, nMin, nSec, nD, nM, nY);
  ts1->day = atoi(nD);
  ts1->month = atoi(nM);
  ts1->year = (atoi(nY))+2000;
  ts1->hour = atoi(nHr);
  ts1->minute = atoi(nMin);
  ts1->second = atoi(nSec);
  return true;
}

bool nbiot::GetCellId(Nordic_Towers *list){
  uint16_t i;
  uint32_t val1, val2;
  char *pch, mcc[6] = {0}, mnc[6] = {0}, region[6] = {0}, ccid[16] = {0}, rssi[6] = {0};
   
  if(SendCommand("AT+CEREG?\r\n")) {          //get cid, lac, mcc and mnc
    //printf("CEREG returned = %s\n", buffer);

    pch = strtok((char *)cell_buffer, "\"");
    pch = strtok(NULL, "\"");
    //printf("LAC = %s\n", pch);
    strcpy(region,pch);
    pch = strtok(NULL, "\"");
    pch = strtok(NULL, "\"");
    //printf("ID = %s\n", pch);
    strncpy(ccid,pch,16);


    SendCommand("AT+COPS?\r\n");           //get cid, lac, mcc and mnc
    printf("COPS returned = %s\n", cell_buffer);
    pch = strtok((char *)cell_buffer, "\"");
    pch = strtok(NULL, "\"");
    //printf("MCC&MNC = %s\n", pch);
    strcpy(mnc,pch+3);
    memcpy(mcc,pch,3);
    mcc[3] = 0;

    sscanf(region, "%x", &val1);
    sscanf(ccid, "%x", &val2);
    //printf("NB,%s,%s,%d,%d \n",mcc,mnc,val1,val2);

    list->mnc = atoi(mnc);
    list->mcc = atoi(mcc);
    list->loc = val1;
    list->cellid = val2;
    return true;
  } else {
    return false;
  }
}

bool nbiot::SendCommand(const char* cmd, unsigned int timeout, const char* expected){
  if (cmd) {
    app_uart_flush();   
    SIM_SERIAL_write(cmd);
  }

  memset(cell_buffer,0,CELL_BUFFER_SIZE);
  *p_cell_timer_flag = 0;
  cell_timer_handler_ptr(timeout);
  int n = 0;
  uint8_t c;
  while (!*p_cell_timer_flag){
    if (SIM_SERIAL_available()) {
      uint8_t c = SIM_SERIAL_read();
      if (n >= CELL_BUFFER_SIZE - 2){
        printf("NBIoT Command Response Buffer Overflow\n\n");
        cell_buffer[CELL_BUFFER_SIZE - 1] = 0;
        return false;
      }
      cell_buffer[n++] = c;
      if (strstr(cell_buffer, expected ? expected : "OK\r")) {
        //printf("NBIoT Command Response received OK\n");
        return true;
      }
    }
  }
  return false;
}

int nbiot::RecvOTAData(const char *characters, const char *timeout, char *nbiottcpbuffer){
  char temp_data[CELL_BUFFER_SIZE];
  memset(temp_data, 0, CELL_BUFFER_SIZE);
  snprintf(temp_data,CELL_BUFFER_SIZE,"AT#XTCPRECV=%s,%s\r\n",characters, timeout);
  SendCommand(temp_data);
  char *p =  cell_buffer + 11; //strtok(cell_buffer, "#XTCPRECV: ");
  for(int i = 0; i < strlen(p) - 3; i++){
      if(p[i] == '#' && p[i + 1] == '#' && p[i + 1] == '#'){
        p[i - 1] = '\0';
        break;
      }
  }
  //printf("Receive TCP Data Len is %d Reply = %s\n\n",strnlen(p,CELL_BUFFER_SIZE),p);
  memcpy(nbiottcpbuffer, p, strlen(p));
  p = strtok(NULL, "\n");
  return atoi(p);
}

void nbiot::GetCellFailParameters(uint8_t* cfail_stat, uint8_t* cfail_cause_type, uint16_t* cfail_rej_cause){
   *cfail_stat = 0;
   *cfail_cause_type  = 0;
   *cfail_rej_cause   = 0;
   if(SendCommand("AT+CEREG?\r\n")) {
    printf("------> CEREG RESP is\n");
    printf("%s\n",cell_buffer);
    char *pch = strtok(cell_buffer, ",");
    if(pch){
     pch = strtok(NULL,",");
     if(pch){
        *cfail_stat = atoi(pch);
        pch = strtok(NULL,",");
        if(pch){
          pch = strtok(NULL,",");
          if(pch){
            pch = strtok(NULL,",");
            if(pch){
              pch = strtok(NULL,",");
              if(pch){
               *cfail_cause_type = atoi(pch);
               pch = strtok(NULL,",");
               if(pch){
                *cfail_rej_cause = atoi(pch);
               }
              }
            }
          }
        }
     }
    }
   }
}

int nbiot::CalculateTxBand(int my_value)
{
  for(const txFrequency_data_type range : txFrequencies) {
    if (my_value > range.startFrequency && my_value <= range.endFrequency)
      return range.returnValue;
  }
  return -1;
}

int nbiot::CalculateRxBand(int my_value)
{
  for(const rxFrequency_data_type range : rxFrequencies) {
    if (my_value > range.startFrequency && my_value <= range.endFrequency)
      return range.returnValue;
  }
  return -1;
}

void nbiot::CwFunction(int txrx, int enable, int frequency, int power, bool modulated)
{

  char local_txrx[2];
  char local_enable[2];
  char local_band[3];
  char local_frequency[7];
  char local_power[5];

  itoa(txrx, local_txrx, 10);
  itoa(enable, local_enable, 10);
  itoa(((frequency * 10)-4), local_frequency, 10);
  itoa(power, local_power, 10);

  if(txrx == 1)
  {
    itoa(CalculateTxBand(frequency), local_band, 10);
  }
  else if(txrx == 0)
  {
    printf("RX Band: %d\n", CalculateRxBand(frequency));
    itoa(CalculateRxBand(frequency), local_band, 10);
  }

  char temp_data[CELL_BUFFER_SIZE];
  memset(temp_data, 0, CELL_BUFFER_SIZE);
  char var_buf[CELL_BUFFER_SIZE];
  memset(var_buf, 0, CELL_BUFFER_SIZE);
  memcpy(temp_data, "AT%XRFTEST=", strlen("AT%XRFTEST="));
  if(modulated == false)
  {
    snprintf(var_buf, CELL_BUFFER_SIZE, "%s,%s,%s,%s,%s,1,4,0,0,0,0,0,0\r\n", local_txrx, local_enable, local_band, local_frequency, local_power);
  }
  else if (modulated == true)
  {
    snprintf(var_buf, CELL_BUFFER_SIZE, "%s,%s,%s,%s,%s,1,0,1,1,0,1,0,0\r\n", local_txrx, local_enable, local_band, local_frequency, local_power);
  }
  strncat(temp_data, var_buf, CELL_BUFFER_SIZE);
  printf("---> NBIoT sending CW data = %s", temp_data);
  nrf_delay_ms(200);
  SendCommand(temp_data);
  printf("Reply = %s\n",cell_buffer);
  
}