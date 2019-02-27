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
const char *setting_mqtt_ip = "mqtt_ip=";
const char *setting_mqtt_port = "mqtt_port=";
const char *setting_mqtt_user = "mqtt_user=";
const char *setting_mqtt_password = "mqtt_password=";
const char *setting_mqtt_device_id = "mqtt_device_id=";

void ICACHE_FLASH_ATTR
user_con_received(void *arg, char *pusrdata, unsigned short length) {
	if (length == 1 && *pusrdata == 127)
		return;

	struct espconn *pesp_conn = arg;

	int i, j;
	uint32_t k;

//	os_printf("result:%s\n", pusrdata);

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


	user_domoticz_mqtt_analysis(pesp_conn,pusrdata);

}
