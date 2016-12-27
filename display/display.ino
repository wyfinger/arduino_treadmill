#include <SoftwareSerial.h>
/*
  Дисплей и панель управления беговой дорожкой.

  2015-12-19, Wyfinger
 */

#include <EEPROM.h>
#include <EasyTransfer.h>

#define Q1          9    // выходы выбора разряда дисплеев
#define Q2         11
#define Q3         12
#define Q4         A1
#define Q5         13
#define Q6         A0
#define Q7          8
#define Q8         10

#define Buttons    A4    // кнопки (все кнопки через делитель напряжения, висят на одной АЦП ноге)

#define Gercone    A3    // геркон безопасности, 0 если замкнут, нужно притянуть в + в коде
#define Beeper     A2    // пищалка со встроенным генератором, нужно только 1 подать

#define SeriesA     5    // декодеры, входы (A,B)
#define SeriesB     4

#define Clicker     6    // кликер 74HC164, по восходящему фронту проталкивает бит из SeriesA в буфер
#define DSPReset    7    // из SeriesB

#define PulseMeter A5    // нога на которой висит датчик пульса, датчик показывает каждый удар сердца, т.е.
  // нужно считать частоту как с энкодера, есть еще датчики пульса, которые дают аналоговое значение от 0 до 5V, поддержку
  // таких датчиков тоже можно будет приделать потом, если ноги свободные останутся.

#define Dights      8    // разрядов в каждой серии


const byte Q[Dights] = {Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8};

String P[] = {                  // программы занятий, скорость*10 на минуту, можно довольно много программ забить
  "2<<<<<<<<2",                 // 10 минут, 0.97 км.
  "2<FFFF<<FFFFF<2",            // 15 минут, 1.62 км.
  "2<FKPUPF<<FKPZ_ddd_ZPKF<2",  // 25 минут, 3.20 км.  
  "27<AFKPUZZZPF<2",            // 15 минут, 1.78 км.
  "2FZddd_ZUPKFA<2",            // 15 минут, 1.97 км.
  ""                      // ТЕСТОВАЯ
};

byte PPos = 0;  // номер минуты в программе тренировки
long CheckProgramTime = 0; //

const float MinSpeed = 1.0;
const float MaxSpeed = 10.0;

byte LineA[Dights]; // байты символов на дисплее, первая и вторая линия
byte LineB[Dights];
byte CurrDight = 0; // текущий отображаемый разряд

byte ProgramNo = 0;  // последняя использованная программа тренировки, сохраняется в EEPROM

enum {dmStop, dmProgramSelect, dmManualRun, dmProgramRun, dmError} DisplayMode = dmStop;
bool AltView = false;  // режим альтернативного отображения - включается кнопкой 'SET', вместо калорий
         // отображается угол наклона полотна дорожки, а если работаем в режиме dmProgramRun - вместо
         // времени и дистанции отображается отсавшееся время и дистанция в соответствии с текущей программой

float CurrSpeed = 0;       // текущая скорость
int   CurrTime = 0;        // время
float CurrCalorie = 0;     // затраченные калории
float CurrDistance = 0;    // дистанция
int   CurrPulse = 0;       // пульс
int   CurrAngle = 0;      // угол наклона полотна, в десятых градуса
int   CurrTemperature = 0; // температура радиатора силового модуля

long StartTime = 0;


long ButtonStamp;       // время последней проверки состояния кнопок, чтобы слишком часто не опрашивать
long ButtonDownStamp;   // время с момента нажатия на кнопку, от дребезга контактов
int  ButtonNo;          // номер нажатой кнопки
int  ButtonNoPrev;

enum {pbOther = 0, pbUp, pbDown} PreviosButton;  // это чтобы бикать по нажатию кнопок только при первом нажатии
long BeeperTime;        // время включения пищалки, чтобы ограничить длительность писка
bool BeeperStatus;      // писчалка включена?
long BeeperCount;       // сколько раз пискнуть, при кажном писке уменьшается на 1


