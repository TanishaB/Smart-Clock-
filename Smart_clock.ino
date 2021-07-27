#include <BlynkSimpleEsp8266.h>           //Blynk lib
#include <SoftwareSerial.h>               //serial communication lib
#include <RTClib.h>                       //rtc lib
#include <Wire.h>                         //allows to communicate with I2C
#include <LiquidCrystal_I2C.h>            //Lcd lib

SoftwareSerial mySerial(3,1); //rx and tx
LiquidCrystal_I2C lcd(0x3F, 16, 2); //address
RTC_DS1307 RTC;

char auth[] = "7faPc064gvg5_arC6z1GqmCGW7uqa0D*" ;     //authentication
char ssid[] = "Tanu" ;                                 //Wifi credentials
char pass[] = "*******" ;
int h;                           //hour variable
int m;                           //min variable

int val;                         // pirsensor input value
int set_alarm;
int buzz= 14;                    //D5 pin(buzzer)
int pir= 13;                     //D7 pin(pir)
int flag=0;                      //To check is alarm is on or off


// Connection of I2C: Vcc, Gnd, SDA->D2, SCL->D1
// Connection of RTC module: Vcc, Gnd, SDA->D2, SCL->D1
// Connection of buzzer: Gnd, o/p->D5
// Connection of Blynk: Hour->V5, Minute->V6, Set_alarm button->V7
// Connection of pir sensor: Vcc, Gnd, input->D7

void setup()
{
  pinMode(buzz,OUTPUT);
  pinMode(pir,INPUT);
  digitalWrite(13,LOW);
  mySerial.begin(9600);        // Setting the baud rate of GSM Module
  
  lcd.init();                  //initialize
  lcd.backlight();
  Wire.begin();               // Start the I2C
  RTC.begin();                // Init RTC
  RTC.adjust(DateTime(__DATE__, __TIME__));  //Used to update the rtc’s internal time and date.
  Blynk.begin(auth,ssid,pass);
}


BLYNK_WRITE(V5)
{
  h = param.asInt();             //read i/p of hour from blynk
}
BLYNK_WRITE(V6)
{
  m = param.asInt();            //read i/p of min from blynk
}
BLYNK_WRITE(V7)
{
  set_alarm = param.asInt();    //read i/p of set_alarm from blynk (0 or 1)
}

void loop()
{
  Blynk.run();
  DateTime now = RTC.now(); //get cuurent time and date
  
  lcd.setCursor(0, 0);    //Display time and date
  lcd.print("Date: ");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  
  lcd.setCursor(6, 0);
  lcd.print(now.year(), DEC);
  lcd.print(":");
  lcd.print(now.month(), DEC);
  lcd.print(":");
  lcd.print(now.day(), DEC);
 
  lcd.setCursor(6, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.print(now.second(), DEC); 
  delay(1000);
  lcd.clear();

val = digitalRead(pir);    //read pir value

if(h==now.hour() and m==now.minute() and set_alarm == 1) //check current time with alarm setup
{
  flag =1;                                              //Alarm time has reached
  lcd.setCursor(0, 0);
  lcd.print("Alarm");
  digitalWrite(buzz, HIGH);                             //Buzzer should blow
  
  }

if (val == 1 and set_alarm ==1 and flag==1)            //On movements
{
      flag = 0;                                        //Alarm is set off on movement
      digitalWrite(buzz,LOW);
      set_alarm =0;
      mySerial.println("AT+CMGF=1");                  //Sets the GSM Module in Text Mode
      delay(1000);
    mySerial.println("AT+CMGS=\"+917498486850\"\r");  //sends msg to the specified number
    delay(1000);
    mySerial.println("Alarm went off. The person has woken up.");  // The SMS text you want to send
    delay(1000);
    mySerial.println((char)26);               // ASCII code 
      
    }

lcd.clear();

}
