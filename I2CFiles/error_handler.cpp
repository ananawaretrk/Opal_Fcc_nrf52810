#include "eventflag_and_errors.h"

static volatile uint16_t peripheral_error_mask = 0;


void SetErrorMask(uint16_t error_mask){
  peripheral_error_mask |= error_mask;
}

void ClearErrorMask(uint16_t error_mask){
  peripheral_error_mask &= ~(error_mask);
}

void ZeroErrorMask(){
  peripheral_error_mask = 0;
}

uint16_t GetErrorMask(){
  return peripheral_error_mask;
}