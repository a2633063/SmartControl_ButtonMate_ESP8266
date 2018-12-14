#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "espconn.h"

#include "user_wifi.h"
#include "user_function.h"
#include "user_setting.h"
#include "user_pwm.h"

/*
 * 通用函数
 * tcp udp接收数据后的处理函数
 */

const char *device_find_request = "Device Report!!";

const char *device_find_response_ok = "I'm button:";
const char *setting_pwm_max = "rudder_max=";
const char *setting_pwm_min = "rudder_min=";
const char *setting_pwm_middle = "rudder_middle=";
const char *setting_pwm_middle_delay = "rudder_delay=";
const char *setting_pwm_test = "rudder_pwm_test=";
const char *setting_idx = "idx=";
const char *setting_get_all = "get all setting";
const char *setting_update = "update";

void ICACHE_FLASH_ATTR
user_con_received(void *arg, char *pusrdata, unsigned short length) {
	if (length == 1 && *pusrdata == 127)
		return;

	struct espconn *pesp_conn = arg;

	int i, j, k;
	char DeviceBuffer[40] = { 0 };

	os_sprintf(DeviceBuffer, "result:%s\n", pusrdata);

	if (pesp_conn->type == ESPCONN_TCP) { //tcp
		//espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));
	} else if (pesp_conn->type == ESPCONN_UDP) {		//udp
		remot_info *premot = NULL;
		if (espconn_get_connection_info(pesp_conn, &premot, 0) != ESPCONN_OK)
			return;
		pesp_conn->proto.tcp->remote_port = premot->remote_port;	//获取端口
		pesp_conn->proto.tcp->remote_ip[0] = premot->remote_ip[0];	//获取IP地址
		pesp_conn->proto.tcp->remote_ip[1] = premot->remote_ip[1];
		pesp_conn->proto.tcp->remote_ip[2] = premot->remote_ip[2];
		pesp_conn->proto.tcp->remote_ip[3] = premot->remote_ip[3];
		//espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));	//发送数据
	}

	if (length == os_strlen(device_find_request) &&
	os_strncmp(pusrdata, device_find_request, os_strlen(device_find_request)) == 0) {

		char Device_mac_buffer[60] = { 0 };

		struct ip_info ipconfig;

		wifi_get_ip_info(STATION_IF, &ipconfig);
		wifi_get_macaddr(STATION_IF, hwaddr);

		os_sprintf(DeviceBuffer, "%s" MACSTR "," IPSTR, device_find_response_ok, MAC2STR(hwaddr), IP2STR(&ipconfig.ip));
		os_printf("%s\n", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));
	} else if (length == 1 && *pusrdata == '+') {
		user_rudder_press(1);
	} else if (length == 1 && *pusrdata == '-') {
		user_rudder_press(0);
	} else if (length == os_strlen(setting_update) && os_strncmp(pusrdata, setting_update, os_strlen(setting_update)) == 0) {
		os_printf("\nupdate\n");
		os_printf("user bin:%d\n", system_upgrade_userbin_check());
		user_update();
	} else if (length == os_strlen(setting_get_all) && os_strncmp(pusrdata, setting_get_all, os_strlen(setting_get_all)) == 0) {

		i = (pwm_max - PWM_MIN_CYCLE + 1) / 10.5;
		j = (pwm_min - PWM_MIN_CYCLE + 1) / 10.5;
		k = (pwm_middle - PWM_MIN_CYCLE + 1) / 10.5;
		int l = rudder_middle_delay;
		os_sprintf(DeviceBuffer, "%s%03d\n%s%03d\n%s%03d\n%s%03d\n", setting_pwm_max, i, setting_pwm_min, j, setting_pwm_middle, k, setting_pwm_middle_delay, l);

		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));
	} else if (length == (os_strlen(setting_pwm_max) + 3) && os_strncmp(pusrdata, setting_pwm_max, os_strlen(setting_pwm_max)) == 0) {
		k = 0;
		for (i = 0; i < 3; i++) {
			j = *(pusrdata + os_strlen(setting_pwm_max) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0) {
			if (k >= 0 && k <= 180) {
				user_set_led(0);
				user_setting_set_pwm_max(k * 10 + k / 2 + PWM_MIN_CYCLE);
				user_set_led(1);
			}
//			os_printf("pwm_max:%d\n", pwm_max);
		}
		k = (pwm_max - PWM_MIN_CYCLE + 1) / 10.5;
		os_sprintf(DeviceBuffer, "%s" "%03d\n", setting_pwm_max, k);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	} else if (length == (os_strlen(setting_pwm_min) + 3) && os_strncmp(pusrdata, setting_pwm_min, os_strlen(setting_pwm_min)) == 0) {
		k = 0;
		for (i = 0; i < 3; i++) {
			j = *(pusrdata + os_strlen(setting_pwm_min) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0) {
			if (k >= 0 && k <= 180) {
				user_set_led(0);
				user_setting_set_pwm_min(k * 10 + k / 2 + PWM_MIN_CYCLE);
				user_set_led(1);
			}
//			os_printf("pwm_min:%d\n", pwm_min);
		}
		k = (pwm_min - PWM_MIN_CYCLE + 1) / 10.5;
		os_sprintf(DeviceBuffer, "%s" "%03d\n", setting_pwm_min, k);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	} else if (length == (os_strlen(setting_pwm_middle) + 3) && os_strncmp(pusrdata, setting_pwm_middle,
	os_strlen(setting_pwm_middle)) == 0) {
		k = 0;
		for (i = 0; i < 3; i++) {
			j = *(pusrdata + os_strlen(setting_pwm_middle) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0) {
			if (k >= 0 && k <= 180) {
				user_set_led(0);
				user_setting_set_pwm_middle(k * 10 + k / 2 + PWM_MIN_CYCLE);
				user_set_led(1);
			}
//			os_printf("pwm_min:%d\n", pwm_middle);
		}
		k = (pwm_middle - PWM_MIN_CYCLE + 1) / 10.5;
		os_sprintf(DeviceBuffer, "%s" "%03d\n", setting_pwm_middle, k);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	} else if (length == (os_strlen(setting_pwm_test) + 3) && os_strncmp(pusrdata, setting_pwm_test,
	os_strlen(setting_pwm_test)) == 0) {
		k = 0;
		for (i = 0; i < 3; i++) {
			j = *(pusrdata + os_strlen(setting_pwm_test) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0 && k <= 180) {
			user_rudder_test(k * 10 + k / 2 + PWM_MIN_CYCLE);
			os_printf("pwm_test:%d\n", k * 10 + k / 2 + PWM_MIN_CYCLE);
		}
	} else if (length == (os_strlen(setting_pwm_middle_delay) + 3) && os_strncmp(pusrdata, setting_pwm_middle_delay, os_strlen(setting_pwm_middle_delay)) == 0) {
		k = 0;
		for (i = 0; i < 3; i++) {
			j = *(pusrdata + os_strlen(setting_pwm_middle_delay) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0) {
			user_set_led(0);
			user_setting_set_pwm_middle_delay(k);
			user_set_led(1);
//			os_printf("pwm_delay:%d\n", rudder_middle_delay);
		}
		os_sprintf(DeviceBuffer, "%s" "%03d\n", setting_pwm_middle_delay, rudder_middle_delay);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	} else if (length > os_strlen(setting_idx) && os_strncmp(pusrdata, setting_idx, os_strlen(setting_idx)) == 0) {
		k = 0;
		for (i = 0; i < length - os_strlen(setting_idx); i++) {
			j = *(pusrdata + os_strlen(setting_idx) + i) - 0x30;
			if (j >= 0 && j <= 9)
				k = k * 10 + j;
			else {
				k = -1;
				break;
			}
		}
		if (k >= 0) {
			user_set_led(0);
			user_setting_set_idx(k);
			user_set_led(1);
		}

		os_sprintf(DeviceBuffer, "%s" "%d\n", setting_idx, k);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	}

}
