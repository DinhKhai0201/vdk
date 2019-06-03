//#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h> 

#define triger 250
//Define the parameters of the flower1
#define servoPosition0 10  
#define triggerValue0 60  
#define wateringTime0 35  

//Define the parameters of the flower2
#define servoPosition1 50  
#define triggerValue1 50 
#define wateringTime1 20  

//Define the parameters of the flower3
#define servoPosition2 120 
#define triggerValue2 50 
#define wateringTime2 15  

const int pumpAnodePin =  6;      
const int pumpCathodePin =7;
const int lamp=5;

Servo horizontalServo; 
Servo rainservo;  
int servoRainPin = 10;   

const int DHTPIN = 3;     
const int DHTTYPE = DHT11;  
 
const int analogInPin0 = A0;  // 
const int analogInPin1 = A1;
const int analogInPin2 = A2;

//the value readed from each moisture sensor
int moistureValue0 = 0;        
int moistureValue1 = 0;
int moistureValue2 = 0;

int rainSensor = A5; // Chân tín hiệu cảm biến mưa ở chân digital 6 (arduino)
int lightSensor = A4;// khai báo chân digital 10 cho cảm biến anh sang


//the sum of the 30 times sampling
long int moistureSum0 = 0;   
long int moistureSum1 = 0;
long int moistureSum2 = 0;

#define COLD_TEMP     15
#define HOT_TEMP      24
#define LIGHT         20
#define RAIN          70

//boolean lampStatus = 0;
float humDHT;
float tempDHT;
boolean posi=true; 
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  pinMode(rainSensor,INPUT);// Đặt chân cảm biến mưa là INPUT, vì tín hiệu sẽ được truyền đến cho Arduino
  pinMode(lightSensor,INPUT);
  Serial.begin(115200);
  Serial.println("Do An !");
  dht.begin(); 
  horizontalServo.attach(9); 
  rainservo.attach(servoRainPin);
  
  initPosition();
  delay(500);
  pinMode(pumpAnodePin, OUTPUT);
  pinMode(pumpCathodePin, OUTPUT);
  pinMode(lamp, OUTPUT);      
}

