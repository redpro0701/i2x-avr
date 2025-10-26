# I2X (I2C) Master Driver

## Overview
Simple I2C master driver for AVR works for atmega2560,atmega328p **tested most others should work too**.  
Supports init, read, and write.

---

## Pins
- SDA: PC4  
- SCL: PC5  
- `IX_SDA_MASK` / `IX_SCL_MASK` for bit masks.

---

## Macros
| Macro | Meaning |
|-------|--------|
| `IX_SLA_W(addr)` | Slave address + write |
| `IX_SLA_R(addr)` | Slave address + read |
| `IX_ACK` / `IX_NACK` | Read acknowledge / not-ack |
| `IX_STATUS` | Bus status |
| `IX_TIMEOUT` | Max wait cycles |

---

## Types
- `ret_code_t` – return code: `I2X_SUCCESS`, `I2X_ERR_*`  
- `i2x_freq_mode_t` – frequency: `IX_FREQ_100K`, `IX_FREQ_250K`, `IX_FREQ_400K`

---

## Functions

### `ix_init(freq, pullup)`
Init I2C pins & bus.  
```c
ix_init(IX_FREQ_100K, true);
