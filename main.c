// Copyright (c) 2025 redpro0701. All Rights Reserved.
#include "i2x/i2x.h"
#include <util/delay.h>
#include <stdio.h>

#define LCD_ADDR      0x3F
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS        0x01

static void lcd_pulse(uint8_t data) {
  i2x_transmit(LCD_ADDR, &data, 1, false);
  _delay_us(1);
  data |= LCD_ENABLE;
  i2x_transmit(LCD_ADDR, &data, 1, false);
  _delay_us(1);
  data &= ~LCD_ENABLE;
  i2x_transmit(LCD_ADDR, &data, 1, false);
  _delay_ms(1);
}

static void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
  uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT | mode;
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

void lcd_init(void) {
  _delay_ms(50);
  lcd_write_nibble(0x30,0); _delay_ms(5);
  lcd_write_nibble(0x30,0); _delay_us(150);
  lcd_write_nibble(0x30,0);
  lcd_write_nibble(0x20,0);

  lcd_command(0x28);
  lcd_command(0x0C);
  lcd_command(0x06);
  lcd_command(0x01);
  _delay_ms(1);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
  const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x52};
  lcd_command(0x80 | (col + row_offsets[row]));
}

int main() {
  i2x_init(I2X_FREQ_100K, true);
  lcd_init();

  lcd_set_cursor(0,0);
  const char* msg = "Hello, 20x4 LCD!";
  for (uint8_t i = 0; msg[i]; i++) {
    lcd_data(msg[i]);
  }

  lcd_set_cursor(1,0);
  const char* msg2 = "Line 2 test";
  for (uint8_t i = 0; msg2[i]; i++) {
    lcd_data(msg2[i]);
  }

  while (1) {

  }

}
