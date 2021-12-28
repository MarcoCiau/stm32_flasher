#ifndef STM32_PROG_H
#define STM32_PROG_H
#include <Arduino.h>
class STM32Prog
{
private:
    /* uart port response timeout timer */
    uint32_t responseTimer;
    /* Reset STM32 Chip */
    void reset();
    /* Send command to STM32 Chip */
    void sendCommand(uint8_t cmd);
    /* Helper: parse & debug stm32 pid */
    void parsePID(uint16_t pid);
    /* Helper: calc checksum for an buffer */
    uint8_t calcChecksum(uint8_t * data, uint8_t len);
    /* Helper: encode an mem addr */
    void encodeAddr(uint32_t addr, uint8_t *result);
    /* Helper: write memory block at specific address */
    bool writeMemoryBlock(uint32_t addr, uint8_t* data, uint8_t len);
    /* Helper: wait for a STM32's response via uart */
    bool waitForResponse();

public:
    STM32Prog(/* args */);
    ~STM32Prog();
    void begin();
    /* Init the STM32 Chip to Prog Mode */
    void init();
    /* Set the STM32 Chip to Normal Mode */
    void release();

    /* STM32 Programmer API */
    /*Allows us to get the version of the bootloader and the supported commands (setion 3.1). */
    bool sendGetCommand();
    /*is used to get the bootloader version and the read protection status (section 3.2) */
    bool sendGetVersionCommand();
    // */is used to get the version of the chip ID (identification) (section 3.3) */
    bool sendGetIDCommand();
    /*is used to write data to any valid memory address i.e. RAM, Flash memory, or option byte area. */
    bool sendWriteMemoryCommand();
};
extern STM32Prog stm32Programmer;
#endif