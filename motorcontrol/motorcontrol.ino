/*
  Управление скоростью вращения мотора.
  Читаем аналоговое значение из A0 и с помощью ПИД
  регулирования выдаем управляющее воздействие на ногу 5.
  Обороты мотора считаем по прерыванию энкодера на ноге 3 (INT0).
  При значении на A0 >= 20 (из 1023) включаем реле на ноге 2.
  
  2016-12-25, Wyfinger
*/

#include <PID_v1.h>
#include <EasyTransfer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define pinIn         A3  // Нога на которой висит потенциометр
#define pinEncoder     2  // Нога на которой висит энкодер
#define intEncoder     0  // INT0, соотвтетствует D2
#define pinOut         5  // D5, ШИМ мотора
#define pinRelay       4  // D4, реле
#define pinAccel      A1  // нога, на которой висит требуемая нам нога акселерометра
#define Temperature    6  // нога, на которой висит датчик температуры (DS18B20)

#define FasterPWM  true  // увеличение скорости ШИМ

// Параметры ПИД-регулятора
double Input, Output, Setpoint;  // эти параметры в км/час
const double consKp=1.5, consKi=0.4, consKd=0.4;
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

// Датчик температуры DS18B20, пока используется один - на радиаторе силового транзистора и
// защитных диодов, возможно также зацепить еще один датчик на мотор (на те жу выводы, шина OneWire).
OneWire TempWire(Temperature);
DeviceAddress Thermometer1 = {0x28, 0xFF, 0xF5, 0xB3, 0x63, 0x15, 0x02, 0xCB};  // ID датчика
DallasTemperature TempSensors(&TempWire);

volatile int intCounter = 0;  // Счетчик скорости полотна, 530 тиков на метр

// Подстроечные коэффициенты
const int intRPMWindow = 100;  // время расчета скорости полотна, мс
const int intEncTickByMeter = 530;  // тиков энкодера на метр
const float fMinSpeed = 1.0;
const float fMaxSpeed = 10.0;

const int intStepLimit = 10;
const int intMinOut = 00;   // с лампочкой в цепи мотор начинает вращаться где-то на 100
const int intMaxOut = 110;   // пока ограничу выходное воздействие на мотор

int ms = 1;

// Служебные переменные
unsigned long intLastMillsPWM;
unsigned long intLastMillsDSP;
float fCurrSpeed;  // скорость, замеренная с энкодера
float fNeedSpeed;  // скорость, требуемая по показаниям потенциометра
int intOut;

int intEncWindow = 50; // микросекунд
long intEncSumm = 0;
long intEncCount = 0;
long intLastEnc = 0;
bool boolEncState = false;
bool boolEncStatePrev = false;

byte intValFromDisplay;

long CheckAngleStamp;  // время, когда последний раз проверялся угол наклона полотна
float BaseAngle;        // угол основания дорожки
float KAngle = 0.6;    // коэффициент усреднения для значения угла (подбирается экспериментально)

long TemperatureReadStamp;     // время с последнего считывания температуры
bool TemperatureRequestSended; // флажек о том, что температура была запрошена, нужно подождать немного перед получением ответа

EasyTransfer ETin, ETout;  // объект EasyTransfer для обмена сообщениями с силовым модулем

struct PacketStruct {
  char Direction;   // направление передачи: D - от дисплея, P - от силового модуля
  char Code;        // код сообщения
  long Value;       // данные, в зависимости от кода сообщения
};

PacketStruct RxData;  // пакет, полученный от силового модуля
PacketStruct TxData;  // пакет для отпрвки на силовой модуль

void setup()
{
  // Установка предделителей частоты, чтобы поднять частоту ШИМ
  // на пятой ноге ШИМ должен стать с частотой 7812.50 Гц
  if (FasterPWM == true)
  {
    TCCR0B = TCCR0B & B11111000 | B00000010;
    ms = 8;
  }
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
  myPID.SetTunings(consKp, consKi, consKd);
  myPID.SetOutputLimits(intMinOut, intMaxOut);
  myPID.SetSampleTime(intRPMWindow*ms);  // устанавливаем период тюнинга ПИД-регулятора равным периоду расчета скорости мотора
  //
  TempSensors.begin();
  TempSensors.setResolution(Thermometer1, 9);
  // Связь для отладки и дисплея
  Serial.begin(9600);
  ETin.begin(details(RxData), &Serial);
  ETout.begin(details(TxData), &Serial);
  // Силовое реле
  digitalWrite(pinRelay, HIGH);  // !!!!!!!!!!!!!!!!!
}

// Обработчик прерывания энкодера
void IncCounter() 
{
  // вообще говоря внутри обработчика прерывания нужно снимать это самое прерывание,
  // а потом устанавливать заново, но у меня тут код на одну машинную инструкцию.
  intCounter++;
}

