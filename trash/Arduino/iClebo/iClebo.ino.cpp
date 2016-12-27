#line 1 "C:/Treadmill/Arduino/iClebo/iClebo.ino"
#include <Arduino.h>
#line 1
byte level = 0;                        // Текущий уровень на входе
byte level_prev = 0;                   // Сохраненный уровень на входе

unsigned long last_up_time = 0;        // Таймер 1 - определяет длительность фронтов
unsigned long last_down_time = 0;      // Таймер 2 - определяет паузы между посылками

String array;                          // Здесь будет раскадровка

void setup() 
{
  pinMode(7, INPUT);                   // Сюда подаем сигнал от ИК-приемника TSOP
  digitalWrite(7, HIGH);               // Притягиваем ногу IR к плюсу

  Serial.begin(115200);
  Serial.println("READY");
  
  last_up_time = micros();             // Инициализируем таймеры
  last_down_time = micros();
}
 
void loop()
{

  level = digitalRead(7);                  // Считываем значение уровня на входе

  if(level != level_prev) {                         // Уровень изменился! Фронт импульса
  
    // Если давно не было изменений покажем что записали, и обнулим буфер
    if ((micros() - last_up_time) > 2000) 
    {
      Serial.println(array);
      array = "";
    }

    // Если считали 0, а раньше была 1 - это восходящий фронт
    if (level == 0)
    {
      array = array + " u" + String(micros() - last_down_time);
      last_up_time = micros();
    } else
    {
      array = array + " d" + String(micros() - last_up_time);
      last_down_time = micros();
    }

    level_prev = level;
  }
}
