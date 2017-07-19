/*
*********************************************************************************************************
*	                                  
*	ģ������ : flash_ifģ��
*	�ļ����� : flash_if.c
*	��    �� : V1.0
*	˵    �� : flash�������
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
*	�� �� ��: FLASH_If_Init
*	����˵��: flash������ɽ���д����
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: FLASH_If_New_App
*	����˵��: ����Ƿ����µ�App������Ҫ���µ�����������
*	��    ��: ��
*	�� �� ֵ: 0 ���³�����Ҫ����
*             1 ���³�����Ҫ����
*********************************************************************************************************
*/
uint8_t FLASH_If_New_App(void)
{ 
	if(memcmp(RANDOM_BOOTLOADER_NEW, (uc8 *)BOOTLOADER_FLAG_ADDRESS, 16)==0)//�����ȱ����������н��бȽ�
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
*	�� �� ��: FLASH_Check_BackUp_App_CRC
*	����˵��: ��ⱸ�ݳ���洢����CRCֵ
*	��    ��: App_Blank ����洢��
*             BackUp_App_Blank ���ݳ���洢��
*	�� �� ֵ: 0 У��ʧ��
*             1 У��ͨ��
*********************************************************************************************************
*/
uint8_t FLASH_Check_CRC(Blank_Mark flash_blank)
{ 
	CRC_ResetDR();	//��λCRC���ݼĴ���
	if(flash_blank == App_Blank)
	{
		if(CRC_CalcBlockCRC((uint32_t *)APPLICATION_ADDRESS, APPLICATION_LENGTH - 1) == *(__IO uint32_t*) (APPLICATION_END_ADDRESS -3))//У���������CRC
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
		if(CRC_CalcBlockCRC((uint32_t *)APPLICATION_BACKUP_ADDRESS, APPLICATION_BACKUP_LENGTH - 1) == *(__IO uint32_t*) (APPLICATION_BACKUP_END_ADDRESS -3))//У�鱸�ݳ�������CRC
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
*	�� �� ��: FLASH_If_Erase
*	����˵��: ����Flash����
*	��    ��: BootLoader_Blank ����洢��
*             App_Blank ���ݳ���洢��
*             BackUp_App_Blank ���ݳ���洢��
*	�� �� ֵ: 0 flash���ɹ�����
*             1 ����ʧ��
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
*	�� �� ��: FLASH_If_Write
*	����˵��: ��flash��д���ݣ�4�ֽڶ���
*	��    ��: FlashAddress flash��ʼ��ַ
*             Data         ���ݵ�ַ
*             DataLength   ���ݳ��ȣ�4�ֽڣ�
*	�� �� ֵ: 0 �ɹ�
*             1 дflash�����г���
*             2 д��flash�е����ݸ������Ĳ���ͬ
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
			/* ���д������� */
			if (*(__IO uint32_t*) (FlashAddressDest + 4*i) != *(__IO uint32_t*) (FlashAddressSrc + 4*i))
			{
				/* �����ĺ�д��Ĳ���ͬ */
				return(2);
			}
		}
		else
		{
			/* ��flash��д����ʱ�������� */
			return (1);
		}
	}
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_DisableWriteProtection
*	����˵��: ����д����
*	��  �� 1: BootLoader_Blank ����洢��
*             App_Blank ���ݳ���洢��
*             BackUp_App_Blank ���ݳ���洢��
*	��  �� 2: ENSABLE ����д����
*             DISABLE ���д����
*	�� �� ֵ: 0 �����ɹ�
*             1 ����ʧ��
*********************************************************************************************************
*/
uint8_t FLASH_If_ConfigWriteProtection(Blank_Mark flash_blank, FunctionalState NewState)
{
	if(flash_blank == BootLoader_Blank)
	{
		FLASH_OB_WRPConfig(OB_WRP_Sector_4, NewState);
		/* ���� Option Bytes ���. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* �Ᵽ��ʧ�� */
			return 1;
		}
	}
	else if(flash_blank == App_Blank)
	{
		FLASH_OB_WRPConfig(OB_WRP_Sector_6 | OB_WRP_Sector_7 | OB_WRP_Sector_8, NewState);
		/* ���� Option Bytes ���. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* �Ᵽ��ʧ�� */
			return 1;
		}
	}
	else if(flash_blank == BackUp_App_Blank)
	{		
		FLASH_OB_WRPConfig(OB_WRP_Sector_9 | OB_WRP_Sector_10 | OB_WRP_Sector_11, NewState);
		/* ���� Option Bytes ���. */  
		if (FLASH_OB_Launch() != FLASH_COMPLETE)
		{
			/* �Ᵽ��ʧ�� */
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: GetSector
*	����˵��: ���ݵ�ַ���������׵�ַ
*	��    ��: Address  ָ����ַ
*	�� �� ֵ: �����׵�ַ
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
