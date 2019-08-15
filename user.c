//user.c

#include "user.h"
#include "const_var.h"

// addresses of registers
volatile uint32_t *DWT_CONTROL = (uint32_t *)0xE0001000;
volatile uint32_t *DWT_CYCCNT = (uint32_t *)0xE0001004; 
volatile uint32_t *DEMCR = (uint32_t *)0xE000EDFC; 

uint32_t count;

void user_program(void)
{
  static bool tgl;
  struct bmp280_uncomp_data ucomp_data;
  uint32_t pres64;
  int32_t temp32;
  float Height;
  float Cadence;
  float Velocity, Velocity_avg;
  float Distance_km;
  uint16_t Distance_m;
  
  *DEMCR = *DEMCR | 0x01000000; // enable the use DWT
  *DWT_CYCCNT = 0; // Reset cycle counter  
  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable cycle counter
  count = 0;
  
  // LED
  if(tgl)GPIO_SetBits(GPIOC, GPIO_Pin_13);
  else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
  tgl = !tgl;
  
  bmp280_get_uncomp_data(&ucomp_data, &bmp);
  bmp280_get_comp_pres_64bit(&pres64, ucomp_data.uncomp_press, &bmp);
  bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);
  
  Height = -0.292256318324018 * (double)(temp32 + 27315) * log((double)pres64 / (double)pres64_);
  
  
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14))
  {
    Impulse_wheel = 1;
    pres64_= pres64; // height
    Capture_total = 0;
  }
  
  Velocity = Kv/(float)Capture1 * Wheel_length; //Kf = 65535/max_seconds * 36/10000 (Km/h)
  Velocity_avg = Kv* Wheel_length /((double)Capture_total / (double)Impulse_wheel);
  Cadence = Kc/Capture2;
  
  Distance_m = (Impulse_wheel * Wheel_length)/1000; //m
  Distance_km = (float)Distance_m/1000;
 
  Count_stop_vel++;
  Count_stop_cad++;
  
  if(Count_stop_cad>6)Cadence   = 0;
  
  //SSD1306_DrawFilledRectangle(0,20,127,52, SSD1306_COLOR_BLACK);// clean area to prevent screen artifacts due variable character width
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_GotoXY(0, 0);
  if(Count_stop_vel<=6) {
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f Km/h",  Velocity);
    Count_stop_vel = 6;}
  else SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f Km/h avg",  Velocity_avg);
  
  SSD1306_GotoXY(0, 20);
  SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f RPM",  Cadence);
  
  SSD1306_GotoXY(0, 40);
  //SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f m",  Height);

  if(Distance_m < 1000)SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%d m",  Distance_m);
  else SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f km",  Distance_km);
  
  
  SSD1306_UpdateScreenDMA();
  count = *DWT_CYCCNT;

}
