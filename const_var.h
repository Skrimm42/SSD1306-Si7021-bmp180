//const_var.h

#ifndef __CONST_VAR_H_
#define __CONST_VAR_H_

#include "stm32f10x.h"
#include <stdbool.h>
#include "bmp280.h"

extern uint16_t IC3ReadValue1, IC3ReadValue1_, Capture1,IC3ReadValue2, IC3ReadValue2_, Capture2;
extern uint32_t Impulse_wheel;
extern uint16_t Wheel_length;
extern const float Kv, Kc;
extern uint32_t pres64_;
extern uint8_t Count_stop_vel, Count_stop_cad;
extern struct bmp280_dev bmp;
extern uint64_t Capture_total;
#endif  //__CONST_VAR_H_