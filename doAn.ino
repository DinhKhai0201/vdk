#include <Blynk.h>

#include "DHT.h"
#include <SimpleTimer.h>

#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


const int DHTPIN = D3;     
const int DHTTYPE = DHT11;   

#define SOIL_MOIST_1_PIN A0
#define PUMP_PIN D6   

#define DRY_SOIL      50
#define WET_SOIL      88
#define COLD_TEMP     15
#define HOT_TEMP      24
#define TIME_PUMP_ON  15


#define READ_SOIL_TEMP_TM 2L
#define READ_SOIL_HUM_TM  10L //Đọc cảm biến ẩm đất
#define READ_AIR_DATA_TM  2L  //Đọc DHT
#define DISPLAY_DATA_TM   10L
#define SEND_UP_DATA_TM   10L
#define AUTO_CTRL_TM      60L 

char auth[] = "60ef39be3eaf4dd399bfff4eb697df6b"; 
char ssid[] = "kai"; 
char pass[] = "12345678"; 

float humDHT;
float tempDHT;

int soilMoist = 0;
boolean pumpStatus = 0;

int timePumpOn = 10; 
long sampleTimingSeconds = 50;
long startTiming = 0;
long elapsedTime = 0;
SimpleTimer timer;
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  aplyCmd();
  Serial.begin(115200);
  Serial.println("Do An!");
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  startTimers();
}

void loop() {
  timer.run(); 
   Blynk.run();
}
BLYNK_WRITE(3) // Điều khiển bơm
{
  int i = param.asInt();
  if (i == 1)
  {
    pumpStatus = !pumpStatus;
    aplyCmd();
  }
}

void getSoilMoist(void)
{
  int i = 0;
  soilMoist = 0;
  for (i = 0; i < 10; i++)  //
  {
    soilMoist += analogRead(SOIL_MOIST_1_PIN); 
    delay(50);   // Đợi đọc giá trị ADC
  }
  soilMoist = soilMoist / (i);
  soilMoist = map(soilMoist, 1023, 0, 0, 100); //Ít nước:0%  ==> Nhiều nước 100%
}

void getDhtData(void)
{

  tempDHT = dht.readTemperature();
  humDHT = dht.readHumidity();
  if (isnan(humDHT) || isnan(tempDHT))   // Kiểm tra kết nối lỗi thì thông báo.
  {
    Serial.println("Lỗi kết nối đến sensor");
    return;
  }
}
void printData(void)
{
  Serial.print("Do am: ");
  Serial.print(humDHT);
  Serial.print(" %\t");
  Serial.print("Nhiet do: ");
  Serial.print(tempDHT);
  Serial.print(" *C\t");
  Serial.print(" %\t");
  Serial.print("Do am dat: ");
 Serial.print(soilMoist);
  Serial.println(" %");
  delay(2000);
}

void aplyCmd()
{
  if (pumpStatus == 1) digitalWrite(PUMP_PIN, LOW);
  if (pumpStatus == 0) digitalWrite(PUMP_PIN, HIGH);
}


void autoControlPlantation(void)
{ 
  if (soilMoist < DRY_SOIL) 
  {
    pumpStatus = !pumpStatus;
    aplyCmd();
    delay(3000);
  }
else{
   pumpStatus = pumpStatus;
    aplyCmd();
  }
}

void startTimers(void)
{
  
  timer.setInterval(READ_AIR_DATA_TM, getDhtData);
  timer.setInterval(READ_SOIL_HUM_TM, getSoilMoist);
  timer.setInterval(SEND_UP_DATA_TM, sendUptime);
  timer.setInterval(AUTO_CTRL_TM, autoControlPlantation);
  timer.setInterval(DISPLAY_DATA_TM, printData);
}
void sendUptime()
{
  Blynk.virtualWrite(10, tempDHT); 
  Blynk.virtualWrite(11, humDHT); 
  Blynk.virtualWrite(12, soilMoist);
}
