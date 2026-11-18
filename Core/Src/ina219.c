#include "ina219.h"

/* Configuration:
 *
 * BRNG = 32V
 * PGA  = /8 => 320mV
 * BADC = 12bit
 * SADC = 12bit
 * MODE = Shunt + Bus Continuous
 */
#define INA219_CONFIG_VALUE    0x399F

/* Calibration value for:
 * Shunt = 0.1 Ohm
 * Max Current = 2A
 */
#define INA219_CALIBRATION_VALUE 4096

static HAL_StatusTypeDef INA219_WriteRegister(
    INA219_HandleTypeDef *ina,
    uint8_t reg,
    uint16_t value)
{
    uint8_t data[2];

    data[0] = (value >> 8) & 0xFF;
    data[1] = value & 0xFF;

    return HAL_I2C_Mem_Write(
        ina->hi2c,
        ina->address,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        2,
        HAL_MAX_DELAY);
}

static HAL_StatusTypeDef INA219_ReadRegister(
    INA219_HandleTypeDef *ina,
    uint8_t reg,
    uint16_t *value)
{
    uint8_t data[2];

    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(
        ina->hi2c,
        ina->address,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        2,
        HAL_MAX_DELAY);

    if(status != HAL_OK)
        return status;

    *value = ((uint16_t)data[0] << 8) | data[1];

    return HAL_OK;
}

HAL_StatusTypeDef INA219_Init(
    INA219_HandleTypeDef *ina,
    I2C_HandleTypeDef *hi2c)
{
    ina->hi2c = hi2c;
    ina->address = INA219_ADDRESS;

    /*
     * Current_LSB = 100uA
     * Power_LSB = 20 * Current_LSB
     */
    ina->currentLSB = 0.1f;   /* mA */
    ina->powerLSB = 2.0f;     /* mW */

    if(INA219_WriteRegister(
           ina,
           INA219_REG_CALIBRATION,
           INA219_CALIBRATION_VALUE)
       != HAL_OK)
    {
        return HAL_ERROR;
    }

    return INA219_WriteRegister(
        ina,
        INA219_REG_CONFIG,
        INA219_CONFIG_VALUE);
}

float INA219_GetBusVoltage_V(INA219_HandleTypeDef *ina)
{
    uint16_t raw;

    if(INA219_ReadRegister(
           ina,
           INA219_REG_BUSVOLTAGE,
           &raw) != HAL_OK)
    {
        return 0.0f;
    }

    raw >>= 3;

    return raw * 0.004f;
}

float INA219_GetShuntVoltage_mV(INA219_HandleTypeDef *ina)
{
    int16_t raw;

    if(INA219_ReadRegister(
           ina,
           INA219_REG_SHUNTVOLTAGE,
           (uint16_t*)&raw) != HAL_OK)
    {
        return 0.0f;
    }

    return raw * 0.01f;
}

float INA219_GetCurrent_mA(INA219_HandleTypeDef *ina)
{
    int16_t raw;

    if(INA219_ReadRegister(
           ina,
           INA219_REG_CURRENT,
           (uint16_t*)&raw) != HAL_OK)
    {
        return 0.0f;
    }

    return raw * ina->currentLSB;
}

float INA219_GetPower_mW(INA219_HandleTypeDef *ina)
{
    uint16_t raw;

    if(INA219_ReadRegister(
           ina,
           INA219_REG_POWER,
           &raw) != HAL_OK)
    {
        return 0.0f;
    }

    return raw * ina->powerLSB;
}


