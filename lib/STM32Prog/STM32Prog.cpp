#include "STM32Prog.h"
#include <ESPDebug.h>
#include "stm32_reg.h"

#define NRST 13
#define BOOT0 4
#define LED 16

STM32Prog::STM32Prog(/* args */)
{
    responseTimer = millis();
}

STM32Prog::~STM32Prog()
{
}

void STM32Prog::reset()
{
    digitalWrite(NRST, 0);
    delay(100);
    digitalWrite(NRST, 1);
    delay(500);
}

void STM32Prog::sendCommand(uint8_t cmd)
{
    Serial.write(cmd);
    Serial.write(cmd ^ 0xFF);
}

void STM32Prog::parsePID(uint16_t pid)
{
    switch (pid)
    {
    case 0x412:
        debugSerial.println("STM32 Low-density");
        break;
    case 0x410:
        debugSerial.println("STM32 Medium-density");
        break;
    case 0x414:
        debugSerial.println("STM32 High-density");
        break;
    default:
        debugSerial.println("Unknow PID");
        break;
    }
}

void STM32Prog::encodeAddr(uint32_t addr, uint8_t *result)
{
    uint8_t addrByte[5];
    for (uint8_t i = 0; i < 4; i++)
    {
        addrByte[i] = (addr >> (i * 8)) && 0xFF;
    }
    addrByte[4] = addrByte[3] ^ addrByte[2] ^ addrByte[1] ^ addrByte[0];
    result = addrByte;
}

bool STM32Prog::waitForResponse()
{
    bool timeoutEvent = false;
    responseTimer = millis();
    while (!timeoutEvent)
    {
        timeoutEvent = (millis() - responseTimer > 500);
        if (Serial.available() > 0)
        {
            break;
        }
    }
    if (timeoutEvent)
    {
        return false;
    }

    return true;
}

void STM32Prog::begin()
{
    //ESP8266->STM32 Pin Control Setup
    pinMode(BOOT0, OUTPUT);
    pinMode(NRST, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    init();
    delay(100);
    sendGetCommand();
    delay(100);
    sendGetVersionCommand();
    delay(100);
    sendGetIDCommand();
    delay(100);
    sendWriteMemoryCommand();
    delay(100);
    release();
}

void STM32Prog::init()
{
    digitalWrite(BOOT0, 1);
    reset();
    Serial.write(STM32_INIT_CMD);
    uint8_t result = STM32_ERROR_STATUS;
    if (!waitForResponse())
    {
        debugSerial.println("Init Chip Failed: UART no response!");
        return;
    }
    result = Serial.read();
    debugSerial.printf("\nInit in Flash Mode Result : %#x\n", result);
    if (result != STM32_ACK_STATUS)
    {
        debugSerial.println("Init Chip Failed : Invalid code response !");
        return;
    }
}

void STM32Prog::release()
{
    digitalWrite(BOOT0, 0);
    reset();
}

bool STM32Prog::sendGetCommand()
{
    sendCommand(STM32_GET_CMD);

    if (!waitForResponse())
    {
        debugSerial.println("Get Command Failed: UART no response!");
        return false;
    }

    // uint8_t result = STM32_ERROR_STATUS;
    // result = Serial.read();
    uint8_t rxBuffer[15];
    Serial.readBytes(rxBuffer, 15);
    Serial.flush();
    debugSerial.printf("Get Command Result : %#x\n", rxBuffer[0]);

    if (rxBuffer[0] != STM32_ACK_STATUS)
    {
        debugSerial.println("Get Command Failed!");
        Serial.flush();
        return false;
    }
    uint8_t len = rxBuffer[1];
    if (len <= 0)
    {
        debugSerial.println("Invalid Bytes Len!");
        return false;
    }

    debugSerial.printf("\nBytes len : %u\n", rxBuffer[1]);
    debugSerial.printf("bootloader version : %#x\n", rxBuffer[2]);
    debugSerial.println("supported commands :");
    for (uint8_t i = 2; i < 15; i++)
    {
        debugSerial.printf("cmd%u : %#x\n", i, rxBuffer[i]);
    }
    debugSerial.print("\n\n");
    return true;
}

bool STM32Prog::sendGetVersionCommand()
{
    sendCommand(STM32_GET_VERSION_CMD);

    if (!waitForResponse())
    {
        debugSerial.println("Get Version Failed: UART no response!");
        return false;
    }
    uint8_t rxBuffer[5];
    Serial.readBytes(rxBuffer, 5);
    Serial.flush();
    debugSerial.printf("Get Version Result : %#x\n", rxBuffer[0]);

    if (rxBuffer[0] != STM32_ACK_STATUS)
    {
        debugSerial.println("Get Version Failed!");
        Serial.flush();
        return false;
    }

    debugSerial.printf("bootloader version : %#x\n", rxBuffer[1]);
    debugSerial.printf("Option Byte 1 : %#x\n", rxBuffer[2]);
    debugSerial.printf("Option Byte 2 : %#x\n", rxBuffer[3]);
    debugSerial.printf("ACK : %#x\n", rxBuffer[4]);
    debugSerial.print("\n\n");
    return true;
}

bool STM32Prog::sendGetIDCommand()
{
    sendCommand(STM32_GET_ID_CMD);

    if (!waitForResponse())
    {
        debugSerial.println("Get ID Failed: UART no response!");
        return false;
    }

    uint8_t rxBuffer[5];
    Serial.readBytes(rxBuffer, 5);
    Serial.flush();
    debugSerial.printf("Get ID Result : %#x\n", rxBuffer[0]);

    if (rxBuffer[0] != STM32_ACK_STATUS)
    {
        debugSerial.println("Get ID Failed!");
        Serial.flush();
        return false;
    }
    uint16_t stm32PID = (rxBuffer[2] << 8) | rxBuffer[3];
    debugSerial.printf("Device PID : %#x\n", stm32PID);
    parsePID(stm32PID);
#ifdef DEBUG_DETAILS
    for (uint8_t i = 0; i < 4; i++)
    {
        debugSerial.printf("data%u : %#x\n", i, rxBuffer[i]);
    }
    debugSerial.print("\n\n");
#endif
    return true;
}

bool STM32Prog::sendWriteMemoryCommand()
{
    //send command
    sendCommand(STM32_WRITE_MEMORY_CMD);

    if (!waitForResponse())
    {
        debugSerial.println("Write Memory Failed: UART no response!");
        return false;
    }
    //check if an ACK is received
    uint8_t result = STM32_ERROR_STATUS;
    result = Serial.read();
    debugSerial.printf("\nWrite Memory Result : %#x\n", result);
    if (result != STM32_ACK_STATUS)
    {
        debugSerial.println("Write Memory Failed : Invalid code response !");
        return false;
    }
    return true;
}

STM32Prog stm32Programmer;