/*
 * BMP280天气监测示例
 * 创建者：LanYanY
 * 创建日期：2023
 *
 * 此示例展示了如何使用BMP280创建简单的天气监测站
 * 它会追踪气压变化并预测天气趋势
 */

#include <BMP280.h>

// 创建BMP280对象
BMP280 bmp(2, 3);  // SDA=2, SCL=3

// 用于追踪气压变化
float previousPressure = 0;
float pressureDelta = 0;
unsigned long lastReadTime = 0;
const unsigned long readInterval = 60000;  // 每分钟读取一次

// 气压变化阈值（百帕）
const float PRESSURE_RISING_THRESHOLD = 0.5;   // 气压上升
const float PRESSURE_FALLING_THRESHOLD = -0.5;  // 气压下降

void setup() {
  Serial.begin(9600);
  Serial.println("BMP280天气监测站");
  
  // 初始化传感器
  if (!bmp.begin()) {
    Serial.println("无法找到BMP280传感器，请检查接线！");
    while (1);
  }
  
  Serial.println("BMP280传感器初始化成功！");
  
  // 初始化参考气压
  previousPressure = bmp.readPressure() / 100.0F;
  Serial.print("初始气压: ");
  Serial.print(previousPressure);
  Serial.println(" hPa");
  
  lastReadTime = millis();
}

void loop() {
  // 读取当前环境数据
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;  // 转换为百帕
  float altitude = bmp.readAltitude();
  
  // 显示当前读数
  Serial.print("温度: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("气压: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  
  Serial.print("估计海拔: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  // 计算并显示气压变化（每分钟）
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readInterval) {
    // 计算气压变化
    pressureDelta = pressure - previousPressure;
    
    Serial.print("过去一分钟气压变化: ");
    Serial.print(pressureDelta);
    Serial.println(" hPa");
    
    // 天气趋势预测
    predictWeather(pressureDelta);
    
    // 更新参考值
    previousPressure = pressure;
    lastReadTime = currentTime;
  }
  
  // 等待一段时间
  delay(5000);
}

// 基于气压变化预测天气
void predictWeather(float pressureChange) {
  Serial.print("天气趋势: ");
  
  if (pressureChange > PRESSURE_RISING_THRESHOLD) {
    Serial.println("气压上升 - 天气可能转好");
  } else if (pressureChange < PRESSURE_FALLING_THRESHOLD) {
    Serial.println("气压下降 - 天气可能恶化");
  } else {
    Serial.println("气压稳定 - 天气可能维持不变");
  }
} 