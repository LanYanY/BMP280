# BMP280 Arduino Software I2C Library

[÷–Œƒ∞Ê README](README_CN.md)

This is an Arduino library for the BMP280 barometric pressure sensor using software I2C communication. It does not rely on hardware I2C interface and can use any two digital pins to communicate with the sensor.

## Features

- Software I2C communication, can connect to any digital pins
- Temperature reading (°„C)
- Pressure reading (Pa)
- Altitude calculation (meters)
- Support for different oversampling settings
- Automatic calibration and temperature compensation

## Installation

### Via Arduino Library Manager

1. Open Arduino IDE
2. Go to menu: Sketch > Include Library > Manage Libraries...
3. Search for "BMP280 SoftI2C"
4. Click Install

### Manual Installation

1. Download the ZIP file of this library
2. Open Arduino IDE
3. Go to menu: Sketch > Include Library > Add .ZIP Library...
4. Select the downloaded ZIP file

## Usage

```cpp
#include <BMP280.h>

// Create BMP280 object, specify SDA and SCL pins (can be any digital pins)
// Default I2C address is 0x76, if your sensor is at 0x77, specify it as the last parameter
BMP280 bmp(2, 3);  // SDA=2, SCL=3
// BMP280 bmp(2, 3, 0x77);  // If address is 0x77

void setup() {
  Serial.begin(9600);
  
  // Initialize sensor
  if (!bmp.begin()) {
    Serial.println("Could not find BMP280 sensor, check wiring!");
    while (1);
  }
  
  // Wait for sensor to stabilize
  delay(1000);
}

void loop() {
  // Read temperature
  float temperature = bmp.readTemperature();
  
  // Read pressure
  float pressure = bmp.readPressure();
  
  // Calculate altitude
  float altitude = bmp.readAltitude();
  
  // Display readings
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °„C");
  
  Serial.print("Pressure: ");
  Serial.print(pressure / 100.0F);  // Convert to hPa
  Serial.println(" hPa");
  
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  Serial.println();
  
  delay(2000);
}
```

## Wiring

Typical connection of BMP280 sensor to Arduino:

| BMP280 | Arduino |
|--------|---------|
| VCC    | 3.3V    |
| GND    | GND     |
| SDA    | User-defined pin (e.g., 2) |
| SCL    | User-defined pin (e.g., 3) |

## Examples

- **BasicReading**: Basic temperature, pressure, and altitude reading
- **WeatherMonitor**: Advanced example that tracks pressure changes and predicts weather trends

## References

- [BMP280 Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)

## Author

LanYanY

## License

MIT License 