#ifndef __USER_PWM_H__
#define __USER_PWM_H__

#include "pwm.h"

#define PWM_MAX_CYCLE	2500	//舵机信号最大值
#define PWM_MIN_CYCLE	600	//舵机信号最小值



/*Definition of GPIO PIN params, for GPIO initialization*/
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_GPIO4_U
#define PWM_0_OUT_IO_NUM 4
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO4


uint32 user_pwm_get_duty(void);
void user_pwm_set_duty(uint32 duty);
void user_pwm_init(void);
void user_rudder_press(unsigned char direction);
void user_rudder_test(uint32 direction);
void sw_pwm_timer_cb(void);
uint8 user_rudder_get_direction(void);

#endif

