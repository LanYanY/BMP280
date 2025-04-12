/*
 * BMP280.cpp - ���I2C������BMP280��������
 * �����ߣ�LanYanY
 * �������ڣ�2023
 * 
 * ������ṩ��ʹ�����I2C��BMP280��ѹ������ͨ�ŵĹ���
 */

#include "BMP280.h"
#include <math.h>

// ���캯��
BMP280::BMP280(uint8_t sda_pin, uint8_t scl_pin, uint8_t address) {
  _sda_pin = sda_pin;
  _scl_pin = scl_pin;
  _address = address;
  t_fine = 0;
}

// ��ʼ��������
bool BMP280::begin() {
  // ��������Ϊ���ģʽ
  pinMode(_sda_pin, OUTPUT);
  pinMode(_scl_pin, OUTPUT);
  
  // ��ʼ����Ϊ�ߵ�ƽ
  digitalWrite(_sda_pin, HIGH);
  digitalWrite(_scl_pin, HIGH);
  
  // �ȴ�����������
  delay(10);
  
  // 8��ʱ�������������κ��豸
  for (int i = 0; i < 8; i++) {
    digitalWrite(_scl_pin, HIGH);
    i2c_delay();
    digitalWrite(_scl_pin, LOW);
    i2c_delay();
  }

  // ����豸ID
  uint8_t chipId = i2c_read_register(BMP280_REG_CHIPID);
  if (chipId != BMP280_CHIPID) {
    return false;
  }
  
  // ��ȡУ׼����
  readCalibrationData();
  
  // ���ô����� - ����ģʽ��16x�������¶ȣ�16x������ѹ������׼IIR�˲���
  i2c_write_register(BMP280_REG_CONFIG, 0b10010000);  // t_sb=0.5ms, filter=16, spi3w_en=0
  i2c_write_register(BMP280_REG_CTRL, 0b10110111);    // osrs_t=16x, osrs_p=16x, mode=normal
  
  // �ȴ�������Ч
  delay(100);
  
  return true;
}

// ��ȡ�¶�
float BMP280::readTemperature() {
  int32_t adc_T = 0;
  int32_t var1, var2;
  
  // ��ȡԭʼ�¶�ֵ
  uint8_t buffer[3];
  i2c_read_register_block(BMP280_REG_TEMP, buffer, 3);
  
  adc_T = ((uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | buffer[2]) >> 4;
  
  // ����У׼���������¶�
  var1 = ((((adc_T >> 3) - ((int32_t)_calib.dig_T1 << 1))) * ((int32_t)_calib.dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)_calib.dig_T1))) >> 12) * ((int32_t)_calib.dig_T3)) >> 14;
  
  t_fine = var1 + var2;
  float temperature = (t_fine * 5 + 128) >> 8;
  
  return temperature / 100.0f;
}

// ��ȡ��ѹ
float BMP280::readPressure() {
  int64_t var1, var2, p;
  int32_t adc_P = 0;
  
  // ��ȷ��t_fine�Ѿ����� - �����û�ж�ȡ���¶ȣ������ڶ�ȡ
  if (t_fine == 0) {
    readTemperature();
  }
  
  // ��ȡԭʼ��ѹֵ
  uint8_t buffer[3];
  i2c_read_register_block(BMP280_REG_PRESS, buffer, 3);
  
  adc_P = ((uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | buffer[2]) >> 4;
  
  // ����У׼����������ѹ
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_calib.dig_P3) >> 8) + ((var1 * (int64_t)_calib.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_calib.dig_P1) >> 33;
  
  if (var1 == 0) {
    return 0; // ���������
  }
  
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)_calib.dig_P8) * p) >> 19;
  
  p = ((p + var1 + var2) >> 8) + (((int64_t)_calib.dig_P7) << 4);
  
  return (float)p / 256.0;
}

// ���㺣��
float BMP280::readAltitude(float seaLevelPressure) {
  float pressure = readPressure() / 100.0F; // ת��Ϊ����
  return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

// ��ȡУ׼����
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

// I2Cͨ�Ż�������ʵ��
void BMP280::i2c_delay() {
  delayMicroseconds(5); // ������ʱ����Ӧ��ͬ��΢������Ƶ��
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
  
  // ����ACK
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
  
  // ����ACK��NACK
  i2c_write_bit(!ack);
  
  return byte;
}

bool BMP280::i2c_write_register(uint8_t reg, uint8_t value) {
  i2c_start();
  bool ack = i2c_write_byte((_address << 1) | 0); // д��ַ
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(reg); // �Ĵ�����ַ
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(value); // д������
  i2c_stop();
  
  return ack;
}

bool BMP280::i2c_write_register_block(uint8_t reg, uint8_t *buffer, uint8_t length) {
  i2c_start();
  bool ack = i2c_write_byte((_address << 1) | 0); // д��ַ
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  ack = i2c_write_byte(reg); // �Ĵ�����ַ
  if (!ack) {
    i2c_stop();
    return false;
  }
  
  for (uint8_t i = 0; i < length; i++) {
    ack = i2c_write_byte(buffer[i]); // д������
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
  i2c_write_byte((_address << 1) | 0); // д��ַ
  i2c_write_byte(reg); // �Ĵ�����ַ
  i2c_start(); // �ظ���ʼ�ź�
  i2c_write_byte((_address << 1) | 1); // ����ַ
  uint8_t value = i2c_read_byte(false); // ��һ���ֽڣ�����ҪACK
  i2c_stop();
  
  return value;
}

void BMP280::i2c_read_register_block(uint8_t reg, uint8_t *buffer, uint8_t length) {
  i2c_start();
  i2c_write_byte((_address << 1) | 0); // д��ַ
  i2c_write_byte(reg); // �Ĵ�����ַ
  i2c_start(); // �ظ���ʼ�ź�
  i2c_write_byte((_address << 1) | 1); // ����ַ
  
  for (uint8_t i = 0; i < length - 1; i++) {
    buffer[i] = i2c_read_byte(true); // ��ȡ�ֽڣ�����ACK
  }
  buffer[length - 1] = i2c_read_byte(false); // ���һ���ֽڣ�����NACK
  
  i2c_stop();
} 