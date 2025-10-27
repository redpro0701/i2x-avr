#ifndef I2X_H_
#define I2X_H_

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#define I2X_STATUS (IXSR & 0xF8)
#define I2X_TIMEOUT 10000U

#define I2X_SCL_PIN PORTC5
#define I2X_SDA_PIN PORTC4
#define I2X_SCL_MASK (1 << I2X_SCL_PIN)
#define I2X_SDA_MASK (1 << I2X_SDA_PIN)


#define I2X_ACK  true
#define I2X_NACK false

#define I2X_SLA_W(ADDR) ((ADDR << 1) | I2X_WRITE)
#define I2X_SLA_R(ADDR) ((ADDR << 1) | I2X_READ)


typedef enum {
    I2X_SUCCESS = 0,
    I2X_ERR_START,
    I2X_ERR_SLA,
    I2X_ERR_DATA,
    I2X_ERR_TIMEOUT
} ret_code_t;

typedef enum {
    I2X_FREQ_100K,
    I2X_FREQ_250K,
    I2X_FREQ_400K
} i2x_freq_mode_t;

void i2x_init(i2x_freq_mode_t i2x_freq, bool pullup_en);
ret_code_t i2x_transmit(uint8_t slave_addr, const uint8_t* p_data, uint8_t len, bool repeat_start);
ret_code_t i2x_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len);

#endif /* I2X_H_ */


