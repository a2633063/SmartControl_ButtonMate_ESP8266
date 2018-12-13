#ifndef __USER_PWM_H__
#define __USER_PWM_H__

#include "pwm.h"

#define PWM_MAX_CYCLE	2500	//����ź����ֵ
#define PWM_MIN_CYCLE	600	//����ź���Сֵ

extern uint32 pwm_middle;	//����м�λ��
extern uint32 pwm_max;		//������Ƕ�
extern uint32 pwm_min;		//�����С�Ƕ�
extern uint32 rudder_middle_delay;		//�����С�Ƕ�

/*Definition of GPIO PIN params, for GPIO initialization*/
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTCK_U
#define PWM_0_OUT_IO_NUM 13
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO13


uint32 user_pwm_get_duty(void);
void user_pwm_set_duty(uint32 duty);
void user_pwm_init(void);
void user_rudder_press(unsigned char direction);
void user_rudder_test(uint32 direction);
void sw_pwm_timer_cb(void);

#endif

