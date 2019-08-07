//#include "ssd1306_fonts.h"

#ifndef SSD1306_FONTS_H
#define SSD1306_FONTS_H 120

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

#include "stm32f10x.h"
#include "string.h"


/**
 * @brief  Font structure used on my LCD libraries
 */
typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef_t;


extern FontDef_t Font_12x15;

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif//SSD1306_FONTS_H

