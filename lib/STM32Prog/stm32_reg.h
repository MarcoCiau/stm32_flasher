#ifndef STM32_REG_H
#define STM32_REG_H
#define STERR "ERROR"

#define STM32_ACK_STATUS 0x79  //  ACK status
#define STM32_NACK_STATUS 0x1F //  NACK status
#define STM32_ERROR_STATUS 0   //  ERROR status

#define STM32_INIT_CMD 0x7F        // Send Init command
#define STM32_GET_CMD 0x00         // get version command
#define STM32_GET_VERSION_CMD 0x01 // get read protection status           never used in here
#define STM32_GET_ID_CMD 0x02      // get chip ID command

#define STM32_WRITE_MEMORY_CMD 0x31      // Write flash command

#define STM32RUN 0x21     // Restart and Run programm
#define STM32RD 0x11      // Read flash command                   never used in here
#define STM32WR 0x31      // Write flash command
#define STM32ERASE 0x43   // Erase flash command
#define STM32ERASEN 0x44  // Erase extended flash command
#define STM32WP 0x63      // Write protect command                never used in here
#define STM32WP_NS 0x64   // Write protect no-stretch command     never used in here
#define STM32UNPCTWR 0x73 // Unprotect WR command                 never used in here
#define STM32UW_NS 0x74   // Unprotect write no-stretch command   never used in here
#define STM32RP 0x82      // Read protect command                 never used in here
#define STM32RP_NS 0x83   // Read protect no-stretch command      never used in here
#define STM32UR 0x92      // Unprotect read command               never used in here
#define STM32UR_NS 0x93   // Unprotect read no-stretch command    never used in here

#define STM32_START_ADDR 0x8000000 // STM32 codes start address, you can change to other address if use custom bootloader like 0x8002000

#endif