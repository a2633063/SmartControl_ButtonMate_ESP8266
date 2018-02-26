#ifndef __USER_SETTING_H__
#define __USER_SETTING_H__

#define	SETTING_SAVE_PWM_MAX_ADDR  0xf0
#define	SETTING_SAVE_PWM_MIN_ADDR	0xf4
#define	SETTING_SAVE_PWM_MIDDLE_ADDR	0xf8
#define	SETTING_SAVE_PWM_MIDDLE_DELAY_ADDR	0xfc

void ser_setting_init(void);

void user_setting_set_pwm_max(uint32 val);
void user_setting_set_pwm_min(uint32 val);
void user_setting_set_pwm_middle(uint32 val);
void user_setting_set_pwm_middle_delay(uint32 val);

uint32 user_setting_get_pwm_max(void);
uint32 user_setting_get_pwm_min(void);
uint32 user_setting_get_pwm_middle(void);
uint32 user_setting_get_pwm_middle_delay(void);

#endif
