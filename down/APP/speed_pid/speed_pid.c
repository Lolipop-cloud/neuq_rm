#include "speed_pid.h"
#include "motor.h"
#include "power_limitation.h"

#define IntegralUpperLimit    5000
#define IntegralSeparation    300
#define vPID_OUT_MAX          8000		//��������
#define gimbal_angel_upperlimit  2000  //��̨λ�û�����
#define gimbal_angel_downlimit   200  //��̨λ�û�������

int find_max(void);

// ����: VPID_Init()
// ����: ���ת��pid������ʼ��
// �������ٶȲ����ṹ��
// �������
// �ڲ��������û��������
void VPID_Init(VPID_t *vpid)
{
	vpid->target_speed=0;
	vpid->actual_speed=0;
	vpid->err=0;
	vpid->last_err=0;
	vpid->err_integration=0;
	vpid->P_OUT=0;
	vpid->I_OUT=0;
	vpid->PID_OUT=0;
}

// ����: VPID_Init_All()
// ����: ��..������ٶȳ�ʼ��
// ��������
// �������
void VPID_Init_All()	
{
	VPID_Init(&motor1.vpid);
	VPID_Init(&motor2.vpid);
	VPID_Init(&motor3.vpid);
	VPID_Init(&motor4.vpid);
	VPID_Init(&motor5.vpid);
}


// ����: vpid_realize()
// ����: �ٶ�pid����
// �������ٶ�pid�����ṹ�壬�ٶ�pid��p��i��΢�����Ҫ
// �������
// �ڲ��������û��������
void vpid_realize(VPID_t *vpid,float kp,float ki)
{
	vpid->err = vpid->target_speed - vpid->actual_speed;
	
	if(abs(vpid->err) <= IntegralSeparation)		//���ַ���
		vpid->err_integration += vpid->err;
	if(vpid->err_integration > IntegralUpperLimit)		//�����ֱ���
		vpid->err_integration = IntegralUpperLimit;
	else if(vpid->err_integration < -IntegralUpperLimit)
		vpid->err_integration = -IntegralUpperLimit;
	
	vpid->P_OUT = kp * vpid->err;								//P��
	vpid->I_OUT = ki * vpid->err_integration;		//I��
	
	//����޷�
	if((vpid->P_OUT + vpid->I_OUT) > vPID_OUT_MAX) 
		vpid->PID_OUT = vPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT) < -vPID_OUT_MAX) 
		vpid->PID_OUT = -vPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT;
}


// ����: vpid_PI_realize()
// ����: ���ת��pidʵ��
// ���������ת��pid��p��i��΢�����Ҫ
// �������
void vpid_PI_realize(float kp,float ki)
{
	//��ȡ�����ǰת��
	motor1.vpid.actual_speed = motor1.actual_speed;
	motor2.vpid.actual_speed = motor2.actual_speed;
	motor3.vpid.actual_speed = motor3.actual_speed;
	motor4.vpid.actual_speed = motor4.actual_speed;
	

	//�������ֵ
	vpid_realize(&motor1.vpid,kp,ki);
	vpid_realize(&motor2.vpid,kp,ki);
	vpid_realize(&motor3.vpid,kp,ki);
	vpid_realize(&motor4.vpid,kp,ki);
	
	/******************���ʿ��Ʒ���*************************/

	power_limitation_jugement();
	power_limitation_coefficient();
	
}

// ����: set_motor_speed()
// ����: ����Ŀ���ٶ�
// ������4�������Ŀ���ٶ�
// �������
void set_chassis_motor_speed(int motor1_speed,int motor2_speed,int motor3_speed,int motor4_speed)
{
	motor1.vpid.target_speed = motor1_speed;		//��Ϊ��������෴  ���ԼӸ���
	motor2.vpid.target_speed = motor2_speed;
	motor3.vpid.target_speed = motor3_speed;
	motor4.vpid.target_speed = motor4_speed;
	
	
	
	motor1.target_speed = motor1_speed;		//��Ϊ��������෴  ���ԼӸ���
	motor2.target_speed = motor2_speed;
	motor3.target_speed = motor3_speed;
	motor4.target_speed = motor4_speed;
	

	
}
void set_trigger_motor_speed(int motor5_speed)
{
	//motor5.vpid.target_speed = motor5_speed;
	
	motor5.target_speed = motor5_speed;	
	

}
/*********************************************��̨pid����*******************************************************/
/*void apid_GIMBAL_realize(VPID_t *vpid,float kpa,float kia,float kpv,float,kiv)
{
	vpid->err = vpid->target_angel - vpid->actual_angel;
	
	if(abs(vpid->err) <= gimbal_angel_downlimit)		//���ַ���
		vpid->err_integration += vpid->err;
	if(vpid->err_integration > gimbal_angel_upperlimit)		//�����ֱ���
		vpid->err_integration = IntegralUpperLimit;
	else if(vpid->err_integration < -gimbal_angel_upperlimit)
		vpid->err_integration = -gimbal_angel_upperlimit;
	
	vpid->P_OUT = kpa * vpid->err;								//P��
	vpid->I_OUT = kia * vpid->err_integration;		//I��
	
	//����޷�
	if((vpid->P_OUT + vpid->I_OUT) > vPID_OUT_MAX) 
		vpid->PID_OUT = vPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT) < -vPID_OUT_MAX) 
		vpid->PID_OUT = -vPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT;
	
		vpid->err = vpid->P_OUT + vpid->I_OUT;
	if(abs(vpid->err) <= gimbal_angel_downlimit)		//���ַ���
		vpid->err_integration += vpid->err;
	if(vpid->err_integration > gimbal_angel_upperlimit)		//�����ֱ���
		vpid->err_integration = IntegralUpperLimit;
	else if(vpid->err_integration < -gimbal_angel_upperlimit)
		vpid->err_integration = -gimbal_angel_upperlimit;
	
	vpid->P_OUT = kpv * vpid->err;								//P��
	vpid->I_OUT = kiv * vpid->err_integration;		//I��
	
	//����޷�
	if((vpid->P_OUT + vpid->I_OUT) > vPID_OUT_MAX) 
		vpid->PID_OUT = vPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT) < -vPID_OUT_MAX) 
		vpid->PID_OUT = -vPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT;
}

void apid_GIMBAL_PI_realize(float kpa,float kia,float kpv,float kiv)
{
	//��ȡ�����ǰת��
	gimbal1.vpid.actual_speed = gimbal1.actual_speed;
	gimbal2.vpid.actual_speed = gimbal2.actual_speed;
	//�������ֵ
	apid_GIMBAL_realize(&gimbal1.vpid,kpa,kia,kpv,kiv);
	apid_GIMBAL_realize(&gimbal2.vpid,kpa,kia,kpv,kiv);
}
*/
/*************************************************��̨pid����******************************************************/

// ����: abs()
// ����: �Զ���������ֵ��������Ϊmath.h��Ĳ�����
// ������input
// �����|input|
int abs(int input)
{
	if(input<0)
		input = -input;
	return input;
}

