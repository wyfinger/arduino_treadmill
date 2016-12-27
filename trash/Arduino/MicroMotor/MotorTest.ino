/********************************************************
 * Wyfinger
 ********************************************************/

int intReadValue;
int intCurrValue;
int intWriteValue;

void setup()
{
  pinMode(3, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  // читаем значение с АЦП 01
  intReadValue = analogRead(A0);
  if(abs(intReadValue-intCurrValue) > 10) {
    intCurrValue = intReadValue;
    intWriteValue = map(intCurrValue, 0, 1024, 0, 255);
    Serial.println("read: " + String(intCurrValue) + ", write: " + String(intWriteValue));
    // пишем значение в ШИМ порт
    analogWrite(3, intWriteValue);
  } 
}
