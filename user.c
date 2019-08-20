//user.c

#include "user.h"
#include "const_var.h"

// addresses of registers
volatile uint32_t *DWT_CONTROL = (uint32_t *)0xE0001000;
volatile uint32_t *DWT_CYCCNT = (uint32_t *)0xE0001004; 
volatile uint32_t *DEMCR = (uint32_t *)0xE000EDFC; 

uint32_t count;


bool GetButton1(void);

bool GetButton1(void)
{
  static bool Fres_;
  bool Fres, Freturn;
  
  Freturn = 0;
  Fres = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
  if((Fres^Fres_) & Fres == 1)
  {
    Freturn = 1;
  }
  Fres_ = Fres;
  
  return Freturn; 
}

void user_program(void)
{
  static bool tgl;
  struct bmp280_uncomp_data ucomp_data;
  uint32_t pres64;
  int32_t temp32;
  uint32_t Distance_km;
  static uint8_t Distance_tmp = 0;
  
  //  *DEMCR = *DEMCR | 0x01000000; // enable the use DWT
  //  *DWT_CYCCNT = 0; // Reset cycle counter  
  //  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable cycle counter
  //  count = 0;
  
  // LED
  if(tgl)GPIO_SetBits(GPIOC, GPIO_Pin_13);
  else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
  tgl = !tgl;
  
  //Switch Frames on OLED
  display_frame_n += GetButton1();
  if(display_frame_n == 3)display_frame_n = 0;
  
  // switch to IDLE 
  if(Count_stop_vel++ >= VEL_LIMIT)
  {
    Count_stop_vel = VEL_LIMIT; 
    prog_state &= ~STATE_VEL;
  }
  if(Count_stop_cad++ >= CAD_LIMIT)
  {
    Count_stop_cad = CAD_LIMIT;
    prog_state &= ~STATE_CAD;
  }
  
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  
  
  bmp280_get_uncomp_data(&ucomp_data, &bmp);
  bmp280_get_comp_pres_64bit(&pres64, ucomp_data.uncomp_press, &bmp);
  bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);
  
  Height = -0.292256318324018 * (double)(temp32 + 27315) * log((double)pres64 / (double)pres64_);
  
  
  Distance_m = (Impulse_wheel * Wheel_length)/1000; //meters
  Distance_km = (uint32_t)Distance_m/1000;
  
  if((prog_state | ~STATE_VEL) == ~STATE_VEL)//idle vel
  {
    Velocity = 0;   
  }
  if((prog_state & STATE_VEL) == STATE_VEL)//work vel
  {
    Velocity = Fvel*(Kv/(float)Capture1 * Wheel_length); //Kv = 65535/max_seconds * 36/10000 (Km/h)
    if((Capture1_total == 0)||(Impulse_wheel == 0))Velocity_avg = 0;
    else Velocity_avg = Kv* Wheel_length /((double)Capture1_total / (double)Impulse_wheel);   
  }
  if((prog_state | ~STATE_CAD) == ~STATE_CAD)//idle cadence
  {
    Cadence = 0;
  }
  if((prog_state & STATE_CAD) == STATE_CAD)//work cadence
  {
    Cadence = Fcad *(Kc/(float)Capture2);
    if((Capture2_total == 0)||(Impulse_crank == 0))Cadence_avg = 0;
    else Cadence_avg = Kc / (float)(Capture2_total/Impulse_crank);
  }
  
  //Calc Max Cadence/Velocity
  if(Velocity > Velocity_max) Velocity_max = Velocity;
  if(Cadence > Cadence_max) Cadence_max = Cadence;
  
  Distance_tmp = Distance_km - Distance_tmp;
  Distance_total += Distance_tmp;
  Distance_tmp = Distance_km;
  
  //Display
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  if(display_frame_n == 0)
  {
    if(((prog_state | ~STATE_VEL) == ~STATE_VEL) )//idle vel 
    {
      SSD1306_GotoXY(0, 0); 
      SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f Km/h avg",  Velocity_avg);
    }
    else if((prog_state & STATE_VEL) == STATE_VEL)//work vel
    {
      SSD1306_GotoXY(0, 0); 
      SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f Km/h",  Velocity);
    }
    if(((prog_state | ~STATE_CAD) == ~STATE_CAD) )//idle cadence
    {
      SSD1306_GotoXY(0, 20);
      SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.1f RPM avg",  Cadence_avg);
    }
    else if((prog_state & STATE_CAD) == STATE_CAD)//work cadence 
    {
      SSD1306_GotoXY(0, 20);
      SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.1f RPM",  Cadence);
    }
    SSD1306_GotoXY(0, 40);
    if(Distance_m < 1000)SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%d m",  Distance_m);
    else SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f km",  (float)Distance_m/1000.0f);
  }
  else if(display_frame_n == 1)
  {
    SSD1306_GotoXY(0, 0); 
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.2f Km/h max",  Velocity_max);
    SSD1306_GotoXY(0, 20);
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.1f RPM max",  Cadence_max);
    SSD1306_GotoXY(0, 40);
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.1f m(h)",  Height);
  }
  else if(display_frame_n == 2)
  {
    SSD1306_GotoXY(0, 0); 
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%.1f degC",  (float)temp32/100.0f);
    SSD1306_GotoXY(0, 20);
    SSD1306_printf(&lessPerfectDOSVGA_13ptFontInfo, "%d km", Distance_total);
  }
  SSD1306_UpdateScreenDMA();
  
  count = *DWT_CYCCNT;
  
  // Reset button
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14))
  {
    Impulse_wheel = 0;
    Impulse_crank = 0;
    pres64_= pres64; // height
    Capture1_total = 0;
    Capture2_total = 0;
    Velocity_max = 0;
    Cadence_max = 0;
    Velocity_avg = 0;
    Cadence_avg = 0;   
  }
}
