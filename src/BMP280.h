/*
 * BMP280.h - 软件I2C驱动的BMP280传感器库
 * 创建者：LanYanY
 * 创建日期：2023
 * 
 * 这个库提供了使用软件I2C与BMP280气压传感器通信的功能
 */

#ifndef BMP280_H
#define BMP280_H

#include <Arduino.h>
#include <stdint.h>

// BMP280寄存器地址
#define BMP280_REG_CHIPID  0xD0
#define BMP280_REG_RESET   0xE0
#define BMP280_REG_STATUS  0xF3
#define BMP280_REG_CTRL    0xF4
#define BMP280_REG_CONFIG  0xF5
#define BMP280_REG_PRESS   0xF7
#define BMP280_REG_TEMP    0xFA
#define BMP280_REG_CALIB   0x88

// BMP280芯片ID
#define BMP280_CHIPID      0x58

// 校准参数存储结构
typedef struct {
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;
} bmp280_calib_data;

class BMP280 {
public:
  BMP280(uint8_t sda_pin, uint8_t scl_pin, uint8_t address = 0x76);
  
  // 初始化传感器
  bool begin();
  
  // 读取温度，返回摄氏度
  float readTemperature();
  
  // 读取气压，返回Pa
  float readPressure();
  
  // 读取海拔，返回米
  float readAltitude(float seaLevelPressure = 1013.25);

private:
  uint8_t _sda_pin;
  uint8_t _scl_pin;
  uint8_t _address;
  
  // 温度补偿值
  int32_t t_fine;
  
  // 校准数据
  bmp280_calib_data _calib;
  
  // I2C通信函数
  void i2c_start();
  void i2c_stop();
  void i2c_write_bit(bool bit);
  bool i2c_read_bit();
  bool i2c_write_byte(uint8_t byte);
  uint8_t i2c_read_byte(bool ack);
  bool i2c_write_register(uint8_t reg, uint8_t value);
  bool i2c_write_register_block(uint8_t reg, uint8_t *buffer, uint8_t length);
  uint8_t i2c_read_register(uint8_t reg);
  void i2c_read_register_block(uint8_t reg, uint8_t *buffer, uint8_t length);
  
  // 延时函数
  void i2c_delay();
  
  // 读取校准数据
  void readCalibrationData();
};

#endif // BMP280_H 