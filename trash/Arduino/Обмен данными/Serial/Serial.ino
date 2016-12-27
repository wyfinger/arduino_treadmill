/*
  Обмен данными по серийному порту
  2015-12-02, Wyfinger
*/

String buff;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) 
  {
    char newChar = Serial.read();
    if (newChar != 13)
    {
      buff = buff + newChar;
    } else {
      String a1 = buff.substring(0, 5);
      String a2 = buff.substring(5, 9);
      String a3 = buff.substring(9, 13);
      Serial.println(a1);
      delay(10);
      Serial.println(a2);
      delay(10);
      Serial.println(a3);
      delay(10);
      buff = "";
    }
    
  }
}