enum Errors {ecNormal = 0, ecGercone, ecTermal};
Errors ErrorCode = ecNormal;  // Состояние (код ошибки)

bool PulseLastStatus;  // последнее состояние с датчика пульса, чтобы отлавливать фронт
long PulseLastTime;    // время крайнего удара сердца
byte PulseAveraging[20];   // окно усреднения пульса
long PulseStamp;   // время с момента поледнего изменения пульса, чтобы часто значение на дисплее не менялось

long CheckTimeLast = 0;
long CheckSpeedStamp = 0; // время последней отправки скорости на силовой модуль

bool StoppingMode = false;  // если true - режим остановка и каждые 3 сек. сбрасываем скорость на 1 км/час
long StoppingModeStamp = 0;

EasyTransfer ETin, ETout;  // объект EasyTransfer для обмена сообщениями с силовым модулем

struct PacketStruct {
  char Direction;   // направление передачи: D - от дисплея, P - от силового модуля
  char Code;        // код сообщения
  long Value;       // данные, в зависимости от кода сообщения
};

PacketStruct RxData;  // пакет, полученный от силового модуля
PacketStruct TxData;  // пакет для отпрвки на силовой модуль

// DBG

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
  // выключим все индикаторы
  digitalWrite(Q1, HIGH);
  digitalWrite(Q2, HIGH);
  digitalWrite(Q3, HIGH);
  digitalWrite(Q4, HIGH);
  digitalWrite(Q5, HIGH);
  digitalWrite(Q6, HIGH);
  digitalWrite(Q7, HIGH);
  digitalWrite(Q8, HIGH);
  // режим аналоговых ножек кнопок и геркона
  pinMode(Buttons, INPUT);  // PULLUP); - переделал схему, нога кнопок притянутя к 0, поэтому внутренний резистор не нужен
  pinMode(Gercone, INPUT_PULLUP);
  pinMode(Beeper, OUTPUT);
  // последняя используемая программа тренировки
  ProgramNo = EEPROM.read(0);
  if (ProgramNo >= sizeof(P))  ProgramNo = sizeof(P)-1;  // TODO:  что это за херня???
  // подготовка дисплея
  PrepareDisplay();
  // связь с силовым модулем
  Serial.begin(9600);
  ETin.begin(details(RxData), &Serial);
  ETout.begin(details(TxData), &Serial);
  //
  BeeperStatus = false;
  PreviosButton = pbOther;
  // заполним окно усреднения пульса нулями
  for (int i = 0; i<sizeof(PulseAveraging); i++)
  {
    PulseAveraging[i] = 0;
  }
}

// преобразование числа в битовую маску для 7 сегментов
byte EncodeNum(byte Num)
{
  switch (Num)
  {
    case 0 : return 0x3F;
    case 1 : return 0x06;
    case 2 : return 0x5B;
    case 3 : return 0x4F;
    case 4 : return 0x66;
    case 5 : return 0x6D;
    case 6 : return 0x7D;
    case 7 : return 0x07;
    case 8 : return 0x7F;
    case 9 : return 0x6F;
  }
  return Num;
}

// Исправление моего косяка с разводкой цифр секунд на плате
byte CheckSecBug(byte Num)
{
  byte Virgin = Num;
  byte dot  =  Num & B10000000;
  byte g    =  Num & B01000000;
  byte abc  = (Num & B00111000) >> 3;
  byte def  = (Num & B00000111) << 3;

  return (dot | def | abc | g);
}

// добавляем точку к числу
byte AddDot(byte Num)
{
  return Num | B10000000;
}

// отобразить цифру на декодере
void ShowNum(byte NumA, byte NumB)
{
  // сбрасываем декодеры
  digitalWrite(DSPReset, LOW);
  digitalWrite(DSPReset, HIGH);
  // отсчитываем нужное значение
  for (int i = 7; i>=0; i--)
  {
    digitalWrite(SeriesA, !bitRead(NumA, i));
    digitalWrite(SeriesB, !bitRead(NumB, i));
    // кликер
    digitalWrite(Clicker, HIGH);
    digitalWrite(Clicker, LOW);
  }
}

