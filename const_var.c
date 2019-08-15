// const_var.c

#include "const_var.h"

uint16_t IC3ReadValue1, IC3ReadValue1_, Capture1,IC3ReadValue2, IC3ReadValue2_, Capture2;
uint32_t Impulse_wheel;
uint16_t Wheel_length = 112;//mm

uint32_t pres64_ = 25503577;

const float Kv = 582.20539, Kc = 485444.44;

uint8_t Count_stop_vel, Count_stop_cad;
uint64_t Capture_total;
struct bmp280_dev bmp;