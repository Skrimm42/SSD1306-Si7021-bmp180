//const_var.h

#ifndef __CONST_VAR_H_
#define __CONST_VAR_H_

#include "stm32f10x.h"
#include <stdbool.h>
#include "bmp280.h"

#define STATE_VEL 0x01
#define STATE_CAD 0x02

#define CAD_LIMIT 6
#define VEL_LIMIT 6
#define STANDBY_LIMIT 50

extern uint16_t IC3ReadValue1, IC3ReadValue1_, Capture1,IC3ReadValue2, IC3ReadValue2_, Capture2;
extern uint32_t Impulse_wheel, Impulse_crank;
extern uint16_t Wheel_length;
extern const float Kv, Kc;
extern uint32_t pres64_;
extern uint8_t Count_stop_vel, Count_stop_cad;
extern struct bmp280_dev bmp;
extern uint32_t Capture1_total, Capture2_total;
extern uint8_t prog_state;
extern bool Fcad, Fvel;
extern float Velocity, Velocity_avg, Velocity_max;
extern uint32_t Distance_m, Distance_total;
extern float Cadence, Cadence_avg, Cadence_max;
extern float Height;
extern uint8_t display_frame_n, display_frame_nw;
extern uint8_t standby_cntr;
extern uint64_t Impulse_wheel_total;

#endif  //__CONST_VAR_H_