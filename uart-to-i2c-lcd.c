#include <avr/io.h>
#include "i2x/i2x.h"
#include "i2x/lcd_i2x.h"


#define F_CPU 16000000UL
#define BAUD 9600
#define MY_UBRR F_CPU/16/BAUD-1

void uart_init(unsigned int ubrr) {
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_transmit(unsigned char data) {
  while (!(UCSR0A & (1<<UDRE0)));
  UDR0 = data;
}

void uart_transmit_string(const char *str) {
  while (*str) {
    uart_transmit(*str++);
  }
}

unsigned char uart_receive(void) {
  while (!(UCSR0A & (1<<RXC0)));
  return UDR0;
}

int main() {
  uart_init(MY_UBRR);
  i2x_init(I2X_FREQ_100K,true);
  lcd_i2x_init(0x3F);
  char CLEAR_CMD = '#';

  while(1) {
    unsigned char c = uart_receive();
    if (c == CLEAR_CMD) {
      lcd_i2x_clear();
    } else {
      char str[2] = { c, '\0' };
      lcd_i2x_print(str);
    }
    
  }
}
