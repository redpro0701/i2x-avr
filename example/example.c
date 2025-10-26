#include "i2x_master.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
    // Initialize I2C at 100 kHz with internal pull-ups
    ix_init(IX_FREQ_100K, true);

    uint8_t data_to_write[2] = {0x00, 0xAB}; // Example: memory address 0x00, value 0xAB
    uint8_t data_read[1];

    ret_code_t err;

    // Write 1 byte to EEPROM at address 0x50
    err = ix_master_transmit(0x50, data_to_write, 2, false);
    if (err != I2X_SUCCESS) {
        // Handle error
        while(1);
    }

    _delay_ms(10); // EEPROM write delay

    // Read 1 byte from EEPROM at address 0x00
    uint8_t addr[1] = {0x00};
    err = ix_master_transmit(0x50, addr, 1, true); // Send memory address with repeated start
    if (err != I2X_SUCCESS) {
        while(1);
    }

    err = ix_master_receive(0x50, data_read, 1);
    if (err != I2X_SUCCESS) {
        while(1);
    }

    // data_read[0] now contains 0xAB
    while (1) {
        // Loop forever
    }
}
