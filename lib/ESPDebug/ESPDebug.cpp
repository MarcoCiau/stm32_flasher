#include "ESPDebug.h"
#include <Arduino.h>

SoftwareSerial debugSerial (DEBUG_RX_PIN, DEBUG_TX_PIN, false);

void debugSerialBegin(int baud)
{
    debugSerial.begin(baud);
}