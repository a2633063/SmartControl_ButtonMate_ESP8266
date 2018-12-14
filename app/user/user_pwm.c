#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "gpio.h"
#include "user_pwm.h"
#include "user_setting.h"
//#include "pwm.h"
/*
 * SDK自带PWM，最开始时会有窄波,无法屏蔽,引起舵机抖动.
 * 使用硬件定时器实现模拟pwm
 * */

#define PERIOD 20000


LOCAL uint32 duty = 2000;		//

LOCAL int rudder_middle_delay_count = 2;		//计数


uint32 ICACHE_FLASH_ATTR user_pwm_get_duty(void) {
	return duty;
//	return pwm_get_duty(0);
}

void ICACHE_FLASH_ATTR user_pwm_set_duty(uint32 val) {
	if (val > PERIOD)
		duty = PERIOD;
	else
		duty = val;
//	if (val != pwm_get_duty(0)) {
//		pwm_set_duty(val, 0);
//		pwm_start();
//	}
//
//	os_printf("pwm_set_duty:%d\n", user_pwm_get_duty());
}

//硬件定时器回调函数
void hw_pwm_timer_cb(void) {

	if (duty >= PERIOD - 100) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM), 1);
		hw_timer_arm(PERIOD);
	} else if (duty < 100) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM), 0);
		hw_timer_arm(PERIOD);
	} else {
		if (GPIO_INPUT_GET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM)) == 1) {
			GPIO_OUTPUT_SET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM), 0);
			hw_timer_arm(PERIOD - duty);
		} else {
			GPIO_OUTPUT_SET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM), 1);
			hw_timer_arm(duty);
		}
	}

}

//软件定时器回调函数
void sw_pwm_timer_cb(void) {
	if (rudder_middle_delay_count > 0) {
		rudder_middle_delay_count--;
	} else if (rudder_middle_delay_count == 0) {
		user_pwm_set_duty(pwm_middle);
		user_set_led(1);
		rudder_middle_delay_count--;
	} else if (rudder_middle_delay_count > -20) {		//20*20ms=400ms后关闭pwm
		rudder_middle_delay_count--;
	} else if (rudder_middle_delay_count == -20) {
		rudder_middle_delay_count--;
		user_pwm_set_duty(0);
	}
}
void ICACHE_FLASH_ATTR user_pwm_init(void) {
	duty = pwm_middle;		//默认位置
	PIN_FUNC_SELECT(PWM_0_OUT_IO_MUX, PWM_0_OUT_IO_FUNC);

	hw_timer_init(1, 1);		//NMI_SOURCE
	hw_timer_set_func(hw_pwm_timer_cb);
	hw_timer_arm(duty);		//周期20ms
	GPIO_OUTPUT_SET(GPIO_ID_PIN(PWM_0_OUT_IO_NUM), 1);

	user_os_timer_init();

}

void ICACHE_FLASH_ATTR user_rudder_set_duty(uint32 val) {
	if (val > PERIOD || val > pwm_max)
		duty = pwm_max;
	else if (val < pwm_min)
		duty = pwm_min;
	else
		duty = val;
}

void user_set_middle_delay(int val) {
	rudder_middle_delay = val;
	if (val < 0)
		rudder_middle_delay = 0;
	rudder_middle_delay_count = val / 20 + 1;
}

void ICACHE_FLASH_ATTR user_rudder_press(unsigned char direction) {

	user_set_led(0);
	if (direction == 0)
		user_pwm_set_duty(pwm_min);
	else
		user_pwm_set_duty(pwm_max);
	user_set_middle_delay(rudder_middle_delay);
}
void ICACHE_FLASH_ATTR user_rudder_test(uint32 val) {

	user_set_led(0);
	if (val > PWM_MAX_CYCLE)
		val = PWM_MAX_CYCLE;
	else if (val < PWM_MIN_CYCLE)
		val = PWM_MIN_CYCLE;

	user_pwm_set_duty(val);
	user_set_middle_delay(1000);
}

