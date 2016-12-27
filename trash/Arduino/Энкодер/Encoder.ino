/********************************************************
 * Wyfinger, 2015-10-25
 * Чтерие скорости с энкодера дорожки
 ********************************************************/

#define PIN_COUNTER 0       // Пин, на котором весит фоторезистор, INT0
#define COUNTER_WINDOW 100 // Окно замера отсчетов энкодера, ms

volatile unsigned long intRpmCounter = 0; // volatile чтобы не оптимизировалась, т.к. используется в прерывании
unsigned long intLastMills = 0;
int intLastRpm = 0;
int relayPin = 4;
int motorPin = 5;
int inputPin = A3;

// Увеличение счетчика RPM по прерыванию
void IncRpm()
{
  intRpmCounter++;
}

// Считаем обороты мотора
int CalcRpm()
{
  int rez = intLastRpm;
  if (millis() - intLastMills >= COUNTER_WINDOW)
  {
    // на время подсчета отключим прерывание
    detachInterrupt(PIN_COUNTER);
    rez = intRpmCounter;
    intRpmCounter = 0;
    intLastRpm = rez;
    intLastMills = millis();
    attachInterrupt(PIN_COUNTER, IncRpm, FALLING);
  }
  return rez;
}

// Инициализация
void setup()
{
  Serial.begin(9600);
  // Подтягивающий резистор
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  // Цепляем прерывание для расчета RPM
  attachInterrupt(PIN_COUNTER, IncRpm, FALLING);

  // Реле
  digitalWrite(relayPin, HIGH);
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
}

// Главный цикл
void loop()
{
  analogWrite(motorPin, map(analogRead(inputPin), 0, 1024, 0, 100));

  Serial.println(CalcRpm());
  
  delay(300);
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