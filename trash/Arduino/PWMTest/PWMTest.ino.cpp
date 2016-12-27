#line 1 "C:/Treadmill/Arduino/PWMTest/PWMTest.ino"
/********************************************************
 * Wyfinger
 ********************************************************/

#include <PID_v1.h>

#define PIN_RPM_COUNTER 0  // Пин, которым считаем обороты мотора INT0
#define PIN_REGULATOR A0   // Пин, на котором висит управляющий потенциометр
#define PIN_OUTPUT 3       // Пин, куда выдаем выходной ШИМ, D3
#define PIN_LED_SET 12
#define PIN_LED_RESET 11

#define CONST_RPM_CALC_TIME 100 // Время окна для расчета RPM

#include <Arduino.h>
void IncRpm();
bool CalcRpm();
void ResetNumber();
void ShowNumber(int n);
#line 15
int intRpmCounter = 0;
long intRpmValue = 0;
int intLastMills = 0;
int intLastMillsDsp = 0;
int intLastLedValue = 0;

double Input, Output, Setpoint;

// Коэффициенты ПИД регулятора
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1.1, consKi=0.4, consKd=0.3;

PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);


// Увеличение счетчика RPM по прерыванию
void IncRpm()
{
  intRpmCounter++;
}

// Считаем обороты мотора
bool CalcRpm()
{
  bool rez = false;
  if (millis() - intLastMills >= CONST_RPM_CALC_TIME)
  {
    // на время подсчета отключим прерывание
    detachInterrupt(PIN_RPM_COUNTER);
    intRpmValue = round(intRpmCounter * 600 / CONST_RPM_CALC_TIME);
    intRpmCounter = 0;
    intLastMills = millis();
    attachInterrupt(PIN_RPM_COUNTER, IncRpm, FALLING);
    rez = true;
  }
  return rez;
}

// Сброс счетчика драйвера LED дисплея
void ResetNumber()
{
    // Для сброса на мгновение ставим контакт
    // reset в HIGH и возвращаем обратно в LOW
    digitalWrite(PIN_LED_RESET, HIGH);
    digitalWrite(PIN_LED_RESET, LOW);
}

void ShowNumber(int n)
{

    if (n != intLastLedValue) {
      intLastLedValue = n;
      // Первым делом обнуляем текущее значение
      ResetNumber();
  
      // Далее быстро «прокликиваем» счётчик до нужного
      // значения
      while (n--) {
          digitalWrite(PIN_LED_SET, HIGH);
          digitalWrite(PIN_LED_SET, LOW);
      }

    }
}

// Инициализация
void setup()
{
  //Serial.begin(9600);
  // Цепляем прерывание для расчета RPM
  attachInterrupt(PIN_RPM_COUNTER, IncRpm, FALLING);
  //
  Setpoint = map(analogRead(PIN_REGULATOR), 0, 1023, 0, 255);
  myPID.SetMode(AUTOMATIC);
  //
  pinMode(13, PIN_OUTPUT);
  //
  pinMode(PIN_LED_SET, PIN_OUTPUT);
  pinMode(PIN_LED_RESET, PIN_OUTPUT);
}

// Главный цикл
void loop()
{
  if(CalcRpm()) {
    Input = intRpmValue;
    Setpoint = map(analogRead(PIN_REGULATOR), 0, 1023, 0, 255);
    myPID.SetTunings(consKp, consKi, consKd);
    myPID.Compute();
    analogWrite(PIN_OUTPUT, round(Output));
    int po = digitalRead(13);
    po = !po;
    digitalWrite(13, po);
  }
  if((millis() - intLastMillsDsp) >= 1500)
  {
    //Serial.println("RPM Value is " + String(intRpmValue));
    //Serial.println("Set Point is " + String(Setpoint));
    //Serial.println("Output is " + String(Output));
    //Serial.println("");
    intLastMillsDsp = millis();
    ShowNumber(intRpmValue);
  }
}


/*
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void setup()
{
  //initialize the variables we're linked to
  Input = analogRead(PIN_INPUT);
  Setpoint = 100;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  Input = analogRead(PIN_INPUT);

  double gap = abs(Setpoint-Input); //distance away from setpoint
  if (gap < 10)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  myPID.Compute();
  analogWrite(PIN_OUTPUT, Output);
}


*/
