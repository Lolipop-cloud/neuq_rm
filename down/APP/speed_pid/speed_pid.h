#ifndef _SPEED_PID_H
#define _SPEED_PID_H




void VPID_Init_All(void);			//电机转速PID参数初始化
void vpid_PI_realize(float kp,float ki);				//电机转速PID实现
void set_chassis_motor_speed(int moto1_speed,int moto2_speed,int moto3_speed,int moto4_speed);			//设置目标速度
void set_trigger_motor_speed(int moto5_speed);	
int abs(int input);				//求绝对值函数void apid_GIMBAL_realize(float kpa,float kia,float kpv,float kiv);
void apid_GIMBAL_PI_realize(float kpa,float kia,float kpv,float kiv);
void set_GIMBAL_angle(int gimbal1_angle,int gimbal2_angle);

#endif
