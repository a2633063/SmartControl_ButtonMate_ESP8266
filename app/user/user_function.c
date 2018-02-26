#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_function.h"
#include "user_setting.h"
#include "user_pwm.h"

/*
 * 掉电读写设置参数
 * 修改设置后保存设置,上电后读取设置参数
 */

void ICACHE_FLASH_ATTR
user_setting_init(void) {

//	os_printf("PWM MAX ADDR:%d\r\n", SETTING_SAVE_PWM_MAX_ADDR);
//	os_printf("PWM MIN ADDR:%d\r\n", SETTING_SAVE_PWM_MIN_ADDR);
//	os_printf("PWM MIDDLE ADDR:%d\r\n", SETTING_SAVE_PWM_MIDDLE_ADDR);

//		int i;
//		unsigned char temp[4] = { 0,0,0,0 };
//		for (i = 0; i < 4096; i+=4) {
//			if (i % 16 == 0)
//				os_printf("\r\n%02x:", i / 16);
//			spi_flash_read(i+0x1000 , (uint32 *) &temp, 4);
//			os_printf("%02x %02x %02x %02x   ", temp[0], temp[1], temp[2], temp[3]);
//		}


	user_setting_get_pwm_max();
	user_setting_get_pwm_min();
	user_setting_get_pwm_middle();
	user_setting_get_pwm_middle_delay();

	os_printf("PWM MAX:%d\r\n", pwm_max);
	os_printf("PWM MIN:%d\r\n", pwm_min);
	os_printf("PWM MIDDLE:%d\r\n", pwm_middle);
	os_printf("PWM DELAY:%d\r\n", rudder_middle_delay);


}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_max(uint32 val) {
	pwm_max=val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MAX_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MAX_ADDR * 4096, &pwm_max, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_min(uint32 val) {
	pwm_min=val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MIN_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MIN_ADDR * 4096, &pwm_min, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_middle(uint32 val) {
	pwm_middle=val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MIDDLE_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MIDDLE_ADDR * 4096, &pwm_middle, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_middle_delay(uint32 val) {
	rudder_middle_delay=val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MIDDLE_DELAY_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MIDDLE_DELAY_ADDR * 4096, &rudder_middle_delay, 4);
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_pwm_max(void) {
	spi_flash_read(SETTING_SAVE_PWM_MAX_ADDR * 4096, &pwm_max, 4);
	if (pwm_max > PWM_MAX_CYCLE || pwm_max < PWM_MIN_CYCLE || pwm_max < pwm_min) {
		pwm_max = PWM_MAX_CYCLE;
	}
	return pwm_max;
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_pwm_min(void) {
	spi_flash_read(SETTING_SAVE_PWM_MIN_ADDR * 4096, &pwm_min, 4);

	if (pwm_min > PWM_MAX_CYCLE || pwm_min < PWM_MIN_CYCLE || pwm_min > pwm_max) {
		pwm_min = PWM_MIN_CYCLE;
	}
	return pwm_min;
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_pwm_middle(void) {
	spi_flash_read(SETTING_SAVE_PWM_MIDDLE_ADDR * 4096, &pwm_middle, 4);

	if (pwm_middle > PWM_MAX_CYCLE || pwm_middle < PWM_MIN_CYCLE) {
		pwm_middle = (PWM_MAX_CYCLE + PWM_MIN_CYCLE) / 2;
	}
	return pwm_middle;
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_pwm_middle_delay(void) {
	spi_flash_read(SETTING_SAVE_PWM_MIDDLE_DELAY_ADDR * 4096, &rudder_middle_delay, 4);
	if(rudder_middle_delay>999) rudder_middle_delay=500;

	return rudder_middle_delay;
}
