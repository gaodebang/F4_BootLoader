/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : �������ļ�
*
*********************************************************************************************************
*/
#include "flash_if.h"

/* ���� */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
uint32_t Err_Flag = 0;
int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

	FLASH_If_Init();	/* ��ʼ��flash���� */
	if(FLASH_If_New_App() == 1)	/* ����Ƿ����³�����Ҫ���� */
	{
		if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* ����³���CRC���³������ڱ������� */
		{
			FLASH_Unlock(); /* ���� FLASH Control */
			FLASH_OB_Unlock(); /* ���� Option Bytes */
			if(FLASH_If_ConfigWriteProtection(App_Blank, DISABLE) == 0) /* ����Ӧ�ó���洢��д���� */
			{
				if(FLASH_If_Erase(App_Blank) == 0)	/* ����Ӧ�ó���洢�� */
				{
					if(FLASH_If_Write(APPLICATION_ADDRESS, APPLICATION_BACKUP_ADDRESS) == 0) /* �����³���Ӧ�ó���洢�� */
					{
						if(FLASH_Check_CRC(App_Blank) == 1) /* ����¸��µ�Ӧ�ó���CRC */
						{
							if(FLASH_If_ConfigWriteProtection(App_Blank, ENABLE) == 0) /* Ӧ�ó���洢��д���� */
							{
								if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, DISABLE) == 0) /* ����BootLoader��־��д���� */
								{
									if(FLASH_If_Erase(BootLoader_Blank) == 0) /* ����BootLoader��־�� */
									{
										if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, ENABLE) == 0) /* BootLoader��־��д���� */
										{
											FLASH_OB_Lock(); /* ���� Option Bytes */
											FLASH_Lock(); /* ���� FLASH Control */
											Err_Flag = 1;
											//NVIC_SystemReset();
										}
										else//BootLoader��־��д����ʧ��
										{
											Err_Flag = 9;
										}
									}
									else//����BootLoader��־��ʧ��
									{
										Err_Flag = 8;
									}
								}
								else//����BootLoader��־��ʧ��
								{
									Err_Flag = 7;
								}
							}
							else//Ӧ�ó���洢��д����ʧ��
							{
								Err_Flag = 6;
							}
						}
						else//�¸��³���CRCУ��ʧ��
						{
							Err_Flag = 5;
						}
					}
					else//���³���ʧ��
					{
						Err_Flag = 4;
					}
				}
				else//����Ӧ�ó���洢��ʧ��
				{
					Err_Flag = 3;
				}
			}
			else//��������洢��д����ʧ��
			{
				Err_Flag = 2;
			}
			FLASH_OB_Lock(); /* ���� Option Bytes */
			FLASH_Lock(); /* ���� FLASH Control */
		}
		else//�³���CRCУ��ʧ��
		{
			if(FLASH_Check_CRC(App_Blank) == 1)	/* ���Ӧ�ó���CRC */
			{
				FLASH_Unlock(); /* ���� FLASH Control */
				FLASH_OB_Unlock(); /* ���� Option Bytes */
				if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, DISABLE)) /* �������ݳ���洢��д���� */
				{
					if(FLASH_If_Erase(BackUp_App_Blank) == 0)	/* �������ݳ������洢�� */
					{
						if(FLASH_If_Write(APPLICATION_BACKUP_ADDRESS, APPLICATION_ADDRESS) == 0) /* ����Ӧ�ó��򵽱��ݳ���洢�� */
						{
							if(FLASH_Check_CRC(BackUp_App_Blank) == 1) /* ��鱸�ݳ��������CRC */
							{
								if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, ENABLE) == 0) /* ���ݳ���洢��д���� */
								{
									if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, DISABLE) == 0) /* ����BootLoader��־��д���� */
									{
										if(FLASH_If_Erase(BootLoader_Blank) == 0) /* ������BootLoader��־���Ƿ�ɹ� */
										{
											if(FLASH_If_ConfigWriteProtection(BootLoader_Blank, ENABLE) == 0) /* BootLoader��־��д���� */
											{
												FLASH_OB_Lock(); /* ���� Option Bytes */
												FLASH_Lock(); /* ���� FLASH Control */
												Err_Flag = 1;
												//NVIC_SystemReset();
											}
											else//BootLoader��־��д����ʧ��
											{
												Err_Flag = 9;
											}
										}
										else//����BootLoader��־��ʧ��
										{
											Err_Flag = 8;
										}
									}
									else//����BootLoader��־��ʧ��
									{
										Err_Flag = 7;
									}
								}
								else//���ݳ���洢��д����ʧ��
								{
									Err_Flag = 6;
								}
							}
							else//���ݳ���洢��CRCУ��ʧ��
							{
								Err_Flag = 5;
							}
						}
						else//���ݳ���ʧ��
						{
							Err_Flag = 4;
						}
					}
					else//�������ݳ���洢��ʧ��
					{
						Err_Flag = 3;
					}
				}
				else//�������ݳ���洢��д����ʧ��
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* ���� Option Bytes */
				FLASH_Lock(); /* ���� FLASH Control */
			}
			else//��������CRCУ�鶼ʧ��
			{
				Err_Flag+=10;
			}
			Err_Flag+=100;
		}
		Err_Flag+=1000;
	}
	else//���³�����Ҫ����
	{
		if(FLASH_Check_CRC(App_Blank) == 1)	/* ���Ӧ�ó���CRC */
		{
			if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* ��鱸�ݳ���CRC */
			{
				Err_Flag = 1;
				//NVIC_SystemReset();
			}
			else//���ݳ���洢��CRCУ��ʧ��
			{
				FLASH_Unlock(); /* ���� FLASH Control */
				FLASH_OB_Unlock(); /* ���� Option Bytes */
				if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, DISABLE)) /* �������ݳ���洢��д���� */
				{
					if(FLASH_If_Erase(BackUp_App_Blank) == 0)	/* �������ݳ������洢�� */
					{
						if(FLASH_If_Write(APPLICATION_BACKUP_ADDRESS, APPLICATION_ADDRESS) == 0) /* �������򵽱��ݳ���洢�� */
						{
							if(FLASH_Check_CRC(BackUp_App_Blank) == 1) /* ��鱸�ݳ��������CRC */
							{
								if(FLASH_If_ConfigWriteProtection(BackUp_App_Blank, ENABLE) == 0) /* ���ݳ���洢��д���� */
								{
									FLASH_OB_Lock(); /* ���� Option Bytes */
									FLASH_Lock(); /* ���� FLASH Control */
									Err_Flag = 1;
									//NVIC_SystemReset();
								}
								else//���ݳ���洢��д����ʧ��
								{
									Err_Flag = 6;
								}
							}
							else//���ݳ�������CRCУ��ʧ��
							{
								Err_Flag = 5;
							}
						}
						else//���ݳ���ʧ��
						{
							Err_Flag = 4;
						}
					}
					else//�������ݳ���洢��ʧ��
					{
						Err_Flag = 3;
					}
				}
				else//�������ݳ���洢��д����ʧ��
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* ���� Option Bytes */
				FLASH_Lock(); /* ���� FLASH Control */
				Err_Flag+=100;
			}
		}
		else//����洢��CRCУ��ʧ��
		{
			if(FLASH_Check_CRC(BackUp_App_Blank) == 1)	/* ��鱸�ݳ���CRC */
			{
				FLASH_Unlock(); /* ���� FLASH Control */
				FLASH_OB_Unlock(); /* ���� Option Bytes */
				if(FLASH_If_ConfigWriteProtection(App_Blank, DISABLE) == 0) /* ����Ӧ�ó���洢��д���� */
				{
					if(FLASH_If_Erase(App_Blank) == 0)	/* ����Ӧ�ó���洢�� */
					{
						if(FLASH_If_Write(APPLICATION_ADDRESS, APPLICATION_BACKUP_ADDRESS) == 0) /* �ָ����ݳ���Ӧ�ó���洢�� */
						{
							if(FLASH_Check_CRC(App_Blank) == 1) /* ���Ӧ�ó���洢��CRC */
							{
								if(FLASH_If_ConfigWriteProtection(App_Blank, ENABLE) == 0) /* Ӧ�ó���洢��д���� */
								{
									FLASH_OB_Lock(); /* ���� Option Bytes */
									FLASH_Lock(); /* ���� FLASH Control */
									Err_Flag = 1;
									//NVIC_SystemReset();
								}
								else//Ӧ�ó���洢��д����ʧ��
								{
									Err_Flag = 6;
								}
							}
							else//Ӧ�ó���CRCУ��ʧ��
							{
								Err_Flag = 5;
							}
						}
						else//�ָ�����ʧ��
						{
							Err_Flag = 4;
						}
					}
					else//����Ӧ�ó�������ʧ��
					{
						Err_Flag = 3;
					}
				}
				else//����Ӧ�ó���洢��д����ʧ��
				{
					Err_Flag = 2;
				}
				FLASH_OB_Lock(); /* ���� Option Bytes */
				FLASH_Lock(); /* ���� FLASH Control */
			}
			else//��������CRCУ�鶼ʧ��
			{
				Err_Flag+=10;
			}
		}
	}
	while (1)
	{
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		/* �û�Ӧ�õ�ַ */
		Jump_To_Application = (pFunction) JumpAddress;
		/* ��ʼ��MSP */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}
}
