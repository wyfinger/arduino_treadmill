/*
  Измерение количества щелчков энкодера,
  Щелчки записываем раз в переменную и отображаем
  в порт.
  2015-11-14, Wyfinger
*/

volatile int intCounter = 0;
int intCounterLast = 0;

void Inerr()
{
  intCounter = intCounter + 1;
}

void setup() {
  Serial.begin(9600);
  //TCCR0B = TCCR0B & B11111000 | B00000010;
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
  attachInterrupt(1, Inerr, RISING);
}

void loop() {
  if (intCounter!=intCounterLast)
  {
    Serial.println(intCounter);
    intCounterLast = intCounter;
  }
}
