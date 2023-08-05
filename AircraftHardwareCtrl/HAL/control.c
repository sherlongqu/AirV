#ifndef _ALL_USER_DATA_H_
#define _ALL_USER_DATA_H_

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define NULL 0
extern volatile uint32_t SysTick_count;

typedef struct
{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
} _st_Mpu;

typedef struct
{
	int16_t magX;
	int16_t magY;
	int16_t magZ;
} _st_Mag;

typedef struct
{
	float rate;
	float height;
} High;

typedef struct
{
	float roll;
	float pitch;
	float yaw;
} _st_AngE;

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
} _st_Remote;

typedef volatile struct
{
	float desired;		  //< set point
	float offset;		  //
	float prevError;	  //< previous error
	float integ;		  //< integral
	float kp;			  //< proportional gain
	float ki;			  //< integral gain
	float kd;			  //< derivative gain
	float IntegLimitHigh; //< integral limit
	float IntegLimitLow;
	float measured;
	float out;
	float OutLimitHigh;
	float OutLimitLow;
} PidObject;

typedef volatile struct
{
	uint8_t unlock;

} _st_ALL_flag;

extern _st_Remote Remote;
extern _st_Mpu MPU6050;
extern _st_Mag AK8975; // 保留，需外接磁力计
extern _st_AngE Angle;

extern _st_ALL_flag ALL_flag;

extern PidObject pidRateX;
extern PidObject pidRateY;
extern PidObject pidRateZ;

extern PidObject pidPitch;
extern PidObject pidRoll;
extern PidObject pidYaw;

extern PidObject pidHeightRate;
extern PidObject pidHeightHigh;

#endif

{
	status = WAITING_2;
}
case WAITING_2: // 解锁完成后判断使用者是否开始拨动遥杆进行飞行控制
if (Remote.thr > 1100)
{
	status = PROCESS_31;
}
break;
case PROCESS_31:
{
	int16_t temp;
	temp = Remote.thr - 1000 + pidHeightRate.out; // 油门+定高输出值
	if (Remote.thr < 1020)						  // 油门太低了，则限制输出  不然飞机乱转
	{
		MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;
		break;
	}
	MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = LIMIT(temp, 0, 900); // 留100给姿态控制

	MOTOR1 += +pidRateX.out - pidRateY.out - pidRateZ.out; //; 姿态输出分配给各个电机的控制量
	MOTOR2 += +pidRateX.out + pidRateY.out + pidRateZ.out; //;
	MOTOR3 += -pidRateX.out + pidRateY.out - pidRateZ.out;
	MOTOR4 += -pidRateX.out - pidRateY.out + pidRateZ.out; //;
}
break;
case EXIT_255:
MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0; // 如果锁定，则电机输出都为0
status = WAITING_1;
break;
default:
break;
}

TIM2->CCR1 = LIMIT(MOTOR1, 0, 1000); // 更新PWM
TIM2->CCR2 = LIMIT(MOTOR2, 0, 1000);
TIM2->CCR3 = LIMIT(MOTOR3, 0, 1000);
TIM2->CCR4 = LIMIT(MOTOR4, 0, 1000);
}

/************************************END OF FILE********************************************/
