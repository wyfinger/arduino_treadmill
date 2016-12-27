#line 1 "C:/Treadmill/Arduino/iClebo2/iClebo2.ino"
#include <IRremote.h>

#include <Arduino.h>
#line 3
int RECV_PIN = 7; //вход ИК приемника
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  irrecv.enableIRIn(); // включить приемник
  pinMode(RECV_PIN, INPUT);
  digitalWrite(RECV_PIN, HIGH);
  Serial.begin(115200);
}

void loop()
{
  if (irrecv.decode(&results))
  {
    Serial.println("decode_type: " + String(results.decode_type));
    Serial.println("panasonicAddress: " + String(results.panasonicAddress));
    Serial.println("value: " + String(results.value));
    Serial.println("bits: " + String(results.bits));
    Serial.println();
    irrecv.resume();
  }
}
