#ifndef ESP_DEBUG_H
#define ESP_DEBUG_H
#include <Arduino.h>
#include <SoftwareSerial.h>
#define DEBUG_RX_PIN 12
#define DEBUG_TX_PIN 14

extern SoftwareSerial debugSerial;
extern void debugSerialBegin(int baud);
#endif //ESP_DEBUG_H