/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_webserver.c
 *
 * Description: The web server mode configration.
 *              Check your hardware connection with the host while use this mode.
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
#include "user_tcp.h"
#include "user_pwm.h"

#include "user_update.h"

LOCAL struct espconn ptrespconn;


/******************************************************************************
 * FunctionName : webserver_recv
 * Description  : Processing the received data from the server
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
 *******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
tcp_recv(void *arg, char *pusrdata, unsigned short length) {
	struct espconn *pesp_conn = arg;
	if (length == 1 && *pusrdata == 127)
		return;

	char DeviceBuffer[70] = { 0 };
//	char Device_mac_buffer[60] = { 0 };
	char hwaddr[6];
	int i, j, k;
	remot_info *premot = NULL;
	struct ip_info ipconfig;

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

	if (length == 1 && *pusrdata == '+') {
		user_rudder_press(1);
	} else if (length == 1 && *pusrdata == '-') {
		user_rudder_press(0);
	} else if (length == os_strlen(setting_update)
			&& os_strncmp(pusrdata, setting_update, os_strlen(setting_update)) == 0) {



		os_printf("\nupdate\n" );
		os_printf("user bin:%d\n", system_upgrade_userbin_check());
		user_update();
	} else if (length == os_strlen(setting_get_all)
			&& os_strncmp(pusrdata, setting_get_all, os_strlen(setting_get_all)) == 0) {

		i = (pwm_max - PWM_MIN_CYCLE + 1) / 10.5;
		j = (pwm_min - PWM_MIN_CYCLE + 1) / 10.5;
		k = (pwm_middle - PWM_MIN_CYCLE + 1) / 10.5;
		os_sprintf(DeviceBuffer, "%s%03d\n%s%03d\n%s%03d\n%s%03d\n", setting_pwm_max, i, setting_pwm_min, j,
				setting_pwm_middle, k, setting_pwm_middle_delay, rudder_middle_delay);

		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));
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
			user_set_led(0);
			user_setting_set_pwm_middle_delay(k);
			user_set_led(1);
//			os_printf("pwm_delay:%d\n", rudder_middle_delay);
		}
		os_sprintf(DeviceBuffer, "%s" "%03d\n", setting_pwm_middle_delay, rudder_middle_delay);
		os_printf("%s", DeviceBuffer);
		espconn_sent(pesp_conn, DeviceBuffer, os_strlen(DeviceBuffer));

	} else {
		os_printf("tcp get:%s\n", pusrdata);
	}

}

/******************************************************************************
 * FunctionName : webserver_recon
 * Description  : the connection has been err, reconnection
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
 *******************************************************************************/
LOCAL ICACHE_FLASH_ATTR
void tcp_recon(void *arg, sint8 err) {
	struct espconn *pesp_conn = arg;

	os_printf("tcp reconnect: %d.%d.%d.%d:%d err %d \n", pesp_conn->proto.tcp->remote_ip[0],
			pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3],
			pesp_conn->proto.tcp->remote_port, err);
}

/******************************************************************************
 * FunctionName : webserver_recon
 * Description  : the connection has been err, reconnection
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
 *******************************************************************************/
LOCAL ICACHE_FLASH_ATTR
void tcp_discon(void *arg) {
	struct espconn *pesp_conn = arg;

	os_printf("tcp disconnect: %d.%d.%d.%d:%d disconnect\n", pesp_conn->proto.tcp->remote_ip[0],
			pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3],
			pesp_conn->proto.tcp->remote_port);
}

LOCAL ICACHE_FLASH_ATTR
void tcp_con(void *arg) {
	struct espconn *pesp_conn = arg;

	os_printf("tcp connected: %d.%d.%d.%d:%d \n", pesp_conn->proto.tcp->remote_ip[0],
			pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3],
			pesp_conn->proto.tcp->remote_port);
}

/******************************************************************************
 * FunctionName : user_accept_listen
 * Description  : server listened a connection successfully
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
 *******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
tcp_listen(void *arg) {
	struct espconn *pesp_conn = arg;

	espconn_regist_connectcb(pesp_conn, tcp_con);
	espconn_regist_recvcb(pesp_conn, tcp_recv);
	espconn_regist_reconcb(pesp_conn, tcp_recon);
	espconn_regist_disconcb(pesp_conn, tcp_discon);
}

/******************************************************************************
 * FunctionName : user_webserver_init
 * Description  : parameter initialize as a server
 * Parameters   : port -- server port
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR
user_tcp_init(uint32 port) {
	LOCAL struct espconn esp_conn;
	LOCAL esp_tcp esptcp;

	esp_conn.type = ESPCONN_TCP;
	esp_conn.state = ESPCONN_NONE;
	esp_conn.proto.tcp = &esptcp;
	esp_conn.proto.tcp->local_port = port;
	espconn_regist_connectcb(&esp_conn, tcp_listen);

	espconn_accept(&esp_conn);

}
