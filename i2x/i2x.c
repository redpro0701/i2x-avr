#include "i2x.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define IX_WAIT_READY(timeout) \
{ \
    uint16_t _tmo = timeout; \
    while (!(TWCR & (1 << TWINT)) && --_tmo); \
    if (_tmo == 0) return I2X_ERR_TIMEOUT; \
}

static ret_code_t ix_start(void)
{
#if DEBUG_LOG
    printf("Send START condition...\n");
#endif
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    IX_WAIT_READY(IX_TIMEOUT);

    uint8_t status = TWSR & 0xF8;
    if (status != TW_START && status != TW_REP_START)
        return I2X_ERR_START;

#if DEBUG_LOG
    printf("START OK\n");
#endif
    return I2X_SUCCESS;
}

static void ix_stop(void)
{
#if DEBUG_LOG
    puts("Send STOP condition");
#endif
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    _delay_us(10);
}

static ret_code_t ix_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    IX_WAIT_READY(IX_TIMEOUT);

    uint8_t status = TWSR & 0xF8;
    if (status != TW_MT_DATA_ACK && status != TW_MT_SLA_ACK)
        return I2X_ERR_DATA;

    return I2X_SUCCESS;
}

static ret_code_t ix_read(uint8_t* data, bool ack)
{
    if (!data) return I2X_ERR_DATA;

    if (ack)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    else
        TWCR = (1 << TWINT) | (1 << TWEN);

    IX_WAIT_READY(IX_TIMEOUT);

    uint8_t status = TWSR & 0xF8;
    if ((ack && status != TW_MR_DATA_ACK) || (!ack && status != TW_MR_DATA_NACK))
        return I2X_ERR_DATA;

    *data = TWDR;
    return I2X_SUCCESS;
}

void ix_init(i2x_freq_mode_t i2x_freq, bool pullup_en)
{
    if (pullup_en)
        PORTC |= IX_SDA_MASK | IX_SCL_MASK;
    else
        PORTC &= ~(IX_SDA_MASK | IX_SCL_MASK);

    DDRC &= ~(IX_SDA_MASK | IX_SCL_MASK);

    uint32_t scl_freq = 100000;
    switch (i2x_freq)
    {
        case IX_FREQ_100K: scl_freq = 100000; break;
        case IX_FREQ_250K: scl_freq = 250000; break;
        case IX_FREQ_400K: scl_freq = 400000; break;
        default: break;
    }

    // Calculate TWBR for given F_CPU and desired SCL frequency
    TWBR = ((F_CPU / scl_freq) - 16) / 2;
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // Prescaler = 1
}

ret_code_t ix_master_transmit(uint8_t slave_addr, const uint8_t* p_data, uint8_t len, bool repeat_start)
{
    ret_code_t err;

    err = ix_start();
    if (err != I2X_SUCCESS) return err;

    err = ix_write((slave_addr << 1) | 0); // Write mode
    if (err != I2X_SUCCESS) return err;

    for (uint8_t i = 0; i < len; i++)
    {
        err = ix_write(p_data[i]);
        if (err != I2X_SUCCESS) return err;
    }

    if (!repeat_start)
        ix_stop();

    return I2X_SUCCESS;
}

ret_code_t ix_master_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len)
{
    ret_code_t err;

    err = ix_start();
    if (err != I2X_SUCCESS) return err;

    err = ix_write((slave_addr << 1) | 1); // Read mode
    if (err != I2X_SUCCESS) return err;

    for (uint8_t i = 0; i < len; i++)
    {
        err = ix_read(&p_data[i], i < (len - 1) ? IX_ACK : IX_NACK);
        if (err != I2X_SUCCESS) return err;
    }

    ix_stop();
    return I2X_SUCCESS;
}
