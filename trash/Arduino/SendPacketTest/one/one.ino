#include <EasyTransfer.h>

String ReciveBuff = "";

void setup() {
	Serial.begin(9600);
}

// отправка сообщения на силовой модуль
void SendPacket(char Code, int Value)
{
  byte buff[10];

  buff[0] = 'O';          // это означает, что пакет от силового модуля на дисплей
  buff[1] = Code;
  // считаем значения в виде чисел
  buff[2] = (byte)(Value*pow(10,-3)) % 10;
  buff[3] = (byte)(Value*pow(10,-2)) % 10;
  buff[4] = (byte)(Value*pow(10,-1)) % 10;
  buff[5] = (byte)(Value*pow(10,-0)) % 10;
  buff[6] = buff[0] ^ buff[1] ^ buff[2] ^ buff[3] ^ buff[4] ^ buff[5];
  // преобразуем их в строки
  buff[2] = String(buff[2], DEC)[0];
  buff[3] = String(buff[3], DEC)[0];
  buff[4] = String(buff[4], DEC)[0];
  buff[5] = String(buff[5], DEC)[0];
  String Hex = "0"+String(buff[6], HEX);
  Hex = Hex.substring(Hex.length()-2);
  buff[6] = Hex[0];
  buff[7] = Hex[1];
  // перенос строки
  buff[8] = '\r';
  buff[9] = '\n';
  Serial.write(buff, sizeof(buff));
}

char Direction;
char Code;
int Value;
String hexCrc;
byte Crc;
byte RealCrc;

// проверка получения сообщений от силового модуля
void RecivePacket()
{
  if (Serial.available() > 0)
  {
    char newChar = Serial.read();
    if (newChar != '\n')
    {
      ReciveBuff = ReciveBuff + newChar;
    } else // закончилась строка, возможно это валидный пакет
    {
      Direction = ReciveBuff[0];  // направление передаци данных, должно быть 'P'
      Code = ReciveBuff[1];
      Value = 1000*(ReciveBuff[2]-0x30) + 100*(ReciveBuff[3]-0x30) + 10*(ReciveBuff[4]-0x30) + 1*(ReciveBuff[5]-0x30);

      // вот это задротство ниже чтобы конвертировать одит байт HEX -> Byte
      hexCrc = "0x"+ReciveBuff[6]+ReciveBuff[7];
      Crc = (int)strtol(&hexCrc[0],NULL,16);
      RealCrc = Direction ^ Code ^ (ReciveBuff[2]-0x30) ^ (ReciveBuff[3]-0x30) ^ (ReciveBuff[4]-0x30) ^ (ReciveBuff[5]-0x30);

      if ((Direction == 'T') && (Crc == RealCrc))  // команду в работу !
      {
        switch (Code)
        {
          case 'A':    // акселерометр сообщает угол наклона дорожки (сообщается раз в 5 секунд)
          {
            digitalWrite(13, 1);
          }
          case 'E':    // какая-то ошибка, возможно перегрев, хотя датчик температуры я пока не цеплял
          {
            digitalWrite(13, 0);
          }
          case 'T':    // температура радиатора силового модуля (сообщается раз в 10 секунд)
          {
            //CurrTemperature = Value;
          }
        }
      }
      ReciveBuff = "";
    }
  }
}

void loop() {
  SendPacket('A', 99);
  RecivePacket();
  delay(700);
}