// выделяем значение разряда, >=0 - до запятой, <0 - после запятой
byte GetDight(float Num, int Dight)
{
  return (int)(Num*pow(10,-Dight)) % 10;
}

// подготовка данных для дисплея в режиме выбора программы тренировки
void PrepareDisplayInProgramSelect()
{
  // отобразим символ 'P' и номер программы, вместо скорости,
  // а также время и дистанцию, заложенную в программу
  int ProgTime = ProgramTime(P[ProgramNo]);
  float ProgDistance = ProgramDistance(P[ProgramNo]);
  // номер программы
  LineA[0] = 0x73;   // символ 'P'
  LineA[1] = GetDight(ProgramNo+1, 1);
  if(LineA[1] > 0)  LineA[1] = EncodeNum(LineA[1]);
  LineA[2] = EncodeNum(GetDight(ProgramNo+1, 0));
  // время
  LineA[6] = EncodeNum(GetDight(ProgTime / 60, 1));
  LineA[7] = AddDot(EncodeNum(GetDight(ProgTime / 60, 0)));
  LineB[6] = AddDot(EncodeNum(GetDight(ProgTime % 60, 1)));
  LineB[7] = EncodeNum(GetDight(ProgTime % 60, 0));
  // !!! Исправление моей ошибки с разводкой платы
  LineB[6] = CheckSecBug(LineB[6]);
  LineB[7] = CheckSecBug(LineB[7]);
  // дистанция
  if (ProgDistance < 1)  // до 1 км выводим два знака после запятой
  {
    LineA[3] = AddDot(EncodeNum(GetDight(ProgDistance, 0)));
    LineA[4] = EncodeNum(GetDight(ProgDistance, -1));
    LineA[5] = EncodeNum(GetDight(ProgDistance, -2));
  } else {           // а больше - только один
    LineA[3] = GetDight(ProgDistance, 1);
    if(LineA[3] > 0)  LineA[3] = EncodeNum(LineA[3]);
    LineA[4] = AddDot(EncodeNum(GetDight(ProgDistance, 0)));
    LineA[5] = EncodeNum(GetDight(ProgDistance, -1));
  }
  // очищаем остальные табло
  LineB[0] = 0;
  LineB[1] = 0;
  LineB[2] = 0;
  LineB[3] = 0;
  LineB[4] = 0;
  LineB[5] = 0;
}

// подготовка данных для дисплея в режиме ошибки
void PrepareDisplayInError()
{
  // отобразим символ 'E' и номер ошибки в табло скорости
  LineA[0] = 0x79;   // символ 'E'
  LineA[1] = 0;
  LineA[2] = EncodeNum(ErrorCode);
  // очищаем остальные табло
  for (int i=3; i<Dights; i++)
  {
    LineA[i] = 0;
    LineB[i] = 0;
  }
  for (int i = 0; i<3; i++)
  {
    LineB[i] = 0;
  }
}

