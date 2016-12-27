#line 1 "C:/Treadmill/Display/Firmware/display.ino"
/*
  Дичплей и панель управления беговой дорожкой.
  
  2015-12-19, Wyfinger
 */

#define Q1  13       // выходы выбора цифры
#define Q2  12
#define Q3  11
#define Q4  10
#define Q5  9
#define Q6  8
#define Q7  7
#define Q8  6

#define SeriesA  5  // декодеры
#define SeriesB  4

#define Clicker  3
#define DSPReset  2

#include <Arduino.h>
byte EncodeNum(byte Value);
void ShowNum(byte Num);
#line 22
byte Num;

void setup() {
  // инициализация выходов
  pinMode(Q1, OUTPUT);
  pinMode(Q2, OUTPUT);
  pinMode(Q3, OUTPUT);
  pinMode(Q4, OUTPUT);
  pinMode(Q5, OUTPUT);
  pinMode(Q6, OUTPUT);
  pinMode(Q7, OUTPUT);
  pinMode(Q8, OUTPUT);
  pinMode(SeriesA, OUTPUT);
  pinMode(SeriesB, OUTPUT);
  pinMode(Clicker, OUTPUT);
  pinMode(DSPReset, OUTPUT);

  Num = EncodeNum(5);
}

// преобразвание числа в битовую маску для 7 сегметнов
byte EncodeNum(byte Value)
{
  switch (Value) 
  {
    case 0 : return 0x7E;
    case 1 : return 0x30;
    case 2 : return 0x6D;
    case 3 : return 0x79;
    case 4 : return 0x33;
    case 5 : return 0x5B;
    case 6 : return 0x5F;
    case 7 : return 0x70;
    case 8 : return 0x7F;
    case 9 : return 0x7B;
  }               
                  
}                 

// отобразить цифру
void ShowNum(byte Num)
{

  byte EncodedNum = EncodeNum(Num);
  // сбрысываем декодеры
  digitalWrite(DSPReset, HIGH);
  digitalWrite(DSPReset, LOW);

  // отсчитываем нужное значение
  for (int i = 0; i<=7; i++)
  {
    bool b = bitRead(EncodedNum, i);
    digitalWrite(SeriesA, b);
    digitalWrite(Clicker, HIGH);
    digitalWrite(SeriesA, LOW);
  }
}

void loop() {
  ShowNum(5);

}

