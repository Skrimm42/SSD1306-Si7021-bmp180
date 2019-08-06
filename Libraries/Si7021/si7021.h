
// si7021.h

#ifndef __SI7021_H
#define __SI7021_H

#define READ_RH_HOLD_MASTER_MODE        0xE5 //Measure Relative Humidity, Hold Master Mode 0xE5
#define READ_RH_NO_HOLD_MASTER_MODE     0xF5 //Measure Relative Humidity, No Hold Master Mode 0xF5
#define READ_T_HOLD_MASTER_MODE         0xE3 //Measure Temperature, Hold Master Mode 0xE3
#define READ_T_NO_HOLD_MASTER_MODE      0xF3 //Measure Temperature, No Hold Master Mode 0xF3
#define READ_T_PREVIOUS_MEASUREMENT     0xE0 //Read Temperature Value from Previous RH Measurement 0xE0
#define RESET_MODULE                    0xFE //Reset 0xFE
#define WRITE_RH_T_USER_REG_1           0xE6 //Write RH/T User Register 1 0xE6 
#define READ_RH_T_USER_REG_1            0xE7 //Read RH/T User Register 1 0xE7
#define WRITE_HEATER_CONTROL_REG        0x51 //Write Heater Control Register 0x51
#define READ_HEATER_CONTROL_REG         0x11 //Read Heater Control Register 0x11
#define READ_EID_1_BYTE_01              0xFA //Read Electronic ID 1st Byte 0xFA 0x0F
#define READ_EID_1_BYTE_02              0x0F
#define READ_EID_2_BYTE_01              0xFC //Read Electronic ID 2nd Byte 0xFC 0xC9
#define READ_EID_2_BYTE_02              0xC9
#define READ_FW_REV_01                  0x84 //Read Firmware Revision 0x84 0xB8
#define READ_FW_REV_02                  0xB8

#define SI7021_ADDR                     0x80 //0x40

#define I2C_PORT I2C1

typedef struct
{
  float Temperature_f;
  uint8_t RH;
} Si7021_measurments;


// Prototypes
void Si7021_Setup(void);
uint16_t SI75_ReadReg(uint8_t reg);
float Si7021_ReadTemp(void);
uint8_t Si7021_ReadRH(void);
float Si7021_ReadTempFormer(void); 
void Si7021_Read_RH_Temp(Si7021_measurments* Si7021_res);


#endif //__SI7021_H