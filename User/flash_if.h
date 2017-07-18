/*
*********************************************************************************************************
*	                                  
*	模块名称 : flash_if模块
*	文件名称 : flash_if.c
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

#include "stm32f4xx.h"

typedef enum 
{
	BootLoader_Blank = 1, 
	App_Blank = 2, 
	BackUp_App_Blank = 3
}Blank_Mark;

extern uc32 RANDOM_BOOTLOADER_NEW[];

/* 各个扇区首地址 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

/* flash结束地址 */
#define FLASH_END_ADDRESS       		0x080FFFFF

/* BootLoader起始地址，使用前四个扇区SECTOR_0——SECTOR_3 */
#define BOOTLOADER_START_ADDRESS        ADDR_FLASH_SECTOR_0
/* BootLoader数据存储区起始地址，使用SECTOR_4 */
#define BOOTLOADER_FLAG_ADDRESS        	ADDR_FLASH_SECTOR_4

/* App数据存储区起始地址，使用SECTOR_5 */
#define APPLICATION_DATA_ADDRESS        ADDR_FLASH_SECTOR_5

/* App程序存储区起始地址，使用SECTOR_6——SECTOR_8 */
#define APPLICATION_ADDRESS				ADDR_FLASH_SECTOR_6
/* App程序存储区结束地址*/
#define APPLICATION_END_ADDRESS			(ADDR_FLASH_SECTOR_9 - 1)
/* 程序存储区长度 uint32*/
#define APPLICATION_LENGTH				((APPLICATION_END_ADDRESS - APPLICATION_ADDRESS + 1)/4)

/* 备份App程序存储区起始地址，使用SECTOR_9——SECTOR_11 */
#define APPLICATION_BACKUP_ADDRESS		ADDR_FLASH_SECTOR_9
/* App程序存储区结束地址*/
#define APPLICATION_BACKUP_END_ADDRESS	FLASH_END_ADDRESS
/* 备份程序存储区长度 uint32*/
#define APPLICATION_BACKUP_LENGTH		((APPLICATION_BACKUP_END_ADDRESS - APPLICATION_BACKUP_ADDRESS + 1)/4)


/* 供外部文件调用的函数 */
void FLASH_If_Init(void);
uint8_t FLASH_If_New_App(void);
uint8_t FLASH_Check_CRC(Blank_Mark flash_blank);
uint8_t FLASH_If_ConfigWriteProtection(Blank_Mark flash_blank, FunctionalState NewState);
uint8_t FLASH_If_Erase(Blank_Mark flash_blank);


uint32_t FLASH_If_Write(uint32_t FlashAddressDest, uint32_t FlashAddressSrc);


#endif  /* __FLASH_IF_H */
