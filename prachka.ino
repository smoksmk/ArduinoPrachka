#include <CyberLib.h> //Библиотека от Cyber-Place.ru 
#include <PID_v1.h>
volatile uint8_t tic, Dimmer1=255; 
uint8_t data; 
int lock = 0;  // Блокировка переключений
int Woter = 0; // уровень воды

int stepp;
//задаем начальные значения для pid регулятора
float kp = 2.0;
float ki = 0.4;
float kd = 0.1;
unsigned int rpm;
long previousMillis = 0;   // здесь будет храниться время последнего изменения состояния светодиода 
long interval = 1000;
long interval2 = 50;
 int save_state = 0; 
//подключаем PID.
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, kp, ki, kd, REVERSE);

void setup()  
{  
  D9_Out; D9_Low;//D12_Low;D4_High;
 D12_Out; D4_Out; D12_Low; D4_High;//мотор
  D14_In; D14_High; //уровен
  D4_In; D4_High; //Кнопка
  D15_Out; D15_Low; // D15 - клапан
  D16_Out; D16_Low;//реверс
  D17_Out;  D17_Low;//помпа 
 
  
  // задаем параметры pid
  myPID.SetOutputLimits(80, 175); // ограничение по оборотам
  myPID.SetSampleTime(50);
 // Input = rpm;
  Setpoint = 0; //после запуска томозим двигатель
  Input = 0;
    Output = 255;
 //int previousMillis = 0; // сбрасываем счетчик времени
// int  ledState = LOW;

  D2_In; D3_In;//настраиваем порт на вход для отслеживания прохождения сигнала через ноль 
  D2_High; 
  attachInterrupt(0, tachoChange, HIGH); // настраиваем прерывание для тахометра
  attachInterrupt(1, detect_up, LOW);  // настроить срабатывание прерывания interrupt0 на pin 3 на низкий уровень 
  StartTimer1(halfcycle, 40); //время для одного разряда ШИМ 
  StopTimer1(); //остановить таймер 
  //UART_Init(57600); //инициализация порта 
    Serial.begin(57600);
    Serial.println("Hello!");
      
      
      
   /*if (D14_Read == 0){
   D17_High;
    Serial.println("Ahtung!!!!");
   delay(100000);
    Serial.println("And_AHTUNG!!!!");
   D17_Low;
   }*/

} 
//********************обработчики прерываний******************************* 
void halfcycle()  //прерывания таймера 
{  
  
  tic++;  //счетчик   
  if(Dimmer1 < tic ) D12_High; //управляем выходом 
} 

void  detect_up()  // обработка внешнего прерывания. Сработает по переднему фронту 
{   
 tic=0;             //обнулить счетчик 
 ResumeTimer1();   //запустить таймер 
 attachInterrupt(1, detect_down, HIGH);  //перепрограммировать прерывание на другой обработчик 
}   

void  detect_down()  // обработка внешнего прерывания. Сработает по заднему фронту 
{    
 StopTimer1(); //остановить таймер 
 D12_Low;       //логический ноль на выходы 
 tic=0;       //обнулить счетчик 
 calculateRPM(); //обнулим счетчик оборотов
 attachInterrupt(1, detect_up, LOW); //перепрограммировать прерывание на другой обработчик 
}  
//************************************************************************* 
volatile unsigned int tacho;

//1096hz=~8200rpm
// 8 tacho full pulse per ravolution = 16 zero cross
void calculateRPM()
{
    // belt = tacho*pulleyD*3.1415*1000/100/ms;
    rpm = tacho;// *60/ms*1000/16;
    tacho = 0;
}

void tachoChange()
{
    tacho++;
}


//Управление нагрузкой




