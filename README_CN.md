# BMP280 Arduino���I2C��

����һ������BMP280��ѹ��������Arduino�⣬ʹ�����I2Cͨ�š�������Ӳ��I2C�ӿڣ�����ʹ�������������������봫����ͨ�š�

## �����ص�

- ʹ�����I2Cͨ�ţ������ӵ�������������
- ��ȡ�¶ȣ����϶ȣ�
- ��ȡ��ѹ��Pa��
- ������Ժ��θ߶ȣ��ף�
- ֧�ֲ�ͬ�Ĺ���������
- �Զ�У׼���¶Ȳ���

## ��װ

### ͨ��Arduino�������

1. ��Arduino IDE
2. ����˵�: ��ͼ > ����� > �����...
3. ��������������"BMP280 SoftI2C"
4. �����װ

### �ֶ���װ

1. ���ش˿��ZIP�ļ�
2. ��Arduino IDE
3. ����˵�: ��ͼ > ����� > ���.ZIP��...
4. ѡ�����ص�ZIP�ļ�

## ʹ�÷���

```cpp
#include <BMP280.h>

// ����BMP280����ָ��SDA��SCL���ţ������������������ţ�
// Ĭ��I2C��ַ��0x76�������Ĵ�������ַ��0x77���������һ������ָ��
BMP280 bmp(2, 3);  // SDA=2, SCL=3
// BMP280 bmp(2, 3, 0x77);  // �����ַ��0x77

void setup() {
  Serial.begin(9600);
  
  // ��ʼ��������
  if (!bmp.begin()) {
    Serial.println("�޷��ҵ�BMP280��������������ߣ�");
    while (1);
  }
  
  // �ȴ��������ȶ�
  delay(1000);
}

void loop() {
  // ��ȡ�¶�
  float temperature = bmp.readTemperature();
  
  // ��ȡ��ѹ
  float pressure = bmp.readPressure();
  
  // ���㺣��
  float altitude = bmp.readAltitude();
  
  // ��ʾ����
  Serial.print("�¶�: ");
  Serial.print(temperature);
  Serial.println(" ��C");
  
  Serial.print("��ѹ: ");
  Serial.print(pressure / 100.0F);  // ת��Ϊ����
  Serial.println(" hPa");
  
  Serial.print("����: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  Serial.println();
  
  delay(2000);
}
```

## ���߷�ʽ

BMP280��������Arduino�ĵ������ӣ�

| BMP280 | Arduino |
|--------|---------|
| VCC    | 3.3V    |
| GND    | GND     |
| SDA    | �û���������ţ�����2�� |
| SCL    | �û���������ţ�����3�� |

## ʾ��

- **BasicReading**: �������¶ȡ���ѹ�ͺ��ζ�ȡ
- **WeatherMonitor**: �߼�ʾ����׷����ѹ�仯��Ԥ����������

## �ο�����

- [BMP280�����ֲ�](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)

## ����

LanYanY

## ���

MIT���֤ 