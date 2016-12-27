#line 1 "D:/Program Files/arduino-1.6.5-r5/examples/03.Analog/AnalogInput/AnalogInput.ino"
/*
  Тест реле
  Читаем аналоговое значение из A0 и если значение > 512
  выдаем еденицу на D2
  На ноге A0 весит потекциометр 1k как делитель напряжения 0-5V
  На ноге D2 через NPN транзистор весит реле
  2015-11-04, Wyfinger
*/

#include <Arduino.h>
#line 10
int inputPin = A0;
int outPin = 2;

int inputValue;

void setup() {
  pinMode(outPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  inputValue = analogRead(inputPin);
  digitalWrite(outPin, inputValue > 512);
  Serial.println(inputValue);
  delay(500); // задержка, чтобы не щелкать в пограничном диапазоне
}
