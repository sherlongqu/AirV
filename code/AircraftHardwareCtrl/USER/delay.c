#ifndef _DELAY_H
#define _DELAY_H 


#include "stm32f10x.h"

#define	IRQ_1MS	1000
#define	IRQ_2MS	2000
#define	IRQ_3MS	3000
#define	IRQ_4MS	4000

#define	DEFAULT_PERIOD	IRQ_1MS

#define	ITS_PER_MS		1000/DEFAULT_PERIOD   // 每毫秒中断的次数


extern volatile uint32_t TIMIRQCNT;



extern void cycleCounterInit(void);
extern void delay_ms(uint16_t nms);
extern uint32_t GetSysTime_us(void); 
extern void delay_us(unsigned int i);

#endif


typedef struct{
	int16_t magX;
	int16_t magY;
	int16_t magZ;
}_st_Mag;


typedef struct{
	float rate;
	float height;
}High;


typedef struct{
	float roll;
	float pitch;
	float yaw;
}_st_AngE;



typedef struct
{
	uint16_t roll;
	uint16_t pitch;
	uint16_t thr;
	uint16_t yaw;
	uint16_t AUX1;
	uint16_t AUX2;
	uint16_t AUX3;
	uint16_t AUX4;	
}_st_Remote;



typedef volatile struct
{
	float desired;     //< set point
	float offset;      //
	float prevError;    //< previous error
	float integ;        //< integral
	float kp;           //< proportional gain
	float ki;           //< integral gain
	float kd;           //< derivative gain
	float IntegLimitHigh;       //< integral limit
	float IntegLimitLow;
	float measured;
	float out;
	float OutLimitHigh;
	float OutLimitLow;
}PidObject;


typedef volatile struct
{
	uint8_t unlock;
	

}_st_ALL_flag;


extern _st_Remote Remote;
extern _st_Mpu MPU6050;
extern _st_Mag AK8975; //保留，需外接磁力计
extern _st_AngE Angle;


extern _st_ALL_flag ALL_flag;


extern	PidObject pidRateX;
extern	PidObject pidRateY;
extern	PidObject pidRateZ;

extern	PidObject pidPitch;
extern	PidObject pidRoll;
extern	PidObject pidYaw;

extern	PidObject pidHeightRate;
extern	PidObject pidHeightHigh;

#endif

