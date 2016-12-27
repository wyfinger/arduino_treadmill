int x, y, z;
 
// Калибровка датчика
int dx=296;
int dy=341;
int dz=350;
void setup()
{
   Serial.begin(9600);
}
 
void loop()
{
  // Значения осей с датчика
  x = analogRead(A0) - dx;
  y = analogRead(A1) - dy;
  z = analogRead(A2) - dz;
 
  // Вывод в Serial monitor
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);
 
  //Период опроса
  delay(100);
}