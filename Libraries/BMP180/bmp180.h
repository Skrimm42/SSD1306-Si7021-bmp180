
#ifndef __BMP180_H
#define __BMP180_H

#define BMP180_I2C_PORT I2C1
#define OSS 3 //oversampling 00, 01, 10, 11 - влияет на точность 

typedef struct
{
  float T;    // in C
  float P;    // in MPa
} BMP180_measurements;



void BMP180_Setup(void);
void BMP180_get_T_P(BMP180_measurements* BMP180_res);

#endif //__BMP180_H