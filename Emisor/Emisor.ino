#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_BMP280.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
 
#define SS         8
#define RST        4
#define DI0        7
#define BAND       433E6  
#define PABOOST    true

#define BMP_SCK    15
#define BMP_MISO   14
#define BMP_MOSI   16
#define BMP_CS     5

static const int RXPin = 10, TXPin = 9;
static const uint32_t GPSBaud = 9600;

int contador = 0, hora=0, minuto=0;
 
Adafruit_BMP280 bmp(BMP_CS);
TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);


void setup() {
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // Retardo nativo de usb
  
  LoRa.setPins(SS,RST,DI0);   //  Pines de LoRa           

  if (!LoRa.begin(BAND)) {
    Serial.println("Inicio de LoRa fallido!");
    while (1);
  }
  Serial.println("LoRa Iniciado!");

  //GPS
  ss.begin(GPSBaud);

  //Sensor BMP280
  unsigned estado;
  estado = bmp.begin();
  if (!estado) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}
 
void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      displayInfo();
      enviarLORA();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10){
    Serial.println(F("GPS NO detectado"));
  }
}


void displayInfo(){
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("DATO INVALIDO"));
  }
  Serial.println();
  Serial.print(F("FECHA/HORA: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("DATO INVALIDO"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour()-5);
    hora=(gps.time.hour()-5);
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    minuto=(gps.time.minute());
    Serial.print(F(" "));
   
  }
  else
  {
    Serial.print(F("DATO INVALIDO"));
  }

  Serial.println();
  Serial.print(F("Temperatura = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" C");
  Serial.print(F("Presion = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  Serial.print(F("Altura aprox = "));
  Serial.print(bmp.readAltitude(1013.25));
  Serial.println(" m");
}

void enviarLORA(){
  Serial.print("Enviando paquete: ");
  Serial.println(contador);
  
  LoRa.beginPacket();
  
  LoRa.print("#");
  LoRa.print(bmp.readTemperature());

  LoRa.print("@");

  LoRa.print(bmp.readPressure());
  LoRa.print("@");

  LoRa.print(bmp.readAltitude(1013.25));
  LoRa.print("@");

  LoRa.print(gps.location.lat(), 6);
  LoRa.print("@");
  LoRa.print(gps.location.lng(), 6);
  LoRa.print("@");

  LoRa.print(gps.date.month());
  LoRa.print(F("/"));
  LoRa.print(gps.date.day());
  LoRa.print(F("/"));
  LoRa.print(gps.date.year());
  LoRa.print("@");

  LoRa.print(hora);
  LoRa.print(":");
  LoRa.print(minuto);
  LoRa.print("@");
  
  LoRa.print("*");

  LoRa.endPacket();

  contador++;

  delay(3000);
}