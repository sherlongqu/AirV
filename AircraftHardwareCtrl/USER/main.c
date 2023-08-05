//========================================================================
//	爱好者电子工作室-淘宝 https://devotee.taobao.com/
//	STM32四轴爱好者QQ群: 810149456
//	作者：小刘
//	电话:13728698082
//	邮箱:1042763631@qq.com
//	日期：2018.05.17
//	版本：V1.0
//========================================================================
// 套件购买地址：https://devotee.taobao.com/
//                 爱好者电子工作室
// 特此声明：
//
//         此程序只能用作学习，如用商业用途。必追究责任！
//
//
//
#include "ALL_DEFINE.h"

// 特别声明：请在无人空旷的地带或者室内进行飞行。遇到非常紧急的情况，可紧急关闭遥控。

int main(void)
{
	cycleCounterInit();								// 得到系统每个us的系统CLK个数，为以后延时函数，和得到精准的当前执行时间使用
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2个bit的抢占优先级，2个bit的子优先级
	SysTick_Config(SystemCoreClock / 1000);			// 系统滴答时钟

	ALL_Init(); // 系统初始化

	while (1)
	{
		ANTO_polling(); // 匿名上位机
		PilotLED();		// LED刷新
	}
}