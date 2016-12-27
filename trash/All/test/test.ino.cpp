#line 1 "C:/Treadmill/All/test/test.ino"
/*
  Дисплей и панель управления беговой дорожкой.
  
  2015-12-19, Wyfinger
 */

#define Buttons    A4
#define Beeper     13  // пока светодиод

#include <Arduino.h>
void CheckButtons();
#line 10
long ButtonStamp;
long ButtonDownStamp;
int  ButtonNo;
int  ButtonNoPrev;

bool BeeperStatus;

void setup()
{
  pinMode(Buttons, INPUT);//_PULLUP);
  //digitalWrite(Buttons, HIGH);
  pinMode(Beeper, OUTPUT);
  Serial.begin(115200);
}

// контроль нажатия на кнопки клавиатуры
void CheckButtons()
{
  if (millis() - ButtonStamp > 100)
  {
    int Val = analogRead(Buttons);
    
    ButtonNo = 0;
    if ((Val > 65) && (Val < 195))  ButtonNo = 1;   // SET
    if ((Val > 195) && (Val < 320))  ButtonNo = 2;  // START
    if ((Val > 320) && (Val < 445))  ButtonNo = 3;  // DOWN
    if ((Val > 445) && (Val < 592))  ButtonNo = 4;  // UP
    if ((Val > 592) && (Val < 777))  ButtonNo = 5;  // RESET
    if ((Val > 777) && (Val < 1000))  ButtonNo = 6; // SELECT

    if ((ButtonNoPrev == 0) && (ButtonNo > 0))  // момент нажатия на кнопку
    {
      ButtonDownStamp = millis();
      ButtonNoPrev = ButtonNo;
    }
    if ((ButtonNoPrev > 0) && (ButtonNo == 0))  // момент отпускания кнопки
    {
      
    }
    ButtonStamp = millis();
  }

  if ((millis() - ButtonDownStamp > 250) && (ButtonNo > 0))  // кнопка нажата дольше 100 мс.
  {
    Serial.println(String(ButtonNo, DEC));
    BeeperStatus = !BeeperStatus;
    ButtonDownStamp = millis();  // отжимаем кнопку
  }
}


void loop() {
  //CheckButtons(); 
  //digitalWrite(Beeper, BeeperStatus); 
  Serial.println(analogRead(Buttons));
  delay(250);
}

