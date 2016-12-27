#line 1 "D:/SyncFolder/Treadmill/Arduino/Enc/EncoderMeter.ino"
/*
  Измерение количества щелчков энкодера,
  Щелчки записываем раз в переменную и отображаем
  в порт.
  2015-11-14, Wyfinger
*/

#include <Arduino.h>
void IncCounter();
#line 8
int pinEnc = 2;                           // Пин, на котором весит фоторезистор, INT1
volatile unsigned long intCounter = 0;    // счетчик
unsigned long intCounterLast = 0;


// Увеличение счетчика RPM по прерыванию
void IncCounter()
{
  intCounter++;  
}

// Инициализация
void setup()
{
  Serial.begin(9600);
  // Подтягивающий резистор
  pinMode(pinEnc, INPUT);
  digitalWrite(pinEnc, HIGH);
  // Цепляем прерывание для расчета RPM
  attachInterrupt(pinEnc, IncCounter, FALLING);
}

// Главный цикл
void loop()
{
  if (intCounter!=intCounterLast)
  {
    Serial.println(intCounter);
    intCounterLast = intCounter;
  }
  delay(1000);
}
