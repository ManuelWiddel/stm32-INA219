#ifndef __INA219_H
#define __INA219_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32fxxx_hal.h"
#include <stdint.h>

/* INA219 I2C Address */
#define INA219_ADDRESS                 (0x40 << 1)

/* Registers */
#define INA219_REG_CONFIG              0x00
#define INA219_REG_SHUNTVOLTAGE        0x01
#define INA219_REG_BUSVOLTAGE          0x02
#define INA219_REG_POWER               0x03
#define INA219_REG_CURRENT             0x04
#define INA219_REG_CALIBRATION         0x05

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint16_t address;

    float currentLSB;
    float powerLSB;

} INA219_HandleTypeDef;

/* Public functions */
HAL_StatusTypeDef INA219_Init(INA219_HandleTypeDef *ina,
                              I2C_HandleTypeDef *hi2c);

float INA219_GetBusVoltage_V(INA219_HandleTypeDef *ina);
float INA219_GetShuntVoltage_mV(INA219_HandleTypeDef *ina);
float INA219_GetCurrent_mA(INA219_HandleTypeDef *ina);
float INA219_GetPower_mW(INA219_HandleTypeDef *ina);

#ifdef __cplusplus
}
#endif

#endif
