/*
 * BMP280.cpp - 软件I2C驱动的BMP280传感器库
 * 创建者：LanYanY
 * 创建日期：2023
 * 
 * 这个库提供了使用软件I2C与BMP280气压传感器通信的功能
 */

#include "BMP280.h"
#include <math.h>

// 构造函数
BMP280::BMP280(uint8_t sda_pin, uint8_t scl_pin, uint8_t address) {
  _sda_pin = sda_pin;
  _scl_pin = scl_pin;
  _address = address;
  t_fine = 0;
}

// 初始化传感器
bool BMP280::begin() {
  // 配置引脚为输出模式
  pinMode(_sda_pin, OUTPUT);
  pinMode(_scl_pin, OUTPUT);
  
  // 初始设置为高电平
  digitalWrite(_sda_pin, HIGH);
  digitalWrite(_scl_pin, HIGH);
  
  // 等待传感器启动
  delay(10);
  
  // 8个时钟脉冲以重置任何设备
  for (int i = 0; i < 8; i++) {
    digitalWrite(_scl_pin, HIGH);
    i2c_delay();
    digitalWrite(_scl_pin, LOW);
    i2c_delay();
  }

  // 检查设备ID
  uint8_t chipId = i2c_read_register(BMP280_REG_CHIPID);
  if (chipId != BMP280_CHIPID) {
    return false;
  }
  
  // 读取校准数据
  readCalibrationData();
  
  // 配置传感器 - 正常模式，16x过采样温度，16x过采样压力，标准IIR滤波器
  i2c_write_register(BMP280_REG_CONFIG, 0b10010000);  // t_sb=0.5ms, filter=16, spi3w_en=0
  i2c_write_register(BMP280_REG_CTRL, 0b10110111);    // osrs_t=16x, osrs_p=16x, mode=normal
  
  // 等待配置生效
  delay(100);
  
  return true;
}

// 读取温度
float BMP280::readTemperature() {
  int32_t adc_T = 0;
  int32_t var1, var2;
  
  // 读取原始温度值
  uint8_t buffer[3];
  i2c_read_register_block(BMP280_REG_TEMP, buffer, 3);
  
  adc_T = ((uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | buffer[2]) >> 4;
  
  // 根据校准参数计算温度
  var1 = ((((adc_T >> 3) - ((int32_t)_calib.dig_T1 << 1))) * ((int32_t)_calib.dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)_calib.dig_T1))) >> 12) * ((int32_t)_calib.dig_T3)) >> 14;
  
  t_fine = var1 + var2;
  float temperature = (t_fine * 5 + 128) >> 8;
  
  return temperature / 100.0f;
}

// 读取气压
float BMP280::readPressure() {
  int64_t var1, var2, p;
  int32_t adc_P = 0;
  
  // 先确保t_fine已经更新 - 如果还没有读取过温度，则现在读取
  if (t_fine == 0) {
    readTemperature();
  }
  
  // 读取原始气压值
  uint8_t buffer[3];
  i2c_read_register_block(BMP280_REG_PRESS, buffer, 3);
  
  adc_P = ((uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | buffer[2]) >> 4;
  
  // 根据校准参数计算气压
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_calib.dig_P3) >> 8) + ((var1 * (int64_t)_calib.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_calib.dig_P1) >> 33;
  
  if (var1 == 0) {
    return 0; // 避免除以零
  }
  
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)_calib.dig_P8) * p) >> 19;
  
  p = ((p + var1 + var2) >> 8) + (((int64_t)_calib.dig_P7) << 4);
  
  return (float)p / 256.0;
}

