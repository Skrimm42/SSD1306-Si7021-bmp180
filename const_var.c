// const_var.c

#include "const_var.h"

uint16_t IC3ReadValue1, IC3ReadValue1_, Capture1 = 1, IC3ReadValue2, IC3ReadValue2_, Capture2 = 1;
uint32_t Impulse_wheel, Impulse_crank;
uint16_t Wheel_length = 2248;//mm

uint32_t pres64_ = 25503577;

const float Kv = 29.1235955, Kc = 485444.44;

uint8_t Count_stop_vel, Count_stop_cad;
uint64_t Capture1_total, Capture2_total;
struct bmp280_dev bmp;

uint8_t prog_state;
bool Fcad, Fvel;
float Velocity, Velocity_avg, Velocity_max;
uint32_t Distance_m, Distance_total;
float Cadence, Cadence_avg, Cadence_max;
float Height;
uint8_t display_frame_n, display_frame_nw;
