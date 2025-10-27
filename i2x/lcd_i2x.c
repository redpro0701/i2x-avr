#include "lcd_i2x.h"
#include "i2x.h"
#include <util/delay.h>

#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_RS          0x01

static uint8_t lcd_addr;

static void lcd_pulse(uint8_t data) {
    i2x_transmit(lcd_addr, &data, 1, false);
    _delay_us(1);
    data |= LCD_ENABLE;
    i2x_transmit(lcd_addr, &data, 1, false);
    _delay_us(1);
    data &= ~LCD_ENABLE;
    i2x_transmit(lcd_addr, &data, 1, false);
    _delay_us(1);
}

static void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0XF0) | LCD_BACKLIGHT | mode;
    lcd_pulse(data);
}

static void lcd_write_byte(uint8_t byte, uint8_t mode) {
    lcd_write_nibble(byte & 0xF0, mode);
    lcd_write_nibble(byte << 4, mode);
}

static void lcd_command(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
}

static void lcd_data(uint8_t data) {
    lcd_write_byte(data, LCD_RS);
}

void lcd_i2x_init(uint8_t i2c_addr) {
    lcd_addr = i2c_addr;

    _delay_ms(50);
    lcd_write_nibble(0x30, 0); _delay_ms(5);
    lcd_write_nibble(0x30, 0); _delay_ms(150);
    lcd_write_nibble(0x30, 0);
    lcd_write_nibble(0x20, 0);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_i2x_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_i2x_set_cursor(uint8_t row, uint8_t col) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3) row = 3;
    lcd_command(0x80 | (col + row_offsets[row]));
}

void lcd_i2x_print(const char* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_i2x_putc(char c) {
    lcd_data(c);
}