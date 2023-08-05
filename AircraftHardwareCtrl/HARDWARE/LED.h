#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f10x.h"

#define IRQ_1MS 1000
#define IRQ_2MS 2000
#define IRQ_3MS 3000
#define IRQ_4MS 4000

#define DEFAULT_PERIOD IRQ_1MS

#define ITS_PER_MS 1000 / DEFAULT_PERIOD // 每毫秒中断的次数

extern volatile uint32_t TIMIRQCNT;

extern void cycleCounterInit(void);
extern void delay_ms(uint16_t nms);
extern uint32_t GetSysTime_us(void);
extern void delay_us(unsigned int i);

#endif
}
status;
}
sLED;

extern sLED LED;
extern void LEDInit(void);
extern void LEDtest(void);
extern void PilotLED(void);

#define LED_TAKE_OFF_ENTER LED.status = WARNING
#define LED_TAKE_OFF_EXIT LED.status = AllFlashLight
#define LED_HEIGHT_LOCK_ENTER \
	LED.FlashTime = 50;       \
	LED.status = AlternateFlash
#define LED_HEIGHT_LOCK_EXIT \
	LED.FlashTime = 100;     \
	LED.status = AllFlashLight
#define LED_3D_ROLL_ENTER LED.status = WARNING
#define LED_3D_ROLL_EXIT LED.status = AllFlashLight
#define LED_SAFTY_TAKE_DOWN_ENTER LED.status = DANGEROURS
#define LED_SAFTY_TAKE_DOWN_EXIT LED.status = AlwaysOn
#define LED_GET_MPU_OFFSET_ENTER LED.status = GET_OFFSET
#define LED_GO_HOME_ENTER LED.status = WARNING
#define LED_GO_HOME_EXIT LED.status = AllFlashLight

#endif
