#ifndef EVENTSFLAG_AND_ERRORS_H
#define EVENTSFLAG_AND_ERRORS_H

#include <stdint.h>
enum {
  NormalMode               = 0,
  InMotion                 = 1,
  RunningToSleep           = 2,
  EnteringAeroplaneMode    = 4,
  InAeroplaneMode          = 5,
  OutOffAeroplaneMode      = 6,
  EnteringWhiteTapeMode    = 7,
  InWhiteTapeMode          = 17,
  OutOffWhiteTapeMode      = 8,
  EnteringHibernationMode  = 9,
  InHibernationMode        = 18,
  OutOffHibernationMode    = 10,
  CutCircuitReset          = 11,
  WatchDogReset            = 12,
  ButtonPressReset         = 13,
  CPULockReset             = 14,
  SoftwareInstructionReset = 15,
  OtherResets              = 16,
  EnteringShutdownMode     = 19,
  InShutdownMode           = 20,
  ExitingShutDownMode      = 22,
  HeartbeatMode            = 21,
  UldMode                  = 23,
};

enum {
  INITIALIZING          = 0,
  RUNNING               = 1,
  SLEEPING              = 2,
  AEROPLANE_MODE        = 3,
  HIBERNATION_MODE      = 4,
  WHITE_TAPE_RUNNING    = 5,
  RUNNING_SLEEP_MODE    = 6,
  SYSTEM_SHUTDOWN_MODE  = 7,
  OTA_MODE              = 8,
  CHECK_VALID_STOP_MODE = 9,
  ULD_MODE_CHECKING     = 10
};

#define TCA_ERROR_MASK        (1 << 0)
#define TEMP_ERROR_MASK       (1 << 1)
#define HUM_ERROR_MASK        (1 << 2)
#define LIGHT_ERROR_MASK      (1 << 3)
#define GPS_ERROR_MASK        (1 << 4)
#define ACC_ERROR_MASK        (1 << 5)
#define PRESS_ERROR_MASK      (1 << 6)
#define MCP_I2C_ERROR_MASK    (1 << 7)
#define CELL_ERROR_MASK       (1 << 8)
#define FLASH_ERROR_MASK      (1 << 9)
#define MCPOPEN_ERROR_MASK    (1 << 10)
#define MCPSHORT_ERROR_MASK   (1 << 11)
#define MCPTIMEOUT_ERROR_MASK (1 << 12)


void SetErrorMask(uint16_t error_mask);
void ClearErrorMask(uint16_t error_mask);
void ZeroErrorMask();
uint16_t GetErrorMask();

#endif