// подготовка данных для дисплея в режиме ошибки
void PrepareDisplayInAltView()
{
  // скорость
  LineA[0] = GetDight(CurrSpeed, 1);
  if(LineA[0] > 0)  LineA[0] = EncodeNum(LineA[0]);  // не отображаем ноль в начале
  LineA[1] = AddDot(EncodeNum(GetDight(CurrSpeed, 0)));
  LineA[2] = EncodeNum(GetDight(CurrSpeed, -1));

  float Distance = CurrDistance;
  int Time = CurrTime;
  if (DisplayMode == dmProgramRun) // режим работы по программе - отображаем
  {
    // дистанция
    Distance = ProgramDistance(P[ProgramNo]) - CurrDistance;
    // время, минуты
    Time = ProgramTime(P[ProgramNo]) - CurrTime;
  }
  // дистанция
  if (Distance < 1)  // до 1 км выводим два знака после запятой
  {
    LineA[3] = AddDot(EncodeNum(GetDight(Distance, 0)));
    LineA[4] = EncodeNum(GetDight(Distance, -1));
    LineA[5] = EncodeNum(GetDight(Distance, -2));
  } else {           // а больше - только один
    LineA[3] = GetDight(Distance, 1);
    if(LineA[3] > 0)  LineA[3] = EncodeNum(LineA[3]);
    LineA[4] = AddDot(EncodeNum(GetDight(Distance, 0)));
    LineA[5] = EncodeNum(GetDight(Distance, -1));
  }
  // время, минуты
  LineA[6] = EncodeNum(GetDight(Time / 60, 1));
  LineA[7] = EncodeNum(GetDight(Time / 60, 0));
  if (Time % 2 == 0)  LineA[7] = AddDot(LineA[7]);
  // в окне калорий отображаем угол наклона
  // если угол меньше 0 - отображаем 0
  if (CurrAngle < 0) CurrAngle = 0;
  LineB[0] = AddDot(EncodeNum(GetDight(CurrAngle, 1)));
  LineB[1] = EncodeNum(GetDight(CurrAngle, 0));
  LineB[2] = 0x63;   // символ градуса
  // вместо пульса показываем температуру силового модуля
  LineB[3] = GetDight(CurrTemperature, 1);     // <= 99 градусов
  if(LineB[3] > 0)  LineB[3] = EncodeNum(LineB[3]);
  LineB[4] = EncodeNum(GetDight(CurrTemperature, 0));
  LineB[5] = 0x63;   // символ градуса, можно еще символ 't' поставить (0x78)
  // время, секунды
  LineB[6] = EncodeNum(GetDight(Time % 60, 1)); // не больше 59, этож секунды
  LineB[7] = EncodeNum(GetDight(Time % 60, 0));
  if (Time % 2 == 0)  LineB[6] = AddDot(LineB[6]);
  // !!! Исправление моей ошибки с разводкой платы
  LineB[6] = CheckSecBug(LineB[6]);
  LineB[7] = CheckSecBug(LineB[7]);
}

// подготовка данных дисплея
void PrepareDisplay()
{
  if (ErrorCode > ecNormal)
  {
    PrepareDisplayInError();
    return ;
  }
  // если дисплей в режиме выбора программы тренировки - отрисовка идет по другому
  if (DisplayMode == dmProgramSelect)
  {
    PrepareDisplayInProgramSelect();
    return ;
  }
  // Альтернативный вывод (угол наклона и оставшееся время)
  if (AltView)
  {
    PrepareDisplayInAltView();
    return ;
  }
  // скорость
  LineA[0] = GetDight(CurrSpeed, 1);
  if(LineA[0] > 0)  LineA[0] = EncodeNum(LineA[0]);  // не отображаем ноль в начале
  LineA[1] = AddDot(EncodeNum(GetDight(CurrSpeed, 0)));
  LineA[2] = EncodeNum(GetDight(CurrSpeed, -1));
  // дистанция
  if (CurrDistance < 1)  // до 1 км выводим два знака после запятой
  {
    LineA[3] = AddDot(EncodeNum(GetDight(CurrDistance, 0)));
    LineA[4] = EncodeNum(GetDight(CurrDistance, -1));
    LineA[5] = EncodeNum(GetDight(CurrDistance, -2));
  } else {           // а больше - только один
    LineA[3] = GetDight(CurrDistance, 1);
    if(LineA[3] > 0)  LineA[3] = EncodeNum(LineA[3]);
    LineA[4] = AddDot(EncodeNum(GetDight(CurrDistance, 0)));
    LineA[5] = EncodeNum(GetDight(CurrDistance, -1));
  }
  // время, минуты
  LineA[6] = EncodeNum(GetDight(CurrTime / 60, 1));
  LineA[7] = EncodeNum(GetDight(CurrTime / 60, 0));
  if (CurrTime % 2 == 0)  LineA[7] = AddDot(LineA[7]);
  // калории
  LineB[0] = GetDight(CurrCalorie, 2);
  if((int)CurrCalorie > 99)  LineB[0] = EncodeNum(LineB[0]);
  LineB[1] = GetDight(CurrCalorie, 1);
  if((int)CurrCalorie > 9)  LineB[1] = EncodeNum(LineB[1]);
  LineB[2] = EncodeNum(GetDight(CurrCalorie, 0));
  // пульс
  LineB[3] = GetDight(CurrPulse, 2);
  if(CurrPulse > 99)  LineB[3] = EncodeNum(LineB[3]);
  LineB[4] = GetDight(CurrPulse, 1);
  if(CurrPulse > 9)  LineB[4] = EncodeNum(LineB[4]);
  LineB[5] = EncodeNum(GetDight(CurrPulse, 0));
  // время, секунды
  LineB[6] = EncodeNum(GetDight(CurrTime % 60, 1)); // не больше 59, этож секунды
  LineB[7] = EncodeNum(GetDight(CurrTime % 60, 0));
  if (CurrTime % 2 == 0)  LineB[6] = AddDot(LineB[6]);
  // !!! Исправление моей ошибки с разводкой платы
  LineB[6] = CheckSecBug(LineB[6]);
  LineB[7] = CheckSecBug(LineB[7]);
  //LineB[6] = EncodeNum(0);
}

