#include <stdint.h>

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;


extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

extern const uint8_t c_chSingal816[16];

//wifi Signal
extern const uint8_t w4[104];
extern const uint8_t w3[104];
extern const uint8_t w2[104];
extern const uint8_t w1[104];
extern const uint8_t w0[104];

extern const uint8_t line[16];
extern const uint8_t Splash[1024];

//Spinner
extern const uint8_t l1 [102];
extern const uint8_t l2 [101];
extern const uint8_t l3 [101];
extern const uint8_t l4 [101];
extern const uint8_t l5 [101];
extern const uint8_t l6 [101];
extern const uint8_t l7 [101];
extern const uint8_t l8 [101];
extern const uint8_t l9 [101];
extern const uint8_t l10 [101];
extern const uint8_t l11 [101];
extern const uint8_t l12 [101];

//Wait Icon
extern const uint8_t hourGlass[1024];


extern const uint8_t c_chBat816[16];

#endif // __SSD1306_FONTS_H__
