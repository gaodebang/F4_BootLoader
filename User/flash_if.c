/*
*********************************************************************************************************
*	                                  
*	模块名称 : flash_if模块
*	文件名称 : flash_if.c
*	版    本 : V1.0
*	说    明 : flash编程驱动
*
*********************************************************************************************************
*/
#include <string.h>
#include "flash_if.h"
#include "stm32f4xx.h"

uc32 RANDOM_BOOTLOADER_NEW[4] = {236352785, 3636502030, 2001483073, 261799706};
static uint32_t GetSector(uint32_t Address);

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Init
*	功能说明: flash解锁后可进行写操作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_If_Init(void)
{ 
	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_New_App
*	功能说明: 检测是否有新的App程序需要更新到主程序区域
*	形    参: 无
*	返 回 值: 0 无新程序需要更新
*             1 有新程序需要更新
*********************************************************************************************************
*/
uint8_t FLASH_If_New_App(void)
{ 
	if(memcmp(RANDOM_BOOTLOADER_NEW, (uc8 *)BOOTLOADER_FLAG_ADDRESS, 16)==0)//与事先保存的随机序列进行比较
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_Check_BackUp_App_CRC
*	功能说明: 检测备份程序存储区的CRC值
*	形    参: App_Blank 程序存储区
*             BackUp_App_Blank 备份程序存储区
*	返 回 值: 0 校验失败
*             1 校验通过
*********************************************************************************************************
*/
uint8_t FLASH_Check_CRC(Blank_Mark flash_blank)
{ 
	CRC_ResetDR();	//复位CRC数据寄存器
	if(flash_blank == App_Blank)
	{
		if(CRC_CalcBlockCRC((uint32_t *)APPLICATION_ADDRESS, APPLICATION_LENGTH - 1) == *(__IO uint32_t*) (APPLICATION_END_ADDRESS -3))//校验程序区域CRC
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(flash_blank == BackUp_App_Blank)
	{
		if(CRC_CalcBlockCRC((uint32_t *)APPLICATION_BACKUP_ADDRESS, APPLICATION_BACKUP_LENGTH - 1) == *(__IO uint32_t*) (APPLICATION_BACKUP_END_ADDRESS -3))//校验备份程序区域CRC
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Erase
*	功能说明: 擦除Flash区域
*	形    参: BootLoader_Blank 程序存储区
*             App_Blank 备份程序存储区
*             BackUp_App_Blank 备份程序存储区
*	返 回 值: 0 flash区成功擦除
*             1 擦除失败
*********************************************************************************************************
*/
uint8_t FLASH_If_Erase(Blank_Mark flash_blank)
{
	uint32_t UserStartSector = 0, i = 0;
	if(flash_blank == BootLoader_Blank)
	{
		UserStartSector = GetSector(BOOTLOADER_FLAG_ADDRESS);
		if (FLASH_EraseSector(UserStartSector, VoltageRange_3) != FLASH_COMPLETE)
		{
			return 1;
		}
	}
	else if(flash_blank == App_Blank)
	{		
		UserStartSector = GetSector(APPLICATION_ADDRESS);
		for(i = UserStartSector; i <= FLASH_Sector_8; i += 8)
		{
			if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
			{
				return 1;
			}
		}
	}
	else if(flash_blank == BackUp_App_Blank)
	{		
		UserStartSector = GetSector(APPLICATION_BACKUP_ADDRESS);
		for(i = UserStartSector; i <= FLASH_Sector_11; i += 8)
		{
			if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
			{
				return 1;
			}
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Write
*	功能说明: 向flash中写数据，4字节对齐
*	形    参: FlashAddress flash起始地址
*             Data         数据地址
*             DataLength   数据长度（4字节）
*	返 回 值: 0 成功
*             1 写flash过程中出错
*             2 写到flash中的数据跟读出的不相同
*********************************************************************************************************
*/
uint32_t FLASH_If_Write(uint32_t FlashAddressDest, uint32_t FlashAddressSrc)
{
	uint32_t i = 0;
	for (i = 0; i < APPLICATION_BACKUP_LENGTH; i++)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		be done by word */  
		if (FLASH_ProgramWord(FlashAddressDest + 4*i, *(__IO uint32_t*) (FlashAddressSrc + 4*i)) == FLASH_COMPLETE)
		{
			/* 检查写入的数据 */
			if (*(__IO uint32_t*) (FlashAddressDest + 4*i) != *(__IO uint32_t*) (FlashAddressSrc + 4*i))
			{
				/* 读出的和写入的不相同 */
				return(2);
			}
		}
		else
		{
			/* 向flash中写数据时发生错误 */
			return (1);
		}
	}
	return (0);
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_DisableWriteProtection
*	功能说明: 解锁写保护
*	形  参 1: BootLoader_Blank 程序存储区
*             App_Blank 备份程序存储区
*             BackUp_App_Blank 备份程序存储区
*	形  参 2: ENSABLE 设置写保护
*             DISABLE 清除写保护
*	返 回 值: 0 操作成功
*             1 操作失败
*********************************************************************************************************
*/
uint8_t FLASH_If_ConfigWriteProtection(Blank_Mark flash_blank, FunctionalState NewState)
{
	if(flash_blank == BootLoader_Blank)
	{
		FLASH_OB_WRPConfig(OB_WRP_Sector_4, NewState);
		/* 进行 Option Bytes 编程. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* 解保护失败 */
			return 1;
		}
	}
	else if(flash_blank == App_Blank)
	{
		FLASH_OB_WRPConfig(OB_WRP_Sector_6 | OB_WRP_Sector_7 | OB_WRP_Sector_8, NewState);
		/* 进行 Option Bytes 编程. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* 解保护失败 */
			return 1;
		}
	}
	else if(flash_blank == BackUp_App_Blank)
	{		
		FLASH_OB_WRPConfig(OB_WRP_Sector_9 | OB_WRP_Sector_10 | OB_WRP_Sector_11, NewState);
		/* 进行 Option Bytes 编程. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* 解保护失败 */
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: GetSector
*	功能说明: 根据地址计算扇区首地址
*	形    参: Address  指定地址
*	返 回 值: 扇区首地址
*********************************************************************************************************
*/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;  
	}
	return sector;
}
