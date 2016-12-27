#line 1 "D:/SyncFolder/Treadmill/Arduino/Мотор с энкодером/MotorEncoder.ino"
/*
  Тест мотора
  Читаем аналоговое значение из A0 и выводим на ногу D5
  отмасштабированное значение. При зрачении на A0 >= 3
  включаем реле на ноге D2
  2015-11-04, Wyfinger
*/

#include <Arduino.h>
#line 9
int inputPin = A0;
int ledPin = 13;
int relayPin = 2;
int motorPin = 5;

int inputValue;
int outputValue;

void setup() {
  // Установка предделителей частоты, чтобы поднять частоту ШИМ
  // на пятой ноге ШИМ должен стать с частотой 7812.50 Гц
  TCCR0B = TCCR0B & B11111000 | B00000010;
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  //Serial.begin(9600);
}

void loop() {
  inputValue = analogRead(inputPin);
  digitalWrite(relayPin, inputValue > 10);
  digitalWrite(ledPin, inputValue > 10);
  outputValue = map(inputValue, 0, 1024, 0, 100);
  analogWrite(motorPin, outputValue);
  //Serial.println("Input: " + String(inputValue) + ", Output: " + String(outputValue));
  //delay(100); // задержка, чтобы не щелкать в пограничном диапазоне
}
