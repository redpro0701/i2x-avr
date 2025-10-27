#ifndef LCD_I2X_H_
#define LCD_I2X_H_

#include <stdint.h>
#include <stdbool.h>

void lcd_i2x_init(uint8_t i2x_addr);
void lcd_i2x_clear(void);
void lcd_i2x_set_cursor(uint8_t row, uint8_t col);
void lcd_i2x_print(const char* str);
void lcd_i2x_putc(char c);

#endif