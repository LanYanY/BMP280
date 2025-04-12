/*
 * BMP280������������ȡʾ��
 * �����ߣ�LanYanY
 * �������ڣ�2023
 *
 * ��ʾ��չʾ�����ʹ��BMP280���ȡ�¶ȡ���ѹ�ͺ���
 */

#include <BMP280.h>

// ����BMP280����ָ��SDA��SCL���ţ������������������ţ�
// Ĭ��I2C��ַ��0x76�������Ĵ�������ַ��0x77���������һ������ָ��
BMP280 bmp(2, 3);  // SDA=2, SCL=3
// BMP280 bmp(2, 3, 0x77);  // �����ַ��0x77

void setup() {
  Serial.begin(9600);
  Serial.println("BMP280����������");
  
  // ��ʼ��������
  if (!bmp.begin()) {
    Serial.println("�޷��ҵ�BMP280��������������ߣ�");
    while (1);
  }
  
  Serial.println("BMP280��������ʼ���ɹ���");
  
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