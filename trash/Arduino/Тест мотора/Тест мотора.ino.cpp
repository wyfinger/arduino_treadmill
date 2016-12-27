#line 1 "C:/Treadmill/Arduino/Тест мотора/MotorTest.ino"
/*
  Тест мотора
  Читаем аналоговое значение из A3 и выводим на ногу D5
  масштабированное значение. При значении на A3 >= 3
  включаем реле на ноге D2
  2015-11-04, Wyfinger
*/

#include <Arduino.h>
#line 9
int inputPin = A3;
int ledPin = 13;
int relayPin = 4;
int motorPin = 5;
int motorLoad;

int inputValue;
int outputValue;

void setup() {
  // Установка предделителей частоты, чтобы поднять частоту ШИМ
  // на пятой ноге ШИМ должен стать с частотой 7812.50 Гц
  TCCR0B = TCCR0B & B11111000 | B00000010;
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);
}

void loop() {
  inputValue = analogRead(inputPin);
  digitalWrite(relayPin, inputValue > 10);
  digitalWrite(ledPin, inputValue > 10);
  outputValue = map(inputValue, 0, 1024, 0, 255);
  analogWrite(motorPin, outputValue);
  if (abs(motorLoad - outputValue) > 1) 
  {
    Serial.println("Input: " + String(inputValue) + ", Output: " + String(outputValue));
    delay(1);
    motorLoad = outputValue;
  }
}
