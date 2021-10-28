/*!
 * @file easy_INA219.c
 *
 * twitter: @ArduinoEasy
 */

#include "INA219_LIB.h"


static void ina219_wireWriteRegister(uint8_t addr, uint8_t reg, uint16_t value);
static void ina219_wireReadRegister(uint8_t addr, uint8_t reg, uint16_t *value);
static int16_t ina219_getShuntVoltage_raw(INA219 *ina219, uint8_t ADDRESS);


void ina219_wireWriteRegister (uint8_t addr, uint8_t reg, uint16_t value){
	i2c_write2ByteRegister(addr, reg, value);
}

void ina219_wireReadRegister(uint8_t addr, uint8_t reg, uint16_t *value){
	*value = i2c_read2ByteRegister(addr, reg);
}

void ina219_setCalibration_32V_2A(INA219 *ina219, uint8_t ADDRESS){
  ina219->ina219_i2caddr = ADDRESS;
  ina219->ina219_calValue = 4096;
  ina219->ina219_currentDivider_mA = 10;  // Current LSB = 100uA per bit (1000/100 = 10)
  ina219->ina219_powerMultiplier_mW = 2;     // Power LSB = 1mW per bit (2/1)

  
  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CALIBRATION, ina219->ina219_calValue);
  
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CONFIG, config);
}

void ina219_setCalibration_32V_1A(INA219 *ina219, uint8_t ADDRESS){
  ina219->ina219_i2caddr = ADDRESS;
  ina219->ina219_calValue = 10240;
  ina219->ina219_currentDivider_mA = 25;      // Current LSB = 40uA per bit (1000/40 = 25)
  ina219->ina219_powerMultiplier_mW = 0.8f;         // Power LSB = 800mW per bit

  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CALIBRATION, ina219->ina219_calValue);

  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CONFIG, config);
}

void ina219_setCalibration_16V_400mA(INA219 *ina219, uint8_t ADDRESS) {
  ina219->ina219_i2caddr = ADDRESS;
  ina219->ina219_calValue = 8192;
  ina219->ina219_currentDivider_mA = 20;  // Current LSB = 50uA per bit (1000/50 = 20)
  ina219->ina219_powerMultiplier_mW = 1.0f;     // Power LSB = 1mW per bit

  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CALIBRATION, ina219->ina219_calValue);
  
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219_wireWriteRegister(ina219->ina219_i2caddr, INA219_REG_CONFIG, config);
}

void ina219_begin(INA219 *ina219, uint8_t ADDRESS_X) {
	ina219_setCalibration_16V_400mA(ina219, ADDRESS_X);
}

static int16_t ina219_getShuntVoltage_raw(INA219 *ina219, uint8_t ADDRESS) {
  ina219->ina219_i2caddr = ADDRESS;
  uint16_t value;
  ina219_wireReadRegister(ina219->ina219_i2caddr, INA219_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

float ina219_getShuntVoltage_mV(INA219 *ina219, uint8_t ADDRESS_X) {
  int16_t value;
  value = ina219_getShuntVoltage_raw(ina219, ADDRESS_X);
  return value * 0.01;
}
