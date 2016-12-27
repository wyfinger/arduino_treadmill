#line 1 "C:/Treadmill/Arduino/MotorControl/MotorControl.ino"
/*
  Управление скоростью вращения мотора.
  Читаем аналоговое значение из A0 и с помощью ПИД
  регулирования выдаем управляющее воздействие на ногу 5.
  Обороты мотора считаем по прерыванию энкодера на ноге 3 (INT0).
  При значении на A0 >= 20 (из 1023) включаем реле на ноге 2.
  
  2015-12-01, Wyfinger
*/

#include <PID_v1.h>

#define pinIn      A3  // Нога на которой висит потенциометр
#define pinEncoder  2  // Нога на которой висит энкодер
#define intEncoder  0  // INT0, соотвтетствует D2
#define pinOut      5  // D5
#define pinRelay    4  // D2

// Параметры ПИД-регулятора
#include <Arduino.h>
void IncCounter();
bool CalcSpeed();
#line 20
double Input, Output, Setpoint;  // эти параметры в км/час
double consKp=0.6, consKi=0.4, consKd=0.3;

PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

volatile int intCounter = 0;  // Счетчик скорости полотна, 530 тиков на метр

// Подстроечные коэффициенты
int intRPMWindow = 100;  // время расчета скорости полотна, мс
int intEncTickByMeter = 530;  // тиков энкодера на метр
double MinSpeed = 1.0;
double MaxSpeed = 6.0;
double StepLimit = 0.1;
int MinOut = 5;   // с лампочкой в цепи мотор начинает вращаться где-то на 100
int MaxOut = 60;  // пока ограничу выходное воздействие на мотор
int FreqFactor = 1;

// Служебные переменные
unsigned long intLastMillsPWM;
unsigned long intLastMillsDSP;
double fCurrSpeed;  // скорость, замеренная с энкодера
double fNeedSpeed;  // скорость, требуемая по показаниям потенциометра
int intOut;

int intEncWindow = 50; // микросекунд
long intEncSumm = 0;
long intEncCount = 0;
long intLastEnc = 0;
bool boolEncState = false;
bool boolEncStatePrev = false;

String buff;

// Обработчик прерывания энкодера
void IncCounter() 
{
  intCounter++;
}

// Вернет True когда посчитаются обороты
bool CalcSpeed()
{
  bool rez = false;
  if (millis() - intLastMillsPWM >= intRPMWindow*FreqFactor)
  {
    // на время подсчета отключим прерывание
    detachInterrupt(intEncoder);
    // 1m/s = 3.6km/h
    fCurrSpeed = 3.6 * ((double(intCounter) / double(intEncTickByMeter)) / double(0.001 * intRPMWindow)); // считаем скорость
    intCounter = 0;
    intLastMillsPWM = millis();
    rez = true;
    attachInterrupt(intEncoder, IncCounter, RISING);    
  }
  return rez;
}

