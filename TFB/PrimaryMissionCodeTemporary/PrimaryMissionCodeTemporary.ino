//https://store.steampowered.com/app/1718240/Beaver_Clicker/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
//#include <NMEAGPS.h>

using namespace std;

Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

TinyGPSCustom pdop(gps, "GNGLL", 1); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GNGLL", 3); // $GPGSA sentence, 16th element

float temp;
float pressure;

float lat, lng, alt;

void setup() {
  Serial.print("Begin");
  Serial.begin(9600);
  while ( !Serial )    // wait for native usb
  {
    Serial.println("usb missing");
    delay(100);
  }

  InitBMP();
  //InitGPS();

  Serial.println("Callsign, BMPTemp, LMTemp, NTCTemp, BMPPressure, MPXPressure, BMPCalculatedAltitude");
}

void InitBMP() {
  unsigned status;
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();
}
void InitGPS() {
  ss.begin(GPSBaud);

  Serial.println(F("UsingCustomFields.ino"));
  Serial.println(F("Demonstrating how to extract any NMEA field using TinyGPSCustom"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop() {
  ReadBMP();
  //ReadGPS();

  float d[] = {
    temp,
    ReadLM(0, 1), //offset, sensitivity
    ReadNTC(0, 1), //off, grad
    pressure,
    ReadMPX(),
    BMPAltutude(991)
    //lat,
    //lng,
    //alt
    //SatelitesInUse()
  };

  OutputData(d, 6);

  delay(500);

  while (ss.available() > 0)
    gps.encode(ss.read());
}

#pragma region readSensors
void ReadBMP() {
  float f = bmp.readTemperature();
  
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  temp = temp_event.temperature;
  pressure = pressure_event.pressure;
}
float BMPAltutude(float seaPressure){
  return bmp.readAltitude(seaPressure);
}

float ReadMPX() {
  float volt = BitToVolt(1);
  return 10*(volt/(0.009*5)+(0.095/0.009));
}

float ReadLM(float lmOffset, float lmSens){
  float volt = BitToVolt(2);
  return (volt - lmOffset)*lmSens;
}

float ReadNTC(float ntcOff, float ntcGrad) {
  float bit = analogRead(A0);
  float volt = BitToVolt(0);
  return ntcOff + ntcGrad * volt;
}

void ReadGPS() {
  /*fix = gps.read();
  lat = fix.latitude();
  lng = fix.longitude();
  alt = fix.altitude();*/
  lat = atof(pdop.value())/100;
  lng = atof(hdop.value())/100;
  //alt = gps.altitude.meters();
}

float SatelitesInUse() {
  int x = gps.satellites.value();
  if(x == 0) {
    Serial.println();
    Serial.print("No satelites found.");
  }
  return x;
}
#pragma endregion readSensors

void OutputData(float data[], int lenght) {
  Serial.println();
  Serial.print("SHH ");

  for(int i = 0; i < lenght; i++)
  {
    if(i == 6 || i == 7) Serial.print(data[i], 6);
    else Serial.print(data[i]);
    if(i != lenght-1) Serial.print(", ");
  }
}

float BitToVolt(int n){    //Function to convert raw ADC-data (0-255) to volt
  int raw = analogRead(n);
  float volt = (float)raw*5.000/1023;
  return volt;
}

/*
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis(); // Hent antall millisekunder siden reset av Arduino-kortet
  do // Testen gjøres etter loopen er kjørt
  {
    while (ss.available())gps.encode(ss.read());
  }
  while (millis() - start < ms); // Testen gjøres før loopen er kjørt
}*/
