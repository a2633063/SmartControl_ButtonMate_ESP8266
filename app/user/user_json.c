#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#include "../cJson/cJSON.h"
#include "user_pwm.h"
#include "user_wifi.h"
#include "user_json.h"
#include "user_setting.h"

uint32 last_time = 0;

void ICACHE_FLASH_ATTR user_json_set_last_time() {
	last_time = system_get_time();
}

void ICACHE_FLASH_ATTR user_domoticz_mqtt_analysis(struct espconn *pesp_conn, u8* jsonRoot) {
//	os_printf("get freeHeap1: %d \n\n", system_get_free_heap_size());
	bool return_flag = true;	//为true时返回json结果,否则不返回
//首先整体判断是否为一个json格式的数据
	cJSON *pJsonRoot = cJSON_Parse(jsonRoot);
	//如果是否json格式数据
	if (pJsonRoot != NULL) {

		//串口打印数据
//		char *s = cJSON_Print(pJsonRoot);
//		os_printf("pJsonRoot: %s\r\n", s);
//		cJSON_free((void *) s);

//解析device report
		os_printf("start json:device report\r\n");
		cJSON *p_cmd = cJSON_GetObjectItem(pJsonRoot, "cmd");
		if (p_cmd && cJSON_IsString(p_cmd) && os_strcmp(p_cmd->valuestring, "device report") == 0) {

			cJSON *pRoot = cJSON_CreateObject();
			cJSON_AddStringToObject(pRoot, "name", mqtt_device_id);
			cJSON_AddStringToObject(pRoot, "mac", strMac);
			cJSON_AddNumberToObject(pRoot, "type", TYPE);
			cJSON_AddStringToObject(pRoot, "type_name", TYPE_NAME);
			char *s = cJSON_Print(pRoot);
			os_printf("pRoot: %s\r\n", s);

			if (pesp_conn) {
				if (pesp_conn && pesp_conn->type == ESPCONN_UDP) {
					pesp_conn->type = ESPCONN_UDP;
					pesp_conn->proto.udp->remote_port = 10181;	//获取端口
					pesp_conn->proto.udp->remote_ip[0] = 255;	//获取IP地址
					pesp_conn->proto.udp->remote_ip[1] = 255;
					pesp_conn->proto.udp->remote_ip[2] = 255;
					pesp_conn->proto.udp->remote_ip[3] = 255;
				}
				espconn_send(pesp_conn, s, os_strlen(s));	//发送数据
			} else {
				user_mqtt_send("domoticz/in", s);
			}
			cJSON_free((void *) s);
			cJSON_Delete(pRoot);
//			cJSON_Delete(p_cmd);
		}

//		os_printf("start json:other\r\n");
		//解析
		cJSON *p_idx = cJSON_GetObjectItem(pJsonRoot, "idx");
		cJSON *p_description = cJSON_GetObjectItem(pJsonRoot, "description");
		cJSON *p_name = cJSON_GetObjectItem(pJsonRoot, "name");
		cJSON *p_mac = cJSON_GetObjectItem(pJsonRoot, "mac");

		//
		if ((p_idx && cJSON_IsNumber(p_idx) && p_idx->valueint == idx) 	//idx
		|| (p_description && cJSON_IsString(p_description) && os_strcmp(p_description->valuestring, mqtt_device_id) == 0) 	//name
				|| (p_name && cJSON_IsString(p_name) && os_strcmp(p_name->valuestring, mqtt_device_id) == 0) 	//name
				|| (p_mac && cJSON_IsString(p_mac) && os_strcmp(p_mac->valuestring, strMac) == 0)	//mac
				) {
//			os_printf("device enter\r\n");
			cJSON *json_send = cJSON_CreateObject();
			cJSON_AddStringToObject(json_send, "mac", strMac);

			cJSON *p_nvalue = cJSON_GetObjectItem(pJsonRoot, "nvalue");
			if (p_nvalue && cJSON_IsNumber(p_nvalue)) {
				uint32 now_time = system_get_time();
				os_printf("system_get_time:%d,%d = %09d\r\n", last_time, now_time, now_time - last_time);
				if (now_time - last_time < 1500000 && p_idx && p_nvalue->valueint == user_rudder_get_direction()) {
					return_flag = false;
				} else {
					user_rudder_press(p_nvalue->valueint);
				}
				user_json_set_last_time();
			}

			if (p_nvalue) {
				cJSON_AddNumberToObject(json_send, "nvalue", user_rudder_get_direction());
			} else
				last_time = 0;

			cJSON *p_setting = cJSON_GetObjectItem(pJsonRoot, "setting");
			if (p_setting) {
				cJSON *json_setting_send = cJSON_CreateObject();
				//设置设备名称/deviceid
				cJSON *p_setting_name = cJSON_GetObjectItem(p_setting, "name");
				if (p_setting_name && cJSON_IsString(p_setting_name)) {
					user_setting_set_mqtt_device_id(p_setting_name->valuestring);
				}

				//设置mqtt ip
				cJSON *p_mqtt_ip = cJSON_GetObjectItem(p_setting, "mqtt_uri");
				if (p_mqtt_ip && cJSON_IsString(p_mqtt_ip)) {
					user_setting_set_mqtt_ip(p_mqtt_ip->valuestring);
				}

				//设置mqtt port
				cJSON *p_mqtt_port = cJSON_GetObjectItem(p_setting, "mqtt_port");
				if (p_mqtt_port && cJSON_IsNumber(p_mqtt_port)) {
					user_setting_set_mqtt_port(p_mqtt_port->valueint);
				}

				//设置mqtt user
				cJSON *p_mqtt_user = cJSON_GetObjectItem(p_setting, "mqtt_user");
				if (p_mqtt_user && cJSON_IsString(p_mqtt_user)) {
					user_setting_set_mqtt_user(p_mqtt_user->valuestring);
				}

				//设置mqtt password
				cJSON *p_mqtt_password = cJSON_GetObjectItem(p_setting, "mqtt_password");
				if (p_mqtt_password && cJSON_IsString(p_mqtt_password)) {
					user_setting_set_mqtt_password(p_mqtt_password->valuestring);
				}

				//设置domoticz idx
				cJSON *p_setting_idx = cJSON_GetObjectItem(p_setting, "idx");
				if (p_setting_idx && cJSON_IsNumber(p_setting_idx)) {
					user_setting_set_idx(p_setting_idx->valueint);
				}

				//设置设备参数
				//设置左侧按键 最小
				cJSON *p_setting_min = cJSON_GetObjectItem(p_setting, "min");
				if (p_setting_min && cJSON_IsNumber(p_setting_min)) {
					int k = p_setting_min->valueint;
					if (k >= 0 && k <= 180) {
						user_setting_set_pwm_min(k * 10 + k / 2 + PWM_MIN_CYCLE);

					}

				}

				//设置右侧按键 最大
				cJSON *p_setting_max = cJSON_GetObjectItem(p_setting, "max");
				if (p_setting_max && cJSON_IsNumber(p_setting_max)) {
					int k = p_setting_max->valueint;
					if (k >= 0 && k <= 180) {
						user_setting_set_pwm_max(k * 10 + k / 2 + PWM_MIN_CYCLE);
					}
				}

				//设置平衡位置
				cJSON *p_setting_middle = cJSON_GetObjectItem(p_setting, "middle");
				if (p_setting_middle && cJSON_IsNumber(p_setting_middle)) {
					int k = p_setting_middle->valueint;
					if (k >= 0 && k <= 180) {
						user_setting_set_pwm_middle(k * 10 + k / 2 + PWM_MIN_CYCLE);
					}

				}
				//设置平衡延时时间
				cJSON *p_setting_middle_delay = cJSON_GetObjectItem(p_setting, "middle_delay");
				if (p_setting_middle_delay && cJSON_IsNumber(p_setting_middle_delay)) {
					user_setting_set_pwm_middle_delay(p_setting_middle_delay->valueint);
				}

				//测试角度
				cJSON *p_setting_pwm_test = cJSON_GetObjectItem(p_setting, "test");
				if (p_setting_pwm_test && cJSON_IsNumber(p_setting_pwm_test)) {
					int k = p_setting_pwm_test->valueint;
					if (k >= 0 && k <= 180) {
					user_rudder_test(k * 10 + k / 2 + PWM_MIN_CYCLE);
					cJSON_AddNumberToObject(json_setting_send, "test",k * 10 + k / 2 + PWM_MIN_CYCLE);
					}
				}

				//开发返回数据
				//设置设备名称/deviceid
				if (p_setting_name) {
					cJSON_AddStringToObject(json_setting_send, "name", mqtt_device_id);
				}

				//设置mqtt ip
				if (p_mqtt_ip) {
					cJSON_AddStringToObject(json_setting_send, "mqtt_uri", mqtt_ip);
				}

				//设置mqtt port
				if (p_mqtt_port) {
					cJSON_AddNumberToObject(json_setting_send, "mqtt_port", mqtt_port);
				}

				//设置mqtt user
				if (p_mqtt_user) {
					cJSON_AddStringToObject(json_setting_send, "mqtt_user", mqtt_user);
				}

				//设置mqtt password
				if (p_mqtt_password) {
					cJSON_AddStringToObject(json_setting_send, "mqtt_password", mqtt_password);
				}

				//设置domoticz idx
				if (p_setting_idx) {
					cJSON_AddNumberToObject(json_setting_send, "idx", idx);
				}

				//设置设备参数
				//设置左侧按键 最小
				if (p_setting_min) {
					cJSON_AddNumberToObject(json_setting_send, "min", (pwm_min - PWM_MIN_CYCLE + 1) / 10.5);
				}

				//设置右侧按键 最大
				if (p_setting_max) {
					cJSON_AddNumberToObject(json_setting_send, "max", (pwm_max - PWM_MIN_CYCLE + 1) / 10.5);
				}

				//设置平衡位置
				if (p_setting_middle) {
					cJSON_AddNumberToObject(json_setting_send, "middle", (pwm_middle - PWM_MIN_CYCLE + 1) / 10.5);
				}

				//设置平衡延时时间
				if (p_setting_middle_delay) {
					cJSON_AddNumberToObject(json_setting_send, "middle_delay", rudder_middle_delay);
				}

				cJSON_AddItemToObject(json_send, "setting", json_setting_send);
			}

			cJSON_AddStringToObject(json_send, "name", mqtt_device_id);

			//if (p_idx)
			if (idx >= 0)
				cJSON_AddNumberToObject(json_send, "idx", idx);

			if (return_flag == true) {
				char *json_str = cJSON_Print(json_send);
				os_printf("pRoot: %s\r\n", json_str);
				if (pesp_conn) {
					if (pesp_conn && pesp_conn->type == ESPCONN_UDP) {
						pesp_conn->type = ESPCONN_UDP;
						pesp_conn->proto.udp->remote_port = 10181;	//获取端口
						pesp_conn->proto.udp->remote_ip[0] = 255;	//获取IP地址
						pesp_conn->proto.udp->remote_ip[1] = 255;
						pesp_conn->proto.udp->remote_ip[2] = 255;
						pesp_conn->proto.udp->remote_ip[3] = 255;
					}
					espconn_send(pesp_conn, json_str, os_strlen(json_str));	//发送数据
				} else {
					user_mqtt_send("domoticz/in", json_str);
				}
				cJSON_free((void *) json_str);
			}
			cJSON_Delete(json_send);
		}

	} else {
		os_printf("this is not a json data:\r\n%s\r\n", jsonRoot);
	}

	cJSON_Delete(pJsonRoot);
//	os_printf("get freeHeap2: %d \n\n", system_get_free_heap_size());
}

