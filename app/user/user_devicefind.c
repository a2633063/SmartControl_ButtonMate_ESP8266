/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_devicefind.c
 *
 * Description: Find your hardware's information while working any mode.
 *
 * Modification history:
 *     2014/3/12, v1.0 create this file.
 *******************************************************************************/
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "espconn.h"
#include "user_devicefind.h"
#include "user_pwm.h"

const char *device_find_request = "Device Report!!";

const char *device_find_response_ok = "I'm button:";
const char *setting_pwm_max = "rudder_max=";
const char *setting_pwm_min = "rudder_min=";
const char *setting_pwm_middle = "rudder_middle=";
const char *setting_pwm_middle_delay = "rudder_delay=";
const char *setting_pwm_test = "rudder_pwm_test=";
const char *setting_get_all = "get all setting";
const char *setting_update = "update";
/*---------------------------------------------------------------------------*/
LOCAL struct espconn ptrespconn;

/******************************************************************************
 * FunctionName : user_devicefind_recv
 * Description  : Processing the received data from the host
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
 *******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_devicefind_recv(void *arg, char *pusrdata, unsigned short length) {
	char DeviceBuffer[40] = { 0 };
	char Device_mac_buffer[60] = { 0 };
	char hwaddr[6];
	int i, j, k;
	remot_info *premot = NULL;
	struct ip_info ipconfig;
	unsigned char temp[4] = { 0 };
	if (wifi_get_opmode() != STATION_MODE) {
		wifi_get_ip_info(SOFTAP_IF, &ipconfig);
		wifi_get_macaddr(SOFTAP_IF, hwaddr);

		if (!ip_addr_netcmp((struct ip_addr * )ptrespconn.proto.udp->remote_ip, &ipconfig.ip, &ipconfig.netmask)) {
			wifi_get_ip_info(STATION_IF, &ipconfig);
			wifi_get_macaddr(STATION_IF, hwaddr);
		}
	} else {
		wifi_get_ip_info(STATION_IF, &ipconfig);
		wifi_get_macaddr(STATION_IF, hwaddr);
	}

	if (pusrdata == NULL) {
		return;
	}

	if (length == os_strlen(device_find_request) &&
	os_strncmp(pusrdata, device_find_request,
	os_strlen(device_find_request)) == 0) {
		os_sprintf(DeviceBuffer, "%s" MACSTR "," IPSTR, device_find_response_ok, MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

		os_printf("%s\n", DeviceBuffer);
		length = os_strlen(DeviceBuffer);
		if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK)
			return;
		os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
		ptrespconn.proto.udp->remote_port = premot->remote_port;
		espconn_sent(&ptrespconn, DeviceBuffer, length);
	} else if (length == (os_strlen(device_find_request) + 18)) {
		os_sprintf(Device_mac_buffer, "%s " MACSTR, device_find_request, MAC2STR(hwaddr));
		os_printf("%s", Device_mac_buffer);

		if (os_strncmp(Device_mac_buffer, pusrdata,
		os_strlen(device_find_request) + 18) == 0) {
			//os_printf("%s\n", Device_mac_buffer);
			length = os_strlen(DeviceBuffer);
			os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, device_find_response_ok, MAC2STR(hwaddr),
					IP2STR(&ipconfig.ip));

			os_printf("%s\n", DeviceBuffer);
			length = os_strlen(DeviceBuffer);
			if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK)
				return;
			os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
			ptrespconn.proto.udp->remote_port = premot->remote_port;
			espconn_sent(&ptrespconn, DeviceBuffer, length);
		} else {
			return;
		}
	} else if (length == 1 && *pusrdata == '+') {
		user_rudder_press(1);
	} else if (length == 1 && *pusrdata == '-') {
		user_rudder_press(0);
	} else if (length == (os_strlen(setting_pwm_max) + 3)
			&& os_strncmp(pusrdata, setting_pwm_max, os_strlen(setting_pwm_max)) == 0) {
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
			if (k >= 0 && k <= 180)
				user_setting_set_pwm_max(k * 10 + k / 2 + PWM_MIN_CYCLE);
			os_printf("pwm_max:%d\n", pwm_max);
		}
	} else if (length == (os_strlen(setting_pwm_min) + 3)
			&& os_strncmp(pusrdata, setting_pwm_min, os_strlen(setting_pwm_min)) == 0) {
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
			if (k >= 0 && k <= 180)
				user_setting_set_pwm_min(k * 10 + k / 2 + PWM_MIN_CYCLE);
			os_printf("pwm_min:%d\n", pwm_min);
		}
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
		os_printf("debug k=%d\n", k);
		if (k >= 0) {
			if (k >= 0 && k <= 180)
				user_setting_set_pwm_middle(k * 10 + k / 2 + PWM_MIN_CYCLE);
			os_printf("pwm_min:%d\n", pwm_middle);
		}
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
		if (k >= 0 && k <= 180)
			user_rudder_test(k * 10 + k / 2 + PWM_MIN_CYCLE);
		os_printf("pwm_test:%d\n", k * 10 + k / 2 + PWM_MIN_CYCLE);
	} else if (length == (os_strlen(setting_pwm_middle_delay) + 3)
			&& os_strncmp(pusrdata, setting_pwm_middle_delay, os_strlen(setting_pwm_middle_delay)) == 0) {
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
			user_setting_set_pwm_middle_delay(k);
			os_printf("pwm_delay:%d\n", rudder_middle_delay);
		}
	}
}

/******************************************************************************
 * FunctionName : user_devicefind_init
 * Description  : the espconn struct parame init
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR
user_devicefind_init(int port) {
	ptrespconn.type = ESPCONN_UDP;
	ptrespconn.proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));
	ptrespconn.proto.udp->local_port = port;
	espconn_regist_recvcb(&ptrespconn, user_devicefind_recv);
	espconn_create(&ptrespconn);
}
