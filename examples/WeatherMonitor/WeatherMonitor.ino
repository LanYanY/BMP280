/*
 * BMP280�������ʾ��
 * �����ߣ�LanYanY
 * �������ڣ�2023
 *
 * ��ʾ��չʾ�����ʹ��BMP280�����򵥵��������վ
 * ����׷����ѹ�仯��Ԥ����������
 */

#include <BMP280.h>

// ����BMP280����
BMP280 bmp(2, 3);  // SDA=2, SCL=3

// ����׷����ѹ�仯
float previousPressure = 0;
float pressureDelta = 0;
unsigned long lastReadTime = 0;
const unsigned long readInterval = 60000;  // ÿ���Ӷ�ȡһ��

// ��ѹ�仯��ֵ��������
const float PRESSURE_RISING_THRESHOLD = 0.5;   // ��ѹ����
const float PRESSURE_FALLING_THRESHOLD = -0.5;  // ��ѹ�½�

void setup() {
  Serial.begin(9600);
  Serial.println("BMP280�������վ");
  
  // ��ʼ��������
  if (!bmp.begin()) {
    Serial.println("�޷��ҵ�BMP280��������������ߣ�");
    while (1);
  }
  
  Serial.println("BMP280��������ʼ���ɹ���");
  
  // ��ʼ���ο���ѹ
  previousPressure = bmp.readPressure() / 100.0F;
  Serial.print("��ʼ��ѹ: ");
  Serial.print(previousPressure);
  Serial.println(" hPa");
  
  lastReadTime = millis();
}

void loop() {
  // ��ȡ��ǰ��������
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;  // ת��Ϊ����
  float altitude = bmp.readAltitude();
  
  // ��ʾ��ǰ����
  Serial.print("�¶�: ");
  Serial.print(temperature);
  Serial.println(" ��C");
  
  Serial.print("��ѹ: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  
  Serial.print("���ƺ���: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  // ���㲢��ʾ��ѹ�仯��ÿ���ӣ�
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readInterval) {
    // ������ѹ�仯
    pressureDelta = pressure - previousPressure;
    
    Serial.print("��ȥһ������ѹ�仯: ");
    Serial.print(pressureDelta);
    Serial.println(" hPa");
    
    // ��������Ԥ��
    predictWeather(pressureDelta);
    
    // ���²ο�ֵ
    previousPressure = pressure;
    lastReadTime = currentTime;
  }
  
  // �ȴ�һ��ʱ��
  delay(5000);
}

// ������ѹ�仯Ԥ������
void predictWeather(float pressureChange) {
  Serial.print("��������: ");
  
  if (pressureChange > PRESSURE_RISING_THRESHOLD) {
    Serial.println("��ѹ���� - ��������ת��");
  } else if (pressureChange < PRESSURE_FALLING_THRESHOLD) {
    Serial.println("��ѹ�½� - �������ܶ�");
  } else {
    Serial.println("��ѹ�ȶ� - ��������ά�ֲ���");
  }
} 