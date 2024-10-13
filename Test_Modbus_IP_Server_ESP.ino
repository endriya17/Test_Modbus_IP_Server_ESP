#ifdef ESP8266 //Pilih Board Lolin (Wemos) D1 R1
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h> //ESP32
#ifndef STASSID
#define STASSID "Experia"
#define STAPSK "12345678"
#endif

#define LedPin 2

const int minValue = 0;
const int maxValue = 1024;

const char* ssid = STASSID;
const char* password = STAPSK;

//Modbus Holding Registers Offsets
const int TEST_HREG1 = 100;
const int TEST_HREG2 = 101;
const int TEST_HREG3 = 102;

//Modbus Input Registers Offsets
const int SENSOR_IREG1 = 100;
const int SENSOR_IREG2 = 102;
const int SENSOR_IREG3 = 104;

//Nilai HReg
int HREG1 = 100;
int HREG2 = 200;
int HREG3 = 300;

//ModbusIP object
ModbusIP mb;

long ts;

void connectWifi()
{
  while(WiFi.status() != WL_CONNECTED) {
    //digitalWrite(LedPin, HIGH); //Matikan Led Board Lolin (Wemos) D1 R1
    digitalWrite(LedPin, LOW); //Matikan led Board ESP32
    delay(500);
    Serial.print(".");
    //Konek Wifi
    WiFi.begin(ssid, password);
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
    delay(1000);
  }

  if(WiFi.status() == WL_CONNECTED) {
    //Tampilkan pada Serial Monitor jika dapat terhubung
    Serial.println("");
    Serial.print("Terhubung:");
    Serial.println(STASSID);
    Serial.println("Alamat IP: ");
    Serial.println(WiFi.localIP());
    //digitalWrite(LedPin, LOW); //nyalakan Led Board Lolin (Wemos) D1 R1
    digitalWrite(LedPin, HIGH); //nyalakan led Board ESP32
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);

  //Mode Wifi Ke Station
  WiFi.mode(WIFI_STA);

  mb.server(); //initial modbus server
  
  connectWifi();
  
  // Tambahkan alamat HREG, dan tulis nilai awal
  mb.addHreg(TEST_HREG1, HREG1);
  mb.addHreg(TEST_HREG2, HREG2);
  mb.addHreg(TEST_HREG3, HREG3);

  // Tambahkan alamat IReg
  mb.addIreg(SENSOR_IREG1);
  mb.addIreg(SENSOR_IREG2);
  mb.addIreg(SENSOR_IREG3);

  ts = millis();
}

void loop() {
 
  while(WiFi.status() != WL_CONNECTED) {
  Serial.println("Connection Lost");
  //digitalWrite(LedPin, HIGH); //Matikan Led Board Lolin (Wemos) D1 R1
  digitalWrite(LedPin, LOW); //Matikan led Board ESP32
  delay(2000);
  Serial.print("Connecting WiFi");
  connectWifi();
  }

  //Dummy nilai sensor IReg
  int sensor1 = random(minValue, maxValue + 1);
  int sensor2 = random(minValue, maxValue + 1);
  int sensor3 = random(minValue, maxValue + 1);
  
  //Call once inside loop() - all magic here
  mb.task();
  
  //Read each two seconds
  if (millis() > ts + 2000) {
    ts = millis();
    //Tampilkan Dummy sensor ke HReg
    mb.Hreg(TEST_HREG1);
    mb.Hreg(TEST_HREG2);
    mb.Hreg(TEST_HREG2);

    //Tampilkan Dummy sensor ke IReg (0-1024)
    mb.Ireg(SENSOR_IREG1, sensor1);  //analogRead(A0)
    mb.Ireg(SENSOR_IREG2, sensor2);
    mb.Ireg(SENSOR_IREG3, sensor3);

    //Tampilkan nilai Dummy Sensor
    Serial.print("IReg1: ");Serial.print(mb.Ireg(SENSOR_IREG1));
    Serial.print("|IReg2: ");Serial.print(mb.Ireg(SENSOR_IREG2));
    Serial.print("|IReg3: ");Serial.println(mb.Ireg(SENSOR_IREG3));

    Serial.print("HREG1: ");Serial.print(mb.Hreg(TEST_HREG1));
    Serial.print("|HREG2: ");Serial.print(mb.Hreg(TEST_HREG2));
    Serial.print("|HREG3: ");Serial.println(mb.Hreg(TEST_HREG3));
  }

  delay(10);
}