// Вернет True когда посчитаются обороты
bool CalcSpeed()
{
  bool rez = false;
  if (millis() - intLastMillsPWM >= intRPMWindow*ms)
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

// отправка сообщения на дисплейный модуль
void SendPacket(char Code, long Value)
{
  TxData.Direction = 'P';
  TxData.Code = Code;
  TxData.Value = Value;
  ETout.sendData();
}

// проверка получения сообщений от силового модуля
void RecivePacket()
{
  if (ETin.receiveData())
  {
    if (RxData.Direction == 'D')
    {
      switch (RxData.Code)
      {
        case 'S':    // Получена требуемая скорость
          intValFromDisplay = RxData.Value;
          break;
        case 'E':    // какая-то ошибка, отключаем реле
          digitalWrite(pinRelay, LOW);
          break;         
      } 
    }
  }
}

// проверка угла наклона полотна
void CheckAngle()
{
  if (millis() - CheckAngleStamp > 1000*ms)  //2000 или 1000
  {
    // считываем данные с акселерометра
    int NewAngle = analogRead(pinAccel);  
    // пытался полчуть замер угла, но и-за того, что акселерометр не совсем предназначен для измерения
    // таких маленьких улов, да еще он не цифровой, а по АЦП есть погрешность, короче говоря нормально
    // измерить угол на практике оказывается невозможно (вернее я не смог, может что-то упускаю из виду),
    // усреденение и типа фильр Калмана не помогает. Поэтому здесь выдаем просто замеренное значение -
    // 350. Это число имеет отношение к углу наклона, но фактически это просто для того, чтобы окно дисплея
    // занять.
    // Еще передаваемое значение служит для плавной остановки мотора при подъеме дорожки если вдруг
    // кому-то вздумается поднимать полотно при работающем движе.
    SendPacket('A', analogRead(pinAccel)); 
    CheckAngleStamp = millis();
  }
}

// проверка температуры с датчика DS18B20
void CheckTemperature()
{
  // считываем данные не очень часто, просим датчик замерить температуру
  if (millis() - TemperatureReadStamp > 2500*ms)    // для работы с датчиками температуры можно использовать библиотеку DallasTemperature,
    if (!TemperatureRequestSended) {                // сейчас она используется в setup() для установки точности работы датчика,
      // просим датчик замерить температуру         // но непосредственно при замере температуры работаем напрямую и самостоятельно пересчитываем
      TempWire.reset();                             // полученное значение, это значительно быстрее. Так при работе с DallasTemperature
      TempWire.write(0xCC);                         // запрос температуры от датчика, также как как и чтение ответа, занимает более 100 мс.
      TempWire.write(0x44);                         // При работе напрямую это время составляет менее 25 мс. А это важно, т.к. мы должны следить
      TempWire.reset();                             // за управляющими воздействиями с центрального блока.
      TemperatureReadStamp = millis();              // При необходимости использовать несколько датчиков прийдется работать через DallasTemperature
      TemperatureRequestSended = true;              // (можно еще на уровне OneWire выбирать устройство на шине и считывать данные, но это по времени также)
      //TempSensors.requestTemperatures();
      //TemperatureReadStamp = millis();
      //TemperatureRequestSended = true;
    } else {                                        
      // запрос на измерение температуры уже был отправлен на датчик, осталось только считать результат
      TempWire.reset();
      TempWire.write(0xCC);
      TempWire.write(0xBE);  
      byte data[2];
      data[0] = TempWire.read();
      data[1] = TempWire.read();
      int Temp = ((data[1] << 8) | data[0]) >> 4;  // °С, делим на 16
      // float fTemp = TempSensors.getTempC(Thermometer1);
      SendPacket('T', Temp);     // отправляем температуру на голову
      TemperatureReadStamp = millis();
      TemperatureRequestSended = false;
      // Если температура большая - выключаемся
      if (Temp > 65) {
        SendPacket('E', 2); // перегрев
        digitalWrite(pinRelay, LOW);   // !!! резкий останов - может лучше плавную остановку сделать ??
      }
    }    
}

// Главный цикл
void loop()
{
  CheckAngle();
  CheckTemperature();
  RecivePacket();
  
  // Самая ответственная часть кода - управление мотором через ПИД-регулятор.
  // intValFromDisplay - значение скорости, полученное от дисплейного модуля
  fNeedSpeed = (float)(intValFromDisplay * 0.1);
  if (fNeedSpeed > fMaxSpeed)  fNeedSpeed = fMaxSpeed; 
  if (fNeedSpeed < fMinSpeed) {
    Input = fCurrSpeed;     // пересчет ПИД регулятора нужен, чтобы при последуюшем включении не было рывка.
    Setpoint = fNeedSpeed;
    Output = 0;
    myPID.Compute();
    intOut = 0;
    analogWrite(pinOut, 0);
    return;
  }

  // Когда рассчитали скорость от энкодера (время расчета определяется intRPMWindow)
  if(CalcSpeed())
  {
    Input = fCurrSpeed;
    Setpoint = fNeedSpeed;
    double prewOut = Output;
    myPID.Compute();
    intOut  = (int)Output;
    if (intOut > intMaxOut) intOut = intMaxOut;
    if (intOut < intMinOut) intOut = 0;

    analogWrite(pinOut, intOut);    // !!!!    
  }
}