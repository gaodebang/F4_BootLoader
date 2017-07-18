/*
*********************************************************************************************************
*	�ں��쳣�жϷ������
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: NMI_Handler
*	����˵��: ���������жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void NMI_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: HardFault_Handler
*	����˵��: Ӳ�������жϷ�����������쳣�������رգ����ַ������쳣���򴥷���
*			  ִ���쳣����ʱ���������쳣���򴥷�����λʱĬ��ʹ�ܡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void hard_fault(unsigned int * hardfault_args)
{
#if 1
        static unsigned int stacked_r0;
        static unsigned int stacked_r1;
        static unsigned int stacked_r2;
        static unsigned int stacked_r3;
        static unsigned int stacked_r12;
        static unsigned int stacked_lr;
        static unsigned int stacked_pc;
        static unsigned int stacked_psr;
        static unsigned int SHCSR;
        static unsigned char MFSR;
        static unsigned char BFSR;       
        static unsigned short int UFSR;
        static unsigned int HFSR;
        static unsigned int DFSR;
        static unsigned int MMAR;
        static unsigned int BFAR;
        stacked_r0 = ((unsigned long) hardfault_args[0]);
        stacked_r1 = ((unsigned long) hardfault_args[1]);
        stacked_r2 = ((unsigned long) hardfault_args[2]);
        stacked_r3 = ((unsigned long) hardfault_args[3]);
        stacked_r12 = ((unsigned long) hardfault_args[4]);
        /*�쳣�жϷ���ʱ������쳣ģʽ�ض�������R14,��lr�����óɸ��쳣ģʽ��Ҫ���صĵ�ַ*/
        stacked_lr = ((unsigned long) hardfault_args[5]);   		// ��Ϊ������ʱǰ��ִ�����
        stacked_pc = ((unsigned long) hardfault_args[6]);
        stacked_psr = ((unsigned long) hardfault_args[7]);
        SHCSR = (*((volatile unsigned long *)(0xE000ED24))); 		//ϵͳHandler���Ƽ�״̬�Ĵ���
        MFSR = (*((volatile unsigned char *)(0xE000ED28)));			//�洢������fault״̬�Ĵ���       
        BFSR = (*((volatile unsigned char *)(0xE000ED29)));			//����fault״̬�Ĵ���       
        UFSR = (*((volatile unsigned short int *)(0xE000ED2A)));	//�÷�fault״̬�Ĵ���               
        HFSR = (*((volatile unsigned long *)(0xE000ED2C))); 		//Ӳfault״̬�Ĵ���                       
        DFSR = (*((volatile unsigned long *)(0xE000ED30)));			//����fault״̬�Ĵ���
        MMAR = (*((volatile unsigned long *)(0xE000ED34)));			//�洢������ַ�Ĵ���
        BFAR = (*((volatile unsigned long *)(0xE000ED38)));			//����fault��ַ�Ĵ���
#endif

//		ִ�г�����������ں˴������������е�����while(1)����
//		��ʱ�۲���Ӧ�Ķ�ջ�͹��ϼĴ���ֵ�� stacked_lr��Ϊ���Ϸ���ʱ��������ж�ǰpc��ֵ��
//		��MDK��������״̬�£�����stacked_lr��ֵΪ0x1A002D08��
//		�����·�����������롰pc = 0x1A002D08�����س���
//		���ɶ�λ��������Ĵ���λ�á�

  /* ��Ӳ��ʧЧ�쳣����ʱ������ѭ�� */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: MemManage_Handler
*	����˵��: �ڴ�����쳣�жϷ������Υ��MPU�趨�Ĵ洢�����ʹ���ʱ������ ��λʱĬ��δʹ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MemManage_Handler(void)
{
  /* ���ڴ�����쳣����ʱ������ѭ�� */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: BusFault_Handler
*	����˵��: ���߷����쳣�жϷ������ȡָ����ݶ�д����ջ���������쳣�� ��λʱĬ��δʹ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BusFault_Handler(void)
{
  /* �������쳣ʱ������ѭ�� */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: UsageFault_Handler
*	����˵��: �÷������жϷ������ִ��δ����ָ��Ƕ������������ʱ������ ��λʱĬ��δʹ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UsageFault_Handler(void)
{
  /* ���÷��쳣ʱ������ѭ�� */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: SVC_Handler
*	����˵��: ͨ��SWIָ���ϵͳ��������жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SVC_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: DebugMon_Handler
*	����˵��: ���Լ������жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DebugMon_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: PendSV_Handler
*	����˵��: �ɹ����ϵͳ��������жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PendSV_Handler(void)
{
}