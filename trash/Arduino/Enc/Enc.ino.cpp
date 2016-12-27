#line 1 "C:/Treadmill/Arduino/Enc/EncoderMeter.ino"
/*
  Измерение количества щелчков энкодера,
  Щелчки записываем раз в переменную и отображаем
  в порт.
  2015-11-14, Wyfinger
*/

#include <Arduino.h>
void IncCounter();
#line 8
int intEnc = 1;                           // INT1
int pinEnc = 3;                           // pin 3
volatile int intCounter = 0;    // счетчик
unsigned long intCounterLast = 0;


// Увеличение счетчика RPM по прерыванию
void IncCounter()
{
  intCounter = intCounter + 1;
}

// Инициализация
void setup()
{
  // Установка предделителей частоты, чтобы поднять частоту ШИМ
  // на пятой ноге ШИМ должен стать с частотой 7812.50 Гц
  //TCCR0B = TCCR0B & B11111000 | B00000010;
  Serial.begin(9600);
  //Serial.println("I'am start");
  // Подтягивающий резистор
  //pinMode(pinEnc, INPUT);
  //digitalWrite(pinEnc, HIGH);
  // Цепляем прерывание для расчета RPM
  attachInterrupt(1, IncCounter, RISING);

  pinMode(13, INPUT);
  //digitalWrite(3, HIGH);
}

// Главный цикл
void loop()
{
//  int d3state = digitalRead(3);
 // Serial.println(d3state);
//  delay(500);

  //if (intCounter!=intCounterLast)
  //{
    Serial.println(intCounter);
  //  intCounterLast = intCounter;
  //}
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
}
