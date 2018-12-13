#ifndef __USER_PWM_H__
#define __USER_PWM_H__

#include "pwm.h"

#define PWM_MAX_CYCLE	2500	//舵机信号最大值
#define PWM_MIN_CYCLE	600	//舵机信号最小值

extern uint32 pwm_middle;	//舵机中间位置
extern uint32 pwm_max;		//舵机最大角度
extern uint32 pwm_min;		//舵机最小角度
extern uint32 rudder_middle_delay;		//舵机最小角度

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

