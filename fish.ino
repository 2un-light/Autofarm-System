#include <SoftwareSerial.h>
#include <OneWire.h>

#define BT_RXD 12
#define BT_TXD 11
SoftwareSerial bluetooth(BT_RXD, BT_TXD);

 //온도센서를 2번 핀으로 연결
int DS18S20_Pin = 2;       

//2번 핀과 연결되 OneWire 객체 생성                    
OneWire ds(DS18S20_Pin);                         

float calibration = 0.00; //change this value to calibrate

//PH센서 (검은색)
const int analogInPin = A5; 

// 수위센서
int water_pin = A0;         //(기존_갈색)
int water_pin1 = A1;        // (새로운_흰색)
int sensorValue = 0; 

//릴레이 모듈
int relay1 = 7; // 새로운(파란색)
int relay2 = 8; // 기존(빨간색)

unsigned long int avgValue; 
float b;
int buf[10],temp;

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600);
  pinMode(relay1, OUTPUT); // 7 펌프
  pinMode(relay2, OUTPUT); // 8 솔밸브
}

void loop(){
   //온도 측정 후 변수에 저장
   float temperature = getTemp();                
   delay(500);

   for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i]; 
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = -5.70 * pHVol + 21.34;

 //수위센서
 int val = analogRead(A0);
 int valone = analogRead(A1);

// ph값
bluetooth.print(phValue);
bluetooth.print("/");
// 수온
bluetooth.print(temperature);
bluetooth.print("/");

 
  if(val > 300 && valone > 300) {
      bluetooth.print("\n");
      digitalWrite(relay1, HIGH); //펌프
      delay(20000);
      digitalWrite(relay2, HIGH); //솔밸브
      delay(1000);
       if(phValue >= 7.0){
          delay(1000);
          }
    }
    else{
      if(phValue >= 7.0){
          digitalWrite(relay1, HIGH); //펌프
          digitalWrite(relay2, LOW);  //솔밸브
     }
     if(phValue <= 6.9){
        digitalWrite(relay1, HIGH); //펌프
        digitalWrite(relay2, HIGH); //솔밸브
        delay(1000);
      }
   }
     if (bluetooth.available() > 0) {
    char num = char(bluetooth.read());
      if (num == '1'){
          digitalWrite(relay1, HIGH); //펌프
          delay(1000);
        }
        else if (num == '2'){
          digitalWrite(relay1, LOW); //펌프
          delay(1000);
        }
        else if (num == '3'){
          digitalWrite(relay2, HIGH); //솔밸브
          delay(1000);
        }
        else if (num == '4'){
          digitalWrite(relay2, LOW); //솔밸브
          delay(1000);
        }
   }

}

float getTemp(){                                   //온도 측정 후 반환하는 함수
 byte data[12];
 byte addr[8];
 if ( !ds.search(addr)) {
   ds.reset_search();
   return -1000;
 }
 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }
 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }
 ds.reset();
 ds.select(addr);
 ds.write(0x44,1);                                   
 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); 
 
 for (int i = 0; i < 9; i++) { 
  data[i] = ds.read();                                                          
 }
 
 ds.reset_search(); 
 byte MSB = data[1];
 byte LSB = data[0];
 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16; 
 return TemperatureSum;
}
