#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ (2)
#define PN532_RESET (3)
#define buzzer 9

String llaveMaestra= "15415418226";

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
void printArray(byte *buffer, byte buffersize);
void mensaje(int bandera, String leido);

void inicio()
{
  Serial.begin(9600);
  nfc.begin();
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("No se encontró ningún modulo PN53x");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Se encontró el chip PN5");
  Serial.println((versiondata>>24) & 0xFF, HEX);

  //Preparar placa para lectura de RFID
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
}

void motor()
{
  bool success;
  uint8_t uid[]= {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  success= nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success)
  {
    Serial.println("Llave encontrada");
    Serial.print("UID longitud: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID: ");
    printArray(uid, uidLength);
    Serial.println("");
    digitalWrite(buzzer, LOW);
    delay(2000);
  }
  else
  {
    Serial.println("Llave no fue encontrada");
  }
}

void printArray(byte *buffer, byte buffersize)
{
  String leido= "";
  byte i;

  for(i= 0;i < buffersize;i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    leido+= buffer[i];
  }

  if (leido== llaveMaestra)
  {
    mensaje(1, leido);
  }
  else
  {
    mensaje(0, leido);
  }
}

void mensaje(int bandera, String leido)
{
  digitalWrite(buzzer, HIGH);
  Serial.println("");
  Serial.println("UID decimal: "+leido);
  if (bandera== 1)
  {
    Serial.println("Llave autorizada");
  }
  else
  {
    Serial.println("Llave no autorizada");
  }
}
