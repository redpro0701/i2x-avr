#include <stdio.h>
#include "i2x/i2x.h"
#include "i2x/lcd_i2x.h"
#include <util/delay.h>

void setup() {
    i2x_init(I2X_FREQ_100K,true);
    lcd_i2x_init(0x3F);
}

void loop() {
    lcd_i2x_clear();
    for (int i = 0;i <= 100; i++) {
        char buffer[12];
        snprintf(buffer, sizeof(buffer), "%d", i);
        lcd_i2x_print(buffer);
        _delay_ms(1000);
        lcd_i2x_clear();
    }
}

int main(void) {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
