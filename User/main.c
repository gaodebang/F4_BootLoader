/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 主程序文件
*
*********************************************************************************************************
*/
#include "flash_if.h"

/* 变量 */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
uint32_t Err_Flag = 0;
int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

	FLASH_If_Init();	/* 初始化flash操作 */
	if(FLASH_If_New_App() == 1)	/* 检查是否有新程序需要更新 */
	{
		if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* 检查新程序CRC，新程序存放在备份区域 */
		{
			FLASH_Unlock(); /* 解锁 FLASH Control */
			FLASH_OB_Unlock(); /* 解锁 Option Bytes */
			if(FLASH_If_ConfigWriteProtection(App_Blank, DISABLE) == 0) /* 解锁应用程序存储区写保护 */
			{
				if(FLASH_If_Erase(App_Blank) == 0)	/* 擦除应用程序存储区 */
				{
					if(FLASH_If_Write(APPLICATION_ADDRESS, APPLICATION_BACKUP_ADDRESS) == 0) /* 拷贝新程序到应用程序存储区 */
					{
						if(FLASH_Check_CRC(App_Blank) == 1) /* 检查新更新的应用程序CRC */
						{
							if(FLASH_If_ConfigWriteProtection(App_Blank, ENABLE) == 0) /* 应用程序存储区写保护 */
							{
								if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, DISABLE) == 0) /* 解锁BootLoader标志区写保护 */
								{
									if(FLASH_If_Erase(BootLoader_Blank) == 0) /* 擦除BootLoader标志区 */
									{
										if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, ENABLE) == 0) /* BootLoader标志区写保护 */
										{
											FLASH_OB_Lock(); /* 锁定 Option Bytes */
											FLASH_Lock(); /* 锁定 FLASH Control */
											Err_Flag = 1;
											//NVIC_SystemReset();
										}
										else//BootLoader标志区写保护失败
										{
											Err_Flag = 9;
										}
									}
									else//擦除BootLoader标志区失败
									{
										Err_Flag = 8;
									}
								}
								else//解锁BootLoader标志区失败
								{
									Err_Flag = 7;
								}
							}
							else//应用程序存储区写保护失败
							{
								Err_Flag = 6;
							}
						}
						else//新更新程序CRC校验失败
						{
							Err_Flag = 5;
						}
					}
					else//更新程序失败
					{
						Err_Flag = 4;
					}
				}
				else//擦除应用程序存储区失败
				{
					Err_Flag = 3;
				}
			}
			else//解锁程序存储区写保护失败
			{
				Err_Flag = 2;
			}
			FLASH_OB_Lock(); /* 锁定 Option Bytes */
			FLASH_Lock(); /* 锁定 FLASH Control */
		}
		else//新程序CRC校验失败
		{
			if(FLASH_Check_CRC(App_Blank) == 1)	/* 检查应用程序CRC */
			{
				FLASH_Unlock(); /* 解锁 FLASH Control */
				FLASH_OB_Unlock(); /* 解锁 Option Bytes */
				if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, DISABLE)) /* 解锁备份程序存储区写保护 */
				{
					if(FLASH_If_Erase(BackUp_App_Blank) == 0)	/* 擦除备份程序程序存储区 */
					{
						if(FLASH_If_Write(APPLICATION_BACKUP_ADDRESS, APPLICATION_ADDRESS) == 0) /* 拷贝应用程序到备份程序存储区 */
						{
							if(FLASH_Check_CRC(BackUp_App_Blank) == 1) /* 检查备份程序区域的CRC */
							{
								if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, ENABLE) == 0) /* 备份程序存储区写保护 */
								{
									if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, DISABLE) == 0) /* 解锁BootLoader标志区写保护 */
									{
										if(FLASH_If_Erase(BootLoader_Blank) == 0) /* 检查擦除BootLoader标志区是否成功 */
										{
											if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, ENABLE) == 0) /* BootLoader标志区写保护 */
											{
												FLASH_OB_Lock(); /* 锁定 Option Bytes */
												FLASH_Lock(); /* 锁定 FLASH Control */
												Err_Flag = 1;
												//NVIC_SystemReset();
											}
											else//BootLoader标志区写保护失败
											{
												Err_Flag = 9;
											}
										}
										else//擦除BootLoader标志区失败
										{
											Err_Flag = 8;
										}
									}
									else//解锁BootLoader标志区失败
									{
										Err_Flag = 7;
									}
								}
								else//备份程序存储区写保护失败
								{
									Err_Flag = 6;
								}
							}
							else//备份程序存储区CRC校验失败
							{
								Err_Flag = 5;
							}
						}
						else//备份程序失败
						{
							Err_Flag = 4;
						}
					}
					else//擦除备份程序存储区失败
					{
						Err_Flag = 3;
					}
				}
				else//解锁备份程序存储区写保护失败
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* 锁定 Option Bytes */
				FLASH_Lock(); /* 锁定 FLASH Control */
			}
			else//两个区域CRC校验都失败
			{
				Err_Flag+=10;
			}
			Err_Flag+=100;
		}
		Err_Flag+=1000;
	}
	else//无新程序需要更新
	{
		if(FLASH_Check_CRC(App_Blank) == 1)	/* 检查应用程序CRC */
		{
			if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* 检查备份程序CRC */
			{
				Err_Flag = 1;
				//NVIC_SystemReset();
			}
			else//备份程序存储区CRC校验失败
			{
				FLASH_Unlock(); /* 解锁 FLASH Control */
				FLASH_OB_Unlock(); /* 解锁 Option Bytes */
				if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, DISABLE)) /* 解锁备份程序存储区写保护 */
				{
					if(FLASH_If_Erase(BackUp_App_Blank) == 0)	/* 擦除备份程序程序存储区 */
					{
						if(FLASH_If_Write(APPLICATION_BACKUP_ADDRESS, APPLICATION_ADDRESS) == 0) /* 拷贝程序到备份程序存储区 */
						{
							if(FLASH_Check_CRC(BackUp_App_Blank) == 1) /* 检查备份程序区域的CRC */
							{
								if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, ENABLE) == 0) /* 备份程序存储区写保护 */
								{
									FLASH_OB_Lock(); /* 锁定 Option Bytes */
									FLASH_Lock(); /* 锁定 FLASH Control */
									Err_Flag = 1;
									//NVIC_SystemReset();
								}
								else//备份程序存储区写保护失败
								{
									Err_Flag = 6;
								}
							}
							else//备份程序区域CRC校验失败
							{
								Err_Flag = 5;
							}
						}
						else//备份程序失败
						{
							Err_Flag = 4;
						}
					}
					else//擦除备份程序存储区失败
					{
						Err_Flag = 3;
					}
				}
				else//解锁备份程序存储区写保护失败
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* 锁定 Option Bytes */
				FLASH_Lock(); /* 锁定 FLASH Control */
				Err_Flag+=100;
			}
		}
		else//程序存储区CRC校验失败
		{
			if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* 检查备份程序CRC */
			{
				FLASH_Unlock(); /* 解锁 FLASH Control */
				FLASH_OB_Unlock(); /* 解锁 Option Bytes */
				if(FLASH_If_ConfigWriteProtection(App_Blank, DISABLE) == 0) /* 解锁应用程序存储区写保护 */
				{
					if(FLASH_If_Erase(App_Blank) == 0)	/* 擦除应用程序存储区 */
					{
						if(FLASH_If_Write(APPLICATION_ADDRESS, APPLICATION_BACKUP_ADDRESS) == 0) /* 恢复备份程序到应用程序存储区 */
						{
							if(FLASH_Check_CRC(App_Blank) == 1) /* 检查应用程序存储区CRC */
							{
								if(FLASH_If_ConfigWriteProtection(App_Blank, ENABLE) == 0) /* 应用程序存储区写保护 */
								{
									FLASH_OB_Lock(); /* 锁定 Option Bytes */
									FLASH_Lock(); /* 锁定 FLASH Control */
									Err_Flag = 1;
									//NVIC_SystemReset();
								}
								else//应用程序存储区写保护失败
								{
									Err_Flag = 6;
								}
							}
							else//应用程序CRC校验失败
							{
								Err_Flag = 5;
							}
						}
						else//恢复程序失败
						{
							Err_Flag = 4;
						}
					}
					else//擦除应用程序区域失败
					{
						Err_Flag = 3;
					}
				}
				else//解锁应用程序存储区写保护失败
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* 锁定 Option Bytes */
				FLASH_Lock(); /* 锁定 FLASH Control */
			}
			else//两个区域CRC校验都失败
			{
				Err_Flag+=10;
			}
		}
	}
	while (1)
	{
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		/* 用户应用地址 */
		Jump_To_Application = (pFunction) JumpAddress;
		/* 初始化MSP */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}
}
