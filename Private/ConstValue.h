#ifndef __CONSTVALUE_H
#define __CONSTVALUE_H


#define FLASH_APP_ADDR_OFFSET 1024

#define FLAG_TO_BOOT 0
#define FLAG_TO_APP 1
#define FLAG_TO_RESET 2
#define FLASH_ADDR_BOOT	((uint32_t)0x08000000) 
#define FLASH_ADDR_UPDATE_FLAG ((uint32_t)0x0807BC00)  //��ű�־λ
#define FLASH_VTOR_OFFSET  ((uint32_t)0x10000)

enum ServerToStm32
{
    SERIAL_CODE_SET_STM32_TO_BOOT = 0,
    SERIAL_CODE_SET_STM32_TO_UPDATE,
    SERIAL_CODE_SET_STM32_UPDATE_PACKEG

};
enum Stm32ToServer
{
    SERIAL_CODE_STM32_UPDATE_PREPAR_BOOT_OK,
    SERIAL_CODE_STM32_UPDATE_PREPAR_OK,
    SERIAL_CODE_STM32_UPDATE_NEXT_PACKEG,
    SERIAL_CODE_STM32_UPDATE_CURRENT_PACKEG,
    SERIAL_CODE_STM32_UPDATE_SUCCESS
};

#endif

