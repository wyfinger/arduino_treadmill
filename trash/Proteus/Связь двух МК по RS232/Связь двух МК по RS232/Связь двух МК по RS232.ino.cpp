#line 1 "C:/Treadmill/Proteus/Связь двух МК по RS232/TestA.ino"
#include <Arduino.h>
#line 1
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("B>");
  delay(1330);
}
