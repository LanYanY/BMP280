# BMP280 Arduino软件I2C库

这是一个用于BMP280气压传感器的Arduino库，使用软件I2C通信。不依赖硬件I2C接口，可以使用任意两个数字引脚与传感器通信。

## 功能特点

- 使用软件I2C通信，可连接到任意数字引脚
- 读取温度（摄氏度）
- 读取气压（Pa）
- 计算相对海拔高度（米）
- 支持不同的过采样设置
- 自动校准和温度补偿

## 安装

### 通过Arduino库管理器

1. 打开Arduino IDE
2. 点击菜单: 草图 > 导入库 > 管理库...
3. 在搜索框中输入"BMP280 SoftI2C"
4. 点击安装

### 手动安装

1. 下载此库的ZIP文件
2. 打开Arduino IDE
3. 点击菜单: 草图 > 导入库 > 添加.ZIP库...
4. 选择下载的ZIP文件

## 使用方法

```cpp
#include <BMP280.h>

// 创建BMP280对象，指定SDA和SCL引脚（可以是任意数字引脚）
// 默认I2C地址是0x76，如果你的传感器地址是0x77，请在最后一个参数指定
BMP280 bmp(2, 3);  // SDA=2, SCL=3
// BMP280 bmp(2, 3, 0x77);  // 如果地址是0x77

void setup() {
  Serial.begin(9600);
  
  // 初始化传感器
  if (!bmp.begin()) {
    Serial.println("无法找到BMP280传感器，请检查接线！");
    while (1);
  }
  
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
```

## 接线方式

BMP280传感器与Arduino的典型连接：

| BMP280 | Arduino |
|--------|---------|
| VCC    | 3.3V    |
| GND    | GND     |
| SDA    | 用户定义的引脚（例如2） |
| SCL    | 用户定义的引脚（例如3） |

## 示例

- **BasicReading**: 基本的温度、气压和海拔读取
- **WeatherMonitor**: 高级示例，追踪气压变化并预测天气趋势

## 参考资料

- [BMP280数据手册](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)

## 作者

LanYanY

## 许可

MIT许可证 