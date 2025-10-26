#ifndef I2X_MASTER_H_
#define I2X_MASTER_H_

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#define IX_STATUS (IXSR & 0xF8)
#define IX_TIMEOUT 10000U

#define IX_SCL_PIN PORTC5
#define IX_SDA_PIN PORTC4
#define IX_SCL_MASK (1 << IX_SCL_PIN)
#define IX_SDA_MASK (1 << IX_SDA_PIN)


#define IX_ACK  true
#define IX_NACK false

#define IX_SLA_W(ADDR) ((ADDR << 1) | IX_WRITE)
#define IX_SLA_R(ADDR) ((ADDR << 1) | IX_READ)


typedef enum {
    I2X_SUCCESS = 0,
    I2X_ERR_START,
    I2X_ERR_SLA,
    I2X_ERR_DATA,
    I2X_ERR_TIMEOUT
} ret_code_t;

typedef enum {
    IX_FREQ_100K,
    IX_FREQ_250K,
    IX_FREQ_400K
} i2x_freq_mode_t;

void ix_init(i2x_freq_mode_t i2x_freq, bool pullup_en);
ret_code_t ix_master_transmit(uint8_t slave_addr, const uint8_t* p_data, uint8_t len, bool repeat_start);
ret_code_t ix_master_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len);

#endif /* I2X_MASTER_H_ */

