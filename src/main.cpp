#include <Arduino.h>
#include <ESPDebug.h>
#include <ESPUpdate.h>
#include <STM32Prog.h>

void setup()
{

  Serial.begin(115200, SERIAL_8E1);
  debugSerialBegin(115200); //Initialize software serial with baudrate of 115200s
  SPIFFS.begin();
  //Init Server
  debugSerial.println("Server Init");
  espOTA.begin();
  //Init STM32 Programmger
  debugSerial.println("STM32 Programmger Init");
  stm32Programmer.begin();
}

void loop()
{
  espOTA.loop();
}