void setup()
{
  // Установка предделителей частоты, чтобы поднять частоту ШИМ
  // на пятой ноге ШИМ должен стать с частотой 7812.50 Гц
  TCCR0B = TCCR0B & B11111000 | B00000010;
  FreqFactor = 8;
  // Подтягиваем ногу энкодера к плюсу, т.к. фототранзистор энкодера
  // включен на ноль
  pinMode(pinEncoder, INPUT);
  digitalWrite(pinEncoder, HIGH);
  // Прерывание для энкодера
  attachInterrupt(intEncoder, IncCounter, RISING);
  // Режим остальных ног
  pinMode(pinOut, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  // ПИД-регулятор
  Setpoint = 0;
  myPID.SetMode(AUTOMATIC);
  // Связь для отладки и дисплея
  Serial.begin(115200);

  digitalWrite(pinRelay, HIGH);  // !!!!!!!!!!!!!!!!!
}

// Главный цикл
void loop()
{
  // фильтрация помех на энкодере
  /*if (digitalRead(pinEncoder))
  {
    intEncSumm++;                                        // считаем все значения на входе
  }
  intEncCount++;
  if (micros() - intLastEnc >= intEncWindow*FreqFactor)   // окно в 50 микросекунд
  {    
    boolEncState = ((intEncSumm / intEncCount) > 0.5);    // берем среднее за окно
    intEncSumm = 0;
    intEncCount = 0;
    if ((boolEncState == true) && (boolEncStatePrev == false))  // если пришла 1, а раньше был 0 - увеличиваем счетчик оборотов
    {
      intCounter++;                              
    }
    intLastEnc = micros();        
    boolEncStatePrev = boolEncState;
  }
  // подсчет скорости
  if (millis() - intLastMillsPWM >= intRPMWindow*FreqFactor)
  {
    fCurrSpeed = 3.6 * ((double(intCounter) / double(intEncTickByMeter)) / double(0.001 * intRPMWindow)); // считаем скорость
    intCounter = 0;
    intLastMillsPWM = millis();
  }
  */
  // читаем требуемую скорость от 0 до MaxSpeed по потенциометру
  fNeedSpeed = double(map(analogRead(pinIn), 0, 1023, 0, round(MaxSpeed*100))) / 100;
  // включаем реле только если задана скорость больше MinSpeed
  //digitalWrite(pinRelay, fNeedSpeed > 0.2);

  if(CalcSpeed()) 
  {  // Если посчиталась скорость корректируем управляющее воздействие
  	Input = fCurrSpeed;
  	Setpoint = fNeedSpeed;
    myPID.SetTunings(consKp, consKi, consKd);
    double prewOut = Output;
    myPID.Compute();
    if ((Output - prewOut) > StepLimit)  Output = prewOut + StepLimit;     // Чтобы быстро не разгонялся
    if ((Output - prewOut) < -StepLimit)  Output = prewOut - StepLimit;     // Чтобы быстро не разгонялся    
    if (Output > 10) Output = 10;
    intOut  = map(Output*100, 0, round(100*MaxSpeed), MinOut, MaxOut);
    if (intOut > MaxOut) intOut = MaxOut;

    analogWrite(pinOut, intOut);
    //analogWrite(pinOut, map(analogRead(pinIn), 0, 1023, 0, 150));   // DEBUG  !!!

    // ОТЛАДКА
    if (Serial.available() > 0) 
    {
      char newChar = Serial.read();
      if (newChar != 13)
      {
        buff = buff + newChar;
      } else
      { 

        String sMinSpeed = buff.substring(0, 5);
        String sMaxSpeed = buff.substring(6, 10);

        String sMinOut = buff.substring(11, 14);
        String sMaxOut = buff.substring(15, 18);

        String sPeriod = buff.substring(19, 23);

        String sKp = buff.substring(24, 28);
        String sKi = buff.substring(29, 33);
        String sKd = buff.substring(34, 38);

        String sLimit = buff.substring(39, 42);

        Serial.println("sMinSpeed=[" + sMinSpeed + "]\r\n"+
          "sMaxSpeed=[" + sMaxSpeed + "]\r\n"+
          "sMinOut=[" + sMinOut + "]\r\n"+
          "sMaxOut=[" + sMaxOut + "]\r\n"+
          "sPeriod=[" + sPeriod + "]\r\n"+
          "sKp=[" + sKp + "]\r\n"+
          "sKi=[" + sKi + "]\r\n"+
          "sKd=[" + sKd + "]\r\n"+
          "sStepLimit=[" + sLimit + "]");
        delay(1);

        MinSpeed = sMinSpeed.toFloat();        
        MaxSpeed = sMaxSpeed.toFloat();
        
        MinOut = sMinOut.toInt();
        MaxOut = sMaxOut.toInt();

        intRPMWindow = sPeriod.toInt();

        consKp = sKp.toFloat();
        consKi = sKi.toFloat();        
        consKd = sKd.toFloat();

        StepLimit = sLimit.toFloat();

        /*String kp = buff.substring(0, 5);
        String ki = buff.substring(5, 9);
        String kd = buff.substring(9, 13);
        Serial.println(kp.toFloat());
        consKp = kp.toFloat();
        delay(1);
        Serial.println(ki.toFloat());
        consKi = ki.toFloat();
        delay(1);
        Serial.println(kd.toFloat());
        consKd = kd.toFloat();
        delay(1);*/
        buff = "";
      }
    }

    Serial.print("N: ");
    Serial.print(fNeedSpeed);
    Serial.print(", C: ");
    Serial.print(fCurrSpeed);
    Serial.print(", S: ");
    Serial.print(Output);
    Serial.print(", V: ");
    Serial.print(intOut);
    Serial.println();
    intLastMillsDSP = millis();
  }
}