void loop() {
  turnOnorOffLightbySensor();
  maiche();
  for(int i = 0; i < 30; i++)//samping 30 time within 3 seconds
  {
    moistureSum0 = moistureSum0 + analogRead(analogInPin0);  
    moistureSum1 = moistureSum1 + analogRead(analogInPin1);
    moistureSum2 = moistureSum2 + analogRead(analogInPin2);
    delay(100);
  }
  moistureValue0 = moistureSum0 / 30;//get the average value
  moistureValue1 = moistureSum1 / 30;
  moistureValue2 = moistureSum2 / 30;
  int m0=map(moistureValue0, 1023, 0, 0, 100);
  int m1=map(moistureValue1, 1023, 0, 0, 100);
  int m2=map(moistureValue2, 1023, 0, 0, 100);
  getDhtData();
  //turnOnorOffLightbySensor();
  // print the results to the serial monitor:
  Serial.print("Moisture0 = " );                       
  Serial.print(m0);
  Serial.print(" %\t");      
  Serial.print(" Moisture1 = ");      
  Serial.print(m1); 
  Serial.print(" %\t");
  Serial.print("Moisture2 = ");  
  Serial.println(m2);
  Serial.print(" %\t"); 
  Serial.print("\n");
  Serial.print("Do am: ");
  Serial.print(humDHT);
  Serial.print(" %\t");
  Serial.print("Nhiet do: ");
  Serial.print(tempDHT);
  Serial.print(" *C\t");
  Serial.print("\n");
  Serial.print("cam bien anh sang " );                      
  Serial.print(map(analogRead(lightSensor), 1023, 0, 0, 100));
  Serial.print(" %"); 
  Serial.print("\n");
  Serial.print("cam bien mua " );                 
  Serial.print(map(analogRead(rainSensor), 1023, 0, 0, 100));
  Serial.print(" %");  
  Serial.print("\n");
  moistureSum0 = 0;//reset the variable
  moistureSum1 = 0;
  moistureSum2 = 0;
  suoiam();
  delay(1000);
  
  if(m0 < triggerValue0)
  {
    proWatering(servoPosition0, wateringTime0);
    return;
   } 
   if(m1 < triggerValue1)
  {
    proWatering(servoPosition1, wateringTime1);
    return;
   } 
   if(m2 < triggerValue2)
  {
    proWatering(servoPosition2, wateringTime2);
    return;
   }
}
void runDHT(void) {
  if (humDHT > HOT_TEMP) {
      digitalWrite(pumpAnodePin, HIGH);
      delay(1000);
  }
  if (tempDHT >HOT_TEMP) {
      digitalWrite(pumpAnodePin, HIGH);
      delay(1000);
  } 
}
void turnOnorOffLightbySensor(){
  int value = analogRead(lightSensor);
  int valueS=map(value, 1023, 0, 0, 100);
  if (valueS < 40){
    digitalWrite(lamp, HIGH);
  }
  else {
    digitalWrite(lamp, LOW);
    } 
}
//mái che
void maiche(){
    int valueRain = analogRead(rainSensor);
  int valueRainS=map(valueRain, 1023, 0, 0, 100);
  if (valueRainS > RAIN){
    if (posi==true) {
    for (int i=0;i<=180;i++){
        rainservo.write(i);
        delay(50);       
    } 
    posi =false;
  }
 }
 if (valueRainS < RAIN){
    if (posi==false) {
    for (int i=180;i>=0;i--){
        rainservo.write(i);
        delay(50);       
    } 
    posi =true;
  }
 }
}
void suoiam (void) {
  if (tempDHT < COLD_TEMP) {
    digitalWrite(lamp, HIGH);
    }   
  }
  
void getDhtData(void){
  tempDHT = dht.readTemperature();
  humDHT = dht.readHumidity();
  if (isnan(humDHT) || isnan(tempDHT))   // Kiểm tra kết nối lỗi thì thông báo.
  {
    Serial.println("\tLỗi kết nối đến sensor");
    return;
  }
}

 void initPosition()
{
  horizontalServo.write(25);  
  delay(1000);
}

void proWatering(int servoPosition, int wateringTime)  
{
  horizontalServo.write(servoPosition - 10);  
  delay(500); 
  digitalWrite(pumpAnodePin, HIGH);  
  digitalWrite(pumpCathodePin, LOW);
  
  for(int pos1 = 15; pos1 < 33; pos1 +=6)//
  {
    int pos;
    for(pos = servoPosition - 10; pos <= servoPosition + 10; pos += 1)  
    {                               
      horizontalServo.write(pos);              
      delay(wateringTime);                      
    } 


    for(pos = servoPosition + 10; pos>= servoPosition - 10; pos-=1)    
    {                                
      horizontalServo.write(pos);              
      delay(wateringTime);                      
    } 
  }
  
  for(int pos1 = 33; pos1 > 15; pos1 -= 6)//
  {

    for(int pos = servoPosition - 10; pos <= servoPosition + 10; pos += 1) 
    {                                  
      horizontalServo.write(pos);             
      delay(wateringTime);                       
    } 

    for(int pos = servoPosition + 10; pos >= servoPosition -10; pos-=1)     
    {                                
      horizontalServo.write(pos);             
      delay(wateringTime);                       
    } 
  }
  digitalWrite(pumpAnodePin, LOW);
  digitalWrite(pumpCathodePin, LOW);
  delay(500);
  initPosition();
}

void watering(int servoPosition, int wateringTime)
{
  Serial.println(servoPosition);
  horizontalServo.write(servoPosition);
  delay(1000);
  digitalWrite(pumpAnodePin, HIGH);
  delay(10000);
  digitalWrite(pumpAnodePin, LOW);
  delay(3000);
}
