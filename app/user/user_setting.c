#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_setting.h"
#include "user_pwm.h"
#include "user_wifi.h"

uint32 pwm_middle = 1650;	//舵机中间位置
uint32 pwm_max = 2500;		//舵机最大角度
uint32 pwm_min = 800;		//舵机最小角度
uint32 rudder_middle_delay = 500;		//舵机回到中间位置倒计时时间

int32 idx = -1;	//domoticz mqtt数据 idx值


uint16 mqtt_ip[4];	//mqtt service ip
uint16 mqtt_port = 0;		//mqtt service port
uint8 mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user
uint8 mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user
uint8 mqtt_device_id[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user
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

	user_setting_get_idx();
	os_printf("Domoticz idx:%d\r\n", idx);

	user_setting_get_mqtt_ip();
	user_setting_get_mqtt_port();
	user_setting_get_mqtt_user();
	user_setting_get_mqtt_password();
	user_setting_get_mqtt_device_id();
	os_printf("MQTT Service:" IPSTR "\r\n", IP2STR(mqtt_ip));
	os_printf("MQTT Service port:%d\r\n", mqtt_port);
	os_printf("MQTT Service user:%s\r\n", mqtt_user);
	os_printf("MQTT Service password:%s\r\n", mqtt_password);
	os_printf("MQTT Service device id:%s\r\n", mqtt_device_id);

}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_max(uint32 val) {
	pwm_max = val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MAX_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MAX_ADDR * 4096, &pwm_max, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_min(uint32 val) {
	pwm_min = val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MIN_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MIN_ADDR * 4096, &pwm_min, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_middle(uint32 val) {
	pwm_middle = val;
	spi_flash_erase_sector(SETTING_SAVE_PWM_MIDDLE_ADDR);
	spi_flash_write(SETTING_SAVE_PWM_MIDDLE_ADDR * 4096, &pwm_middle, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_pwm_middle_delay(uint32 val) {
	rudder_middle_delay = val;
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
	if (rudder_middle_delay > 999)
		rudder_middle_delay = 500;

	return rudder_middle_delay;
}

void ICACHE_FLASH_ATTR
user_setting_set_idx(uint32 val) {
	idx = val;
	spi_flash_erase_sector(SETTING_SAVE_IDX_ADDR);
	spi_flash_write(SETTING_SAVE_IDX_ADDR * 4096, &idx, 4);
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_idx(void) {
	spi_flash_read(SETTING_SAVE_IDX_ADDR * 4096, &idx, 4);
	return idx;
}

void ICACHE_FLASH_ATTR
user_setting_set_mqtt_ip(uint8 * p) {
	mqtt_ip[0] = p[0];
	mqtt_ip[1] = p[1];
	mqtt_ip[2] = p[2];
	mqtt_ip[3] = p[3];
	spi_flash_erase_sector(SETTING_SAVE_MQTT_IP_ADDR);
	spi_flash_write(SETTING_SAVE_MQTT_IP_ADDR * 4096, (uint32 *) mqtt_ip, 4);
}

void ICACHE_FLASH_ATTR
user_setting_get_mqtt_ip() {
	spi_flash_read(SETTING_SAVE_MQTT_IP_ADDR * 4096, (uint32 *) mqtt_ip, 4);
}

void ICACHE_FLASH_ATTR
user_setting_set_mqtt_port(uint32 val) {
	if (val < 1 || val > 65535)
		return;
	mqtt_port = val;
	spi_flash_erase_sector(SETTING_SAVE_MQTT_PORT_ADDR);
	spi_flash_write(SETTING_SAVE_MQTT_PORT_ADDR * 4096, &val, 4);
}

uint32 ICACHE_FLASH_ATTR
user_setting_get_mqtt_port(void) {
	uint32 val;
	spi_flash_read(SETTING_SAVE_MQTT_PORT_ADDR * 4096, &val, 4);
	if (val < 1 || val > 65535)
		mqtt_port = 1883;
	mqtt_port = val;
	return mqtt_port;
}

void ICACHE_FLASH_ATTR
user_setting_set_mqtt_user(uint8 * p) {
	uint32_t length = os_strlen(p);
	if (length > 31)
		return;

	os_strcpy(mqtt_user, p);
	mqtt_user[length] = 0;
	if (length % 4 != 0)
		length += 4 - length % 4;
	spi_flash_erase_sector(SETTING_SAVE_MQTT_USER_ADDR);
	spi_flash_write(SETTING_SAVE_MQTT_USER_ADDR * 4096, (uint32 *) mqtt_user, length);
}

void ICACHE_FLASH_ATTR
user_setting_get_mqtt_user(void) {
	uint32 val;
	spi_flash_read(SETTING_SAVE_MQTT_USER_ADDR * 4096, (uint32 *) mqtt_user, SETTING_MQTT_STRING_LENGTH_MAX);
	if (mqtt_user[0] > 0x7f)
		user_setting_set_mqtt_user("user");
}

void ICACHE_FLASH_ATTR
user_setting_set_mqtt_password(uint8 * p) {
	uint32_t length = os_strlen(p);
	if (length > 31)
		return;

	os_strcpy(mqtt_password, p);
	mqtt_password[length] = 0;
	if (length % 4 != 0)
		length += 4 - length % 4;
	spi_flash_erase_sector(SETTING_SAVE_MQTT_PASSWORD_ADDR);
	spi_flash_write(SETTING_SAVE_MQTT_PASSWORD_ADDR * 4096, (uint32 *) mqtt_password, length);
}

void ICACHE_FLASH_ATTR
user_setting_get_mqtt_password(void) {
	uint32 val;
	spi_flash_read(SETTING_SAVE_MQTT_PASSWORD_ADDR * 4096, (uint32 *) mqtt_password, SETTING_MQTT_STRING_LENGTH_MAX);
	if (mqtt_password[0] > 0x7f)
		user_setting_set_mqtt_password("123456");
}

void ICACHE_FLASH_ATTR
user_setting_set_mqtt_device_id(uint8 * p) {
	uint32_t length = os_strlen(p);
	if (length > 31)
		return;

	os_strcpy(mqtt_device_id, p);
	mqtt_device_id[length] = 0;
	if (length % 4 != 0)
		length += 4 - length % 4;
	spi_flash_erase_sector(SETTING_SAVE_MQTT_DEVICE_ID_ADDR);
	spi_flash_write(SETTING_SAVE_MQTT_DEVICE_ID_ADDR * 4096, (uint32 *) mqtt_device_id, length);
}

void ICACHE_FLASH_ATTR
user_setting_get_mqtt_device_id(void) {
	uint32 val;
	spi_flash_read(SETTING_SAVE_MQTT_DEVICE_ID_ADDR * 4096, (uint32 *) mqtt_device_id, SETTING_MQTT_STRING_LENGTH_MAX);
	if (mqtt_device_id[0] > 0x7f) {
		if (wifi_get_macaddr(STATION_IF, hwaddr)) {
			uint8 device_id[SETTING_MQTT_STRING_LENGTH_MAX];
//			os_printf("device id:"SETTING_SAVE_MQTT_DEVICE_ID_NAME" \r\n", hwaddr[4],hwaddr[5]);
			os_sprintf(device_id, SETTING_SAVE_MQTT_DEVICE_ID_NAME, hwaddr[4], hwaddr[5]);
			user_setting_set_mqtt_device_id(device_id);
		} else{
			os_printf("wifi_get_macaddr error \r\n");
			user_setting_set_mqtt_device_id("Deviced_Id");
		}
	}
}