void add_water(){
 // Serial.println("I add water");
  if (D14_Read == 1){
     Woter = Woter + 1;
  }
  D15_High;
   if (Woter >= 990){
   stepp = stepp + 1;
   D15_Low;
  }
  
  if (millis() - previousMillis > interval2) {
    previousMillis = millis();   // запоминаем текущее время
    
      Serial.print(" Woter_lavel: ");
        Serial.println(Woter);  
        Woter = 0;
} 

  
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// класс задержки времени

class Flasher
{
  // Переменные - члены класса
  // Инициализируются при запуске
  int ledPin; // номер пина со светодиодом
  long OnTime; // время включения в миллисекундах
  long OffTime; // время, когда светодиод выключен
  int Int_Cikle;
 int count =0;
 int  value1 = 0;
  // Текущее состояние
  int ledState; // состояние ВКЛ/ВЫКЛ
  int value;
  unsigned long previousMillis; // последний момент смены состояния
 
 void motor_on(){//стирка/полоскание
Serial.println("motor_on");
   Setpoint = 1;
   myPID.SetMode(AUTOMATIC);
   }
  
void motor_off(){
    
  Serial.println("motor stop");
   myPID.SetMode(MANUAL);
            Output = 255;
            
      if (value1 == 0){
        value1 = 1;
        Serial.println("left");
      D16_High; 
       }else{
        value1 = 0;
        Serial.println("right");
      D16_Low; 
       }
      
     
}



void pomp_on(){//Слив воды
  Serial.println("water down");
  
    D17_High;
}
void pomp_off(){
  Serial.println("pomp Stop");
    D17_Low;
   
  }
  
 void motor2_on(){//стирка/полоскание
Serial.println("motor_on");
   D16_Low; 
   Setpoint = 4;
   myPID.SetMode(AUTOMATIC);
   }
   void motor3_on(){//стирка/полоскание
Serial.println("motor_on");
   D16_Low; 
   Setpoint = 20;
   myPID.SetMode(AUTOMATIC);
   }
  // Конструктор создает экземпляр Flasher и инициализирует 
  // переменные-члены класса и состояние
  public:
  Flasher(int pin, long on, long off, int cikle)
  {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);
 
    OnTime = on;
    OffTime = off;
    Int_Cikle = cikle;
 
    ledState = LOW;
    value = LOW;
    previousMillis = 0;
  }
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Update()
  {
   // выясняем не настал ли момент сменить состояние светодиода
 
   unsigned long currentMillis = millis(); // текущее время в миллисекундах
 
   if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
   {
     ledState = LOW; // выключаем
     previousMillis = currentMillis; // запоминаем момент времени
     if (ledPin) digitalWrite(ledPin, ledState); // реализуем новое состояние
     count++;
     switch (ledPin) {
          case 1:
              motor_off();
         break;
          case 2:
              pomp_off();
        break;
         case 3:
                Serial.println("woter off");
              D15_Low;
        break;
        case 4:
              motor_off();
        break;
         case 5:
              motor_off();
               D17_Low;
        break;
   }
    if (lock == 1)count = 0;
    if (count  == Int_Cikle){  stepp = stepp + 1; count =0;}
    //if (uart_delay  == 1) 
    //Serial.println(stepp);  
    Serial.println(count);  
   }
   else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
   {
     ledState = HIGH; // выключаем
     previousMillis = currentMillis ; // запоминаем момент времени
     digitalWrite(ledPin, ledState); // реализуем новое состояние
     switch (ledPin) {
          case 1:
              motor_on();
              Serial.println("motor");
         break;
          case 2:
              pomp_on();
        break;
        case 3:
              Serial.println("woter");
              D15_High;
        break;
            case 4:
              motor2_on();
        break;
         case 5:
              motor3_on();
               D17_High;
        break;
   }
  // if (lock == 1)stepp = 0;
   }
  
 }  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

Flasher motor(1, 40000, 5000, 10);
Flasher pomp(2, 20000, 2000, 2);
Flasher led1(3, 45000, 1000, 1);
Flasher motor2(5, 80000, 500, 1);
Flasher motor3(4, 30000, 500, 1);

void loop()  
{Start 
int lock2 = 0;



     
   

      Input = rpm;
      myPID.Compute();
      Dimmer1 = Output;//5    

  while(Serial.available()) 
 { 
    data=Serial.read();
    if (data==49) stepp = 1;//1
    if (data==50) stepp = 3;//2
    if (data==51) stepp = 7;//3    
    if (data==52) stepp = 13;//4
    if (data==53) {
      
      Setpoint = 2;
        myPID.SetMode(AUTOMATIC);
     
      //Dimmer1 = 173; 
    
  }
    if (data==54)  {
      
      Setpoint = 5;
        myPID.SetMode(AUTOMATIC);
     
      //Dimmer1 = 173; 
    
  }//6
    if (data==55) Output = 180;//7
    if (data==56) Output = 170;//8
    if (data==48){ 
    Setpoint = 0; 
              myPID.SetMode(MANUAL);
            Output = 255;
             }//0
   //if(data>47 && data<59) 
    //{ 
      //data=225-(data-48)*25; 
     // Dimmer1=data;
     //UART_SendByte("I resive:");
      //delay_ms(5000);
      Serial.print("I seid: ");
        Serial.println(data);
     
   // UART_SendByte(data); 
    //}
    } 
     switch(stepp){
      case 1: // набираем воду
            add_water();
            break;
      case 2: // Добираем не достающее
          //stepp = stepp+1;
          led1.Update();
          break;
      case 3: // стирка 
          lock2 = 1;
          motor.Update();
         // Serial.println("start");
          break;
       case 4: // слив воды
           pomp.Update();
            break;
         case 5: // Набираем воду
          add_water();
          break;
            case 6:// Добираем воду
          led1.Update();
          break;
      case 7:// полоскаем
          // lock2 = 1;
          motor.Update();
          break;
       case 8:// сливаем
           pomp.Update();
            break;
       case 9://отжим
           motor3.Update();
            break;
       case 10://отжим
           motor2.Update();
       //    pomp.Update();
            break;
             case 11: // Набираем воду
          add_water();
          break;
            case 12:// Добираем воду
          led1.Update();
          break;
      case 13:// полоскаем
          // lock2 = 1;
          motor.Update();
          break;
       case 14:// сливаем
           pomp.Update();
            break;
       case 15://отжим
           motor3.Update();
            break;
       case 16://отжим
           motor2.Update();
       //    pomp.Update();
            break;
       case 17:
           stepp = 0;
           break;
         }
       
       
       //Setpoint = 5;
  if (millis() - previousMillis > interval) {
    previousMillis = millis();   // запоминаем текущее время
   
     int  val = analogRead(5);     // считываем значение
     //if (val <= 650) val = val + 10;   
     if (val <= 450 && lock2 == 1){// бллокируем переключения пока вода не нагреется
       lock = 1;
       D9_High;
     }else if(val >= 520){
      
       lock = 0;
       D9_Low;
     }// бллокируем переключения пока вода не нагреется
      Serial.print(" temp: ");
      Serial.print(val);  
      Serial.print(" rpm: ");
      Serial.print(rpm);
      Serial.print(" Dimmer1: ");
      Serial.print(Dimmer1);
      Serial.print(" tic: ");
      Serial.print(tic);
      Serial.print(" count: ");
      Serial.print(stepp);
      Serial.print(" lock: ");
      Serial.print(lock);
      Serial.print(" Woter_lavel: ");
      Serial.println(Woter);
      
 
      
      
      
      
      
      
 if (D4_Read == 1){ // Кнопка старт пауза
  delay_ms(30);
    if (stepp == 0 & save_state == 0 ){
      stepp = 1;
       Serial.println("stepp1");
    }else if(save_state !=0 & stepp ==0){
      stepp = save_state;
      save_state = 0;
       Serial.println("resume");
    }else if(stepp !=0 & save_state ==0){
       Serial.println("pause");
      save_state = stepp;
      stepp = 0;
    }
     delay_ms(30);
  }
 
 
  }  
End}