// подсчет времени в тренировочной программе
int ProgramTime(String Program)
{
  return  60 * Program.length();
}

// подсчет дистанции в тренировочной программе
float ProgramDistance(String Program)
{
  float Summ = 0.0001;
  for (int i = 0; i<=Program.length(); i++)
  {
    Summ = Summ + Program[i];   // км/ч, сумма по минутам
  }
  return Summ / 600.0;
}

// обновление содержимого дисплея
void UpdateDisplay()
{
  // выключаем предыдущий разряд
  digitalWrite(Q[CurrDight], HIGH);
  // проталкиваем в декодер следующий разряд
  CurrDight++;
  if (CurrDight==Dights)  CurrDight = 0;
  ShowNum(LineA[CurrDight], LineB[CurrDight]);
  // включаем новый разряд
  digitalWrite(Q[CurrDight], LOW);
}

// отправка сообщения на силовой модуль
void SendPacket(char Code, long Value)
{
  TxData.Direction = 'D';
  TxData.Code = Code;
  TxData.Value = Value;
  ETout.sendData();
}

// пискнуть
void Beep(long Count = 1)
{
  BeeperCount = Count;
  BeeperTime = millis();
  BeeperStatus = HIGH;
  digitalWrite(Beeper, BeeperStatus);   // включили писчалку
}

// проверка получения сообщений от силового модуля
void RecivePacket()
{
  if (ETin.receiveData())
  {
    if (RxData.Direction == 'P')
    {
      switch (RxData.Code)
      {
        case 'A':    // акселерометр сообщает угол наклона дорожки (сообщается раз в 5 секунд)
          CurrAngle = RxData.Value*0.1;
          // если значение угла (вернее это не угол, а то, что выдал акселерометр) больше 380, а мотор
          // вращается - плавно его остановим
          if ((RxData.Value >= 380) && ((DisplayMode == dmManualRun) || (DisplayMode == dmProgramRun))) {
            StoppingModeStamp = millis(); // иначе плавно снижаем скорость
            StoppingMode = true;   
          }
          break;
        case 'E':    // какая-то ошибка, возможно перегрев, хотя датчик температуры я пока не цеплял
          ErrorCode = (Errors)RxData.Value; // а как выходить ?? TODO!!!
          break;
        case 'T':    // температура радиатора силового модуля (сообщается раз в 10 секунд)
          CurrTemperature = RxData.Value;
          break;
      } 
    }
  }
}

