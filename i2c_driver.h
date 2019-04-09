#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

void I2cInit(void);
void I2cOpen(void);
void I2cClose(void);
uint32_t I2cReadData(uint8_t slave_addr, uint8_t *reg_addr, uint8_t *read_data_bffer, uint8_t read_data_len);
uint32_t I2cWriteData(uint32_t slave_addr, uint8_t *reg_addr, uint8_t *write_data, uint8_t data_len);

void I2c1Init(void);
void I2c1Open(void);
void I2c1Close(void);
uint32_t I2c1ReadData(uint8_t slave_addr, uint8_t *reg_addr, uint8_t *read_data_bffer, uint8_t read_data_len);
uint32_t I2c1WriteData(uint32_t slave_addr, uint8_t *reg_addr, uint8_t *write_data, uint8_t data_len);

#ifdef __cplusplus
}
#endif

#endif