void ICACHE_FLASH_ATTR creatJson() {

	/*
	 {
	 "mac":"84:f3:eb:b3:a7:05",
	 "number":2,
	 "value":{"name":"xuhongv",
	 "age":18 ,
	 "blog":"https://blog.csdn.net/xh870189248"
	 },
	 "hex":[51,15,63,22,96]
	 }
	 */

	//取一下本地的station的mac地址
	cJSON *pRoot = cJSON_CreateObject();
	cJSON *pValue = cJSON_CreateObject();

	uint8 tempMessage[6];
	tempMessage[0] = 1;
	tempMessage[1] = 2;
	tempMessage[2] = 3;
	tempMessage[3] = 4;
	tempMessage[4] = 5;
	tempMessage[5] = 6;
	cJSON_AddStringToObject(pRoot, "mac", tempMessage);
	cJSON_AddNumberToObject(pRoot, "number", 2);

	cJSON_AddStringToObject(pValue, "mac", "xuhongv");
	cJSON_AddNumberToObject(pValue, "age", 18);
	cJSON_AddStringToObject(pValue, "mac", "https://blog.csdn.net/xh870189248");

	cJSON_AddItemToObject(pRoot, "value", pValue);

	int hex[5] = { 51, 15, 63, 22, 96 };
	cJSON *pHex = cJSON_CreateIntArray(hex, 5);
	cJSON_AddItemToObject(pRoot, "hex", pHex);

	char *s = cJSON_Print(pRoot);
	os_printf("\r\n creatJson : %s\r\n", s);
	cJSON_free((void *) s);

	cJSON_Delete(pRoot);
}