// отправляем на силовой модуль команду отключить реле (при ошибке, скорость тоже будет сброшена)
void SendError()
{
  SendPacket('E', 1);  // геркон
}

// выключаем пищалку
void CheckBeeper()
{
  if (((millis() - BeeperTime) > 200) && (BeeperCount > 0))  // длительность писка 200 мс
  {
    BeeperStatus = !BeeperStatus;
    digitalWrite(Beeper, BeeperStatus);        // выключили писчалку
    if (BeeperStatus == LOW)  BeeperCount--;   // уменьшили количество оставшихся биков
    if (BeeperCount > 0)  BeeperTime = millis();
  }
}

//** Обработка нажатий кнопок

void ButtonReset()
{
  if ((DisplayMode == dmProgramSelect) || (DisplayMode == dmStop))
  {
    DisplayMode = dmStop;
    CurrSpeed = 0;
    CurrTime = 0;
    CurrCalorie = 0;
    CurrDistance = 0;
    for (int i = 0; i<sizeof(PulseAveraging); i++)
    {
      PulseAveraging[i] = 0;
    }
    Beep();
  }
}

void ButtonUp()
{
  if ((DisplayMode == dmStop) || (DisplayMode == dmManualRun) || (DisplayMode == dmProgramRun))
  {
    if (CurrSpeed < MinSpeed)
    {
      CurrSpeed = MinSpeed;
      StartTime = millis();
    } else {
      CurrSpeed = CurrSpeed + 0.1;
    }    
    if (CurrSpeed >= MaxSpeed)
    {
      CurrSpeed = MaxSpeed;
    } else {
      if ((PreviosButton != pbUp) || (millis() - BeeperTime > 1000))  Beep(); // при регулировании скорости, когда зажата клавиша Up бикаем раз в 1 сек
    }                                                                         
    PreviosButton = pbUp;
    DisplayMode = dmManualRun;    
  }
}

void ButtonDown()
{
  if ((DisplayMode == dmStop) || (DisplayMode == dmManualRun) || (DisplayMode == dmProgramRun))
  {
    if (CurrSpeed == 0)  return;
    if (CurrSpeed > MinSpeed) {
      CurrSpeed = CurrSpeed - 0.1;
      if ((PreviosButton != pbDown) || (millis() - BeeperTime > 1000))  Beep(); // при регулировании скорости, когда зажата клавиша Down бикаем раз в 1 сек
    } 
    PreviosButton = pbDown;
    if (CurrSpeed <= MinSpeed) 
    {
      CurrSpeed = MinSpeed;
    }
    if (CurrSpeed != 0) DisplayMode = dmManualRun;
  }
}

void ButtonStartStop()
{
 if (DisplayMode == dmStop)  // если в режиме остановки - начинаем тренировку в ручном режиме с минимальной скорости
 {
   if (CurrSpeed < MinSpeed)  // START
   {
     CurrSpeed = MinSpeed;
     DisplayMode = dmManualRun;
     StartTime = millis();
   }
   StartTime = millis();
   Beep();
   return;
 }
 if (DisplayMode == dmProgramSelect) // если в режиме выбора программы - начинаем работу по программе
 {
   DisplayMode = dmProgramRun;
   StartTime = millis();
   PPos = 0;
   CurrSpeed = (float)P[ProgramNo][PPos] / 10.0;
   Beep();
   return;
 }
 if ((DisplayMode == dmManualRun) || (DisplayMode == dmProgramRun)) // STOP
 {
   if (CurrSpeed < MinSpeed) {   // если скорость маленькая - просто выключаем мотор
     CurrSpeed = 0;
     DisplayMode = dmStop;
   } else {
     StoppingModeStamp = millis(); // иначе плавно снижаем скорость
     StoppingMode = true;   
   }
   Beep();
 }
}

