/*
 * BMP280传感器基本读取示例
 * 创建者：LanYanY
 * 创建日期：2023
 *
 * 此示例展示了如何使用BMP280库读取温度、气压和海拔
 */

#include <BMP280.h>

// 创建BMP280对象，指定SDA和SCL引脚（可以是任意数字引脚）
// 默认I2C地址是0x76，如果你的传感器地址是0x77，请在最后一个参数指定
BMP280 bmp(2, 3);  // SDA=2, SCL=3
// BMP280 bmp(2, 3, 0x77);  // 如果地址是0x77

void setup() {
  Serial.begin(9600);
  Serial.println("BMP280传感器测试");
  
  // 初始化传感器
  if (!bmp.begin()) {
    Serial.println("无法找到BMP280传感器，请检查接线！");
    while (1);
  }
  
  Serial.println("BMP280传感器初始化成功！");
  
  // 等待传感器稳定
  delay(1000);
}

void loop() {
  // 读取温度
  float temperature = bmp.readTemperature();
  
  // 读取气压
  float pressure = bmp.readPressure();
  
  // 计算海拔
  float altitude = bmp.readAltitude();
  
  // 显示读数
  Serial.print("温度: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("气压: ");
  Serial.print(pressure / 100.0F);  // 转换为百帕
  Serial.println(" hPa");
  
  Serial.print("海拔: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  Serial.println();
  
  delay(2000);
} 