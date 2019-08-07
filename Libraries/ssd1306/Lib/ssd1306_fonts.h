//ssd1306_fonts.c

#ifndef SSD1306_FONTS_H
#define SSD1306_FONTS_H

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

#include "stm32f10x.h"

typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint8_t *data; /*!< Pointer to data font data array */
} FontDef_t;


extern FontDef_t Font_6x8;

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif // SSD1306_FONTS_H