void ButtonSelect()
{
  // если находимся в режиме выбора программы - увеличим выбранную программу
  if (DisplayMode == dmProgramSelect)
  {
    ProgramNo++;
    if (ProgramNo >= (sizeof(P) / sizeof(P[0])) )  ProgramNo = 0;
    EEPROM.write(0, ProgramNo);
    Beep();
  }
  // перейти в режим выбора программы можно только находясь в режиме останова
  if (DisplayMode == dmStop)
  {
    DisplayMode = dmProgramSelect;
    Beep();
  }
}

void ButtonSet()
{
  if ((DisplayMode != dmProgramSelect) && (DisplayMode != dmError))
  {
    AltView = !AltView;
    Beep();
  }
}

// контроль нажатия на кнопки клавиатуры
void CheckButtons()
{
  if (millis() - ButtonStamp > 500)
  {
    int Val = analogRead(Buttons);

    /*
SELECT - 780
RESET  - 600
UP     - 440
DOWN   - 300
START  - 155
SET    - 1024
   */

    ButtonNo = 0;
    if ((Val >= 700) && (Val <= 825))  ButtonNo = 1;  // SELECT
    if ((Val >= 550) && (Val <= 650))  ButtonNo = 2;  // RESET
    if ((Val >= 375) && (Val <= 485))  ButtonNo = 3;  // UP
    if ((Val >= 250) && (Val <= 350))  ButtonNo = 4;  // DOWN
    if ((Val >= 100) && (Val <= 200))  ButtonNo = 5;  // START
    if ((Val >= 950) && (Val <= 1024)) ButtonNo = 6;  // SET (ALT VIEW)

    if ((ButtonNoPrev == 0) && (ButtonNo > 0))  // момент нажатия на кнопку
    {
      ButtonDownStamp = millis();
      ButtonNoPrev = ButtonNo;
    }
    if ((ButtonNoPrev > 0) && (ButtonNo == 0))  // момент отпускания кнопки
    {
      //ReadyToBeep = true;   // бикнуть по нажатию кнопки можно только послее ее отпускания, один раз, чтобы не пищала при повторных нажатиях
    }
    ButtonStamp = millis();
  }

  if ((millis() - ButtonDownStamp > 500) && (ButtonNo > 0))  // кнопка нажата дольше 500 мс.
  {
    // действие
    switch (ButtonNo) {
        case 1:
          ButtonSelect();
          break;
        case 2:
          ButtonReset();
          break;
        case 3:
          ButtonUp();
          break;
        case 4:
          ButtonDown();
          break;
        case 5:
          ButtonStartStop();
          break;
        case 6:
          ButtonSet();
          break;
    }
    //if (ReadyToBeep == true) Beep; // бипаем внутри обработчиков кнопок, в зависимости от режима
    //ReadyToBeep = false;
    ButtonDownStamp = millis();  // отжимаем кнопку
  }
}

// если StoppingMode - постепенно снижаем скорость вплоть до полной остановки
void CheckStoppingMode()
{
  if (StoppingMode && (millis() - StoppingModeStamp > 500))
  {
    if (CurrSpeed <= MinSpeed) {  // окончательная остановка
      CurrSpeed = 0;
      DisplayMode = dmStop;
      StoppingMode = false;
      Beep();      
    } else {  // сбрасываем скорость
        CurrSpeed = CurrSpeed - 0.5; 
    }
    StoppingModeStamp = millis();
  }
}

// если находимся в режиме работы по программе - раз в минуту переходим к следующему этапу
void CheckProgram()
{
  if (DisplayMode == dmProgramRun)
  {
    if ((CurrTime % 60 == 0) && (CheckProgramTime % 60 == 59))
    {
      PPos++;
      if (PPos >= P[ProgramNo].length()) // программа тренировки окончена
      {
        CurrTime = ProgramTime(P[ProgramNo]);           // округляем время и дистанцию
        CurrDistance = ProgramDistance(P[ProgramNo]);  
        PPos = 0;
        // Делаем плавный останов
        Beep(3);
        StoppingModeStamp = millis(); // иначе плавно снижаем скорость
        StoppingMode = true; 
      } else {
        CurrSpeed = (float)P[ProgramNo][PPos] / 10.0;
      }
    }
    CheckProgramTime = CurrTime;
  }
}

