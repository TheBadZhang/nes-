
#include "u8g2.h"
#include "U8g2lib.h"


uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr) {
	return 0;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	return 0;
}

class U8G2_SSD1327_MIDAS_128X128_f_4W_HW_SPI : public U8G2 {
	public: U8G2_SSD1327_MIDAS_128X128_f_4W_HW_SPI(const u8g2_cb_t *rotation) : U8G2() {
		u8g2_Setup_ssd1327_midas_128x128_f(&u8g2, rotation, u8x8_byte_4wire_hw_spi, u8x8_gpio_and_delay);
	}
};