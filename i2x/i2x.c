#include "i2x.h"
#include <util/delay.h>
#include <stdio.h>

#define IX_WAIT_READY(timeout) \
{ \
    uint16_t _tmo = timeout; \
    while (!(IXCR & (1 << IXINT)) && --_tmo); \
    if (_tmo == 0) return IXI_ERR_TIMEOUT; \
}

static ret_code_t ix_start(void)
{
#if DEBUG_LOG
    printf("Send START condition...\n");
#endif
    IXCR = (1 << IXINT) | (1 << IXEN) | (1 << IXSTA);
    IX_WAIT_READY(IX_TIMEOUT);

    if (IX_STATUS != IX_START && IX_STATUS != IX_REP_START)
        return IXI_ERR_START;

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
    IXCR = (1 << IXINT) | (1 << IXEN) | (1 << IXSTO);
    _delay_us(10);
}

static ret_code_t ix_write_sla(uint8_t sla)
{
#if DEBUG_LOG
    printf("Write SLA+R/W: 0x%02X\n", sla);
#endif
    IXDR = sla;
    IXCR = (1 << IXINT) | (1 << IXEN);
    IX_WAIT_READY(IX_TIMEOUT);

    if (IX_STATUS != IX_MT_SLA_ACK && IX_STATUS != IX_MR_SLA_ACK)
        return I2X_ERR_SLA;

#if DEBUG_LOG
    printf("SLA ACK OK\n");
#endif
    return I2X_SUCCESS;
}

static ret_code_t ix_write(uint8_t data)
{
#if DEBUG_LOG
    printf("Write DATA: 0x%02X\n", data);
#endif
    IXDR = data;
    IXCR = (1 << IXINT) | (1 << IXEN);
    IX_WAIT_READY(IX_TIMEOUT);

    if (IX_STATUS != IX_MT_DATA_ACK)
        return I2X_ERR_DATA;

#if DEBUG_LOG
    printf("DATA ACK OK\n");
#endif
    return I2X_SUCCESS;
}

static ret_code_t ix_read(uint8_t* data, bool ack)
{
    if (!data) return I2X_ERR_DATA;

    if (ack)
        IXCR = (1 << IXINT) | (1 << IXEN) | (1 << IXEA);
    else
        IXCR = (1 << IXINT) | (1 << IXEN);

    IX_WAIT_READY(IX_TIMEOUT);

    if ((ack && IX_STATUS != IX_MR_DATA_ACK) || (!ack && IX_STATUS != IX_MR_DATA_NACK))
        return I2X_ERR_DATA;

    *data = IXDR;

#if DEBUG_LOG
    printf("Read DATA: 0x%02X\n", *data);
#endif
    return I2X_SUCCESS;
}

void ix_init(ixi_freq_mode_t ixi_freq, bool pullup_en)
{
    if (pullup_en)
        PORTC |= IX_SDA_MASK | IX_SCL_MASK;
    else
        PORTC &= ~(IX_SDA_MASK | IX_SCL_MASK);

    DDRC &= ~(IX_SDA_MASK | IX_SCL_MASK);

    uint32_t scl_freq = 100000;
    switch (ixi_freq)
    {
        case IX_FREQ_100K: scl_freq = 100000; break;
        case IX_FREQ_250K: scl_freq = 250000; break;
        case IX_FREQ_400K: scl_freq = 400000; break;
        default: break;
    }
    IXBR = ((F_CPU / scl_freq) - 16) / 2;
    IXSR &= ~((1 << IXPS1) | (1 << IXPS0));
}

ret_code_t ix_master_transmit(uint8_t slave_addr, const uint8_t* p_data, uint8_t len, bool repeat_start)
{
    ret_code_t err;

    err = ix_start();
    if (err != I2X_SUCCESS) return err;

    err = ix_write_sla(IX_SLA_W(slave_addr));
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

    err = ix_write_sla(IX_SLA_R(slave_addr));
    if (err != I2X_SUCCESS) return err;

    for (uint8_t i = 0; i < len; i++)
    {
        err = ix_read(&p_data[i], i < (len - 1) ? IX_ACK : IX_NACK);
        if (err != I2X_SUCCESS) return err;
    }

    ix_stop();
    return I2X_SUCCESS;
}
