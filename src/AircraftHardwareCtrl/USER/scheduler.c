//========================================================================
//	�����ߵ��ӹ�����-�Ա� https://devotee.taobao.com/
//	STM32���ᰮ����QQȺ: 810149456
//	���ߣ�С��
//	�绰:13728698082
//	����:1042763631@qq.com
//	���ڣ�2018.05.17
//	�汾��V1.0
//========================================================================
//�׼������ַ��https://devotee.taobao.com/
//                 �����ߵ��ӹ�����
//�ش�������
//
//         �˳���ֻ������ѧϰ��������ҵ��;����׷�����Σ�
//          
//
//
#include "ALL_DEFINE.h"
#include "scheduler.h"


void TIM3_IRQHandler(void)   //TIM3�ж� 3ms
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		static uint8_t cnt_3ms = 0;
		static uint8_t cnt_6ms = 0;	 
		SysTick_count++;
		cnt_3ms++;
		cnt_6ms++;
		
		if(cnt_3ms == 1)  //3ms����һ��
		{
			cnt_3ms = 0;
			MpuGetData();
			RC_Analy();			
			FlightPidControl(0.003f);
			MotorControl();
		}		
		if(cnt_6ms == 2) //6ms����һ��
		{
			cnt_6ms = 0;
			GetAngle(&MPU6050,&Angle,0.00626f);
		}		

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}