// если геркон не замкнут - отображаем ошибку и отключаем мотор (и релюху тоже)
void CheckGercone()
{
  if (digitalRead(Gercone))
  {
    DisplayMode = dmError;
    ErrorCode = ecGercone;
    CurrSpeed = 0;
    CurrTime = 0;
    CurrCalorie = 0;
    CurrDistance = 0;
    SendError();  // это остановит мотор и отключит реле
  } else {
    if ((DisplayMode == dmError) && (ErrorCode > ecNormal))
    {
      DisplayMode = dmStop;  /// TODO: нужно ловить момент изменения состояния
      ErrorCode = ecNormal;
    }
  }
}

// подсчет частоты пульса
void CheckPulse()
{
  // считаем период времени между ударами сердца и сохранем для усреднения
  bool Status = digitalRead(PulseMeter);   // ловим нисходящий фронт - это "удар" сердца
  if ((Status == false) && (PulseLastStatus == true)) // нисходящий фронт
  {
    int PulsePeriod = millis() - PulseLastTime;  // период крайнего сердечного сокращения
    if ((PulsePeriod > 240)  && (PulsePeriod < 3000))   // левые какие-то данные, отсекаем (пульс от 20 до 250 уд/мин)
    {
      long PulseRaw = 60000 / PulsePeriod;  // измеренный пульс по времени между двумя ударами
      for (int i=sizeof(PulseAveraging)-1; i>=1; i--)   // сдвигаем данные в окне усреднения
      {
        PulseAveraging[i] = PulseAveraging[i-1];
      }
      PulseAveraging[0] = PulseRaw;      
    }
    PulseLastTime = millis();
  }
  // усредняем данные и отображаем на экране раз в 2 секунды 
  if (millis() - PulseStamp > 2000)
  {
    int PulseSum = 0;
    int PulseCount = 1;  // чтобы деления на ноль не было в случае чего
    for (int i = 0; i<sizeof(PulseAveraging); i++)
    {
      if (PulseAveraging[i] > 0)
      {
        PulseSum = PulseSum + PulseAveraging[i];
        PulseCount++;
      }
    }
    CurrPulse = PulseSum / PulseCount;
    PulseStamp = millis();
  }  
  PulseLastStatus = Status;  // это чтобы нисходящий фронт ловить
}

// проверка отображаемого времени, пройденной дистанции и затраченных калорий
void CheckTime()
{
  if ((DisplayMode == dmManualRun) || (DisplayMode == dmProgramRun))
  {
    CurrTime = (millis() - StartTime) / 1000;
    // раз в секунду обновляем также значения пройденной дистанции и затраченных калорий
    if ((millis() - CheckTimeLast) >= 1000)
    {
      CheckTimeLast = millis();
      float SecDist = (CurrSpeed * 0.278) / 1000.0; // расстояние, пройденное за поледнюю секунду
      CurrDistance = CurrDistance + SecDist;
      // расчет калорий мутное-какое-то дело, пока здесь простая арифметика
      CurrCalorie = CurrCalorie + SecDist * 80 * (1+pow(sin(CurrAngle/57.296), 0.9));
    }
  }
}

// отправка значения требуемой скорости на силовой модуль
void CheckSpeed()
{
  if ((millis() - CheckSpeedStamp) > 300)
  {
    SendPacket('S', (int)(CurrSpeed*10));
    CheckSpeedStamp = millis();
  }
}

// Основной цикл программы
void loop() {
  CheckButtons();
  CheckBeeper();
  CheckStoppingMode();
  CheckGercone();
  CheckPulse();
  CheckProgram();
  CheckTime();
  CheckSpeed();
  RecivePacket();
  PrepareDisplay();
  UpdateDisplay();
}