// 计算海拔
float BMP280::readAltitude(float seaLevelPressure) {
  float pressure = readPressure() / 100.0F; // 转换为百帕
  return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

// 读取校准数据
void BMP280::readCalibrationData() {
  uint8_t buffer[24];
  i2c_read_register_block(BMP280_REG_CALIB, buffer, 24);
  
  _calib.dig_T1 = (buffer[1] << 8) | buffer[0];
  _calib.dig_T2 = (buffer[3] << 8) | buffer[2];
  _calib.dig_T3 = (buffer[5] << 8) | buffer[4];
  
  _calib.dig_P1 = (buffer[7] << 8) | buffer[6];
  _calib.dig_P2 = (buffer[9] << 8) | buffer[8];
  _calib.dig_P3 = (buffer[11] << 8) | buffer[10];
  _calib.dig_P4 = (buffer[13] << 8) | buffer[12];
  _calib.dig_P5 = (buffer[15] << 8) | buffer[14];
  _calib.dig_P6 = (buffer[17] << 8) | buffer[16];
  _calib.dig_P7 = (buffer[19] << 8) | buffer[18];
  _calib.dig_P8 = (buffer[21] << 8) | buffer[20];
  _calib.dig_P9 = (buffer[23] << 8) | buffer[22];
}

// I2C通信基本函数实现
void BMP280::i2c_delay() {
  delayMicroseconds(5); // 调整延时以适应不同的微控制器频率
}

void BMP280::i2c_start() {
  digitalWrite(_sda_pin, HIGH);
  digitalWrite(_scl_pin, HIGH);
  i2c_delay();
  digitalWrite(_sda_pin, LOW);
  i2c_delay();
  digitalWrite(_scl_pin, LOW);
  i2c_delay();
}

void BMP280::i2c_stop() {
  digitalWrite(_sda_pin, LOW);
  i2c_delay();
  digitalWrite(_scl_pin, HIGH);
  i2c_delay();
  digitalWrite(_sda_pin, HIGH);
  i2c_delay();
}

void BMP280::i2c_write_bit(bool bit) {
  digitalWrite(_sda_pin, bit ? HIGH : LOW);
  i2c_delay();
  digitalWrite(_scl_pin, HIGH);
  i2c_delay();
  digitalWrite(_scl_pin, LOW);
  i2c_delay();
}

bool BMP280::i2c_read_bit() {
  pinMode(_sda_pin, INPUT);
  i2c_delay();
  digitalWrite(_scl_pin, HIGH);
  i2c_delay();
  bool bit = digitalRead(_sda_pin);
  digitalWrite(_scl_pin, LOW);
  i2c_delay();
  pinMode(_sda_pin, OUTPUT);
  return bit;
}

bool BMP280::i2c_write_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    i2c_write_bit(byte & 0x80);
    byte <<= 1;
  }
  
  // 接收ACK
  pinMode(_sda_pin, INPUT);
  i2c_delay();
  digitalWrite(_scl_pin, HIGH);
  i2c_delay();
  bool ack = (digitalRead(_sda_pin) == LOW);
  digitalWrite(_scl_pin, LOW);
  i2c_delay();
  pinMode(_sda_pin, OUTPUT);
  
  return ack;
}

uint8_t BMP280::i2c_read_byte(bool ack) {
  uint8_t byte = 0;
  
  pinMode(_sda_pin, INPUT);
  for (int i = 0; i < 8; i++) {
    byte <<= 1;
    i2c_delay();
    digitalWrite(_scl_pin, HIGH);
    i2c_delay();
    if (digitalRead(_sda_pin)) {
      byte |= 1;
    }
    digitalWrite(_scl_pin, LOW);
    i2c_delay();
  }
  pinMode(_sda_pin, OUTPUT);
  
  // 发送ACK或NACK
  i2c_write_bit(!ack);
  
  return byte;
}

bool BMP280::i2c_write_register(uint8_t reg, uint8_t value) {
  i2c_start();
  bool ack = i2c_write_byte((_address << 1) | 0); // 写地址
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(reg); // 寄存器地址
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(value); // 写入数据
  i2c_stop();
  
  return ack;
}

bool BMP280::i2c_write_register_block(uint8_t reg, uint8_t *buffer, uint8_t length) {
  i2c_start();
  bool ack = i2c_write_byte((_address << 1) | 0); // 写地址
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(reg); // 寄存器地址
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  for (uint8_t i = 0; i < length; i++) {
    ack = i2c_write_byte(buffer[i]); // 写入数据
    if (!ack) {
      i2c_stop();
      return false;
    }
  }
  
  i2c_stop();
  return true;
}

uint8_t BMP280::i2c_read_register(uint8_t reg) {
  i2c_start();
  i2c_write_byte((_address << 1) | 0); // 写地址
  i2c_write_byte(reg); // 寄存器地址
  i2c_start(); // 重复起始信号
  i2c_write_byte((_address << 1) | 1); // 读地址
  uint8_t value = i2c_read_byte(false); // 读一个字节，不需要ACK
  i2c_stop();
  
  return value;
}

void BMP280::i2c_read_register_block(uint8_t reg, uint8_t *buffer, uint8_t length) {
  i2c_start();
  i2c_write_byte((_address << 1) | 0); // 写地址
  i2c_write_byte(reg); // 寄存器地址
  i2c_start(); // 重复起始信号
  i2c_write_byte((_address << 1) | 1); // 读地址
  
  for (uint8_t i = 0; i < length - 1; i++) {
    buffer[i] = i2c_read_byte(true); // 读取字节，发送ACK
  }
  buffer[length - 1] = i2c_read_byte(false); // 最后一个字节，发送NACK
  
  i2c_stop();
} 