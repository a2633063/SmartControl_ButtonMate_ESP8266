#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#include "../cJson/cJSON.h"
#include "user_wifi.h"
#include "user_json.h"
#include "user_setting.h"

void ICACHE_FLASH_ATTR user_domoticz_mqtt_analysis(struct espconn *pesp_conn, u8* jsonRoot) {
	os_printf("get freeHeap1: %d \n\n", system_get_free_heap_size());

	//首先整体判断是否为一个json格式的数据
	cJSON *pJsonRoot = cJSON_Parse(jsonRoot);
	os_printf("get freeHeap2: %d \n\n", system_get_free_heap_size());
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
			os_printf("get freeHeap3: %d \n\n", system_get_free_heap_size());
			cJSON_free((void *) s);
			cJSON_Delete(pRoot);
		}
		os_printf("get freeHeap4: %d \n\n", system_get_free_heap_size());
		cJSON_Delete(p_cmd);

		os_printf("start json:other\r\n");
		//解析
		os_printf("get freeHeap5: %d \n\n", system_get_free_heap_size());
		cJSON *p_idx = cJSON_GetObjectItem(pJsonRoot, "idx");
		os_printf("get freeHeap6: %d \n\n", system_get_free_heap_size());
		cJSON *p_description = cJSON_GetObjectItem(pJsonRoot, "description");
		os_printf("get freeHeap7: %d \n\n", system_get_free_heap_size());
		cJSON *p_name = cJSON_GetObjectItem(pJsonRoot, "name");
		os_printf("get freeHeap8: %d \n\n", system_get_free_heap_size());
		cJSON *p_mac = cJSON_GetObjectItem(pJsonRoot, "mac");
		os_printf("get freeHeap9: %d \n\n", system_get_free_heap_size());

		//
		if (
				(p_idx && cJSON_IsNumber(p_idx) && p_idx->valueint == idx) 	//idx
				|| (p_description && cJSON_IsString(p_description) && os_strcmp(p_description->valuestring, mqtt_device_id) == 0)//name
				|| (p_name && cJSON_IsString(p_name) && os_strcmp(p_name->valuestring, mqtt_device_id) == 0)//name
				|| (p_mac && cJSON_IsString(p_mac) && os_strcmp(p_mac->valuestring, strMac) == 0)

		) {
			os_printf("device enter\r\n");
			cJSON *p_nvalue = cJSON_GetObjectItem(pJsonRoot, "nvalue");
			if (p_nvalue)
				user_rudder_press(p_nvalue->valueint);

			cJSON *p_setting = cJSON_GetObjectItem(pJsonRoot, "setting");
			if(p_setting){

			}

		}
		cJSON_Delete(p_idx);
		os_printf("get freeHeap10: %d \n\n", system_get_free_heap_size());
		cJSON_Delete(p_description);
		os_printf("get freeHeap11: %d \n\n", system_get_free_heap_size());
		cJSON_Delete(p_name);
		os_printf("get freeHeap12: %d \n\n", system_get_free_heap_size());
		cJSON_Delete(p_mac);
		os_printf("get freeHeap13: %d \n\n", system_get_free_heap_size());

	} else {
		os_printf("this is not a json data:\r\n%s\r\n", jsonRoot);
	}

	os_printf("get freeHeap14: %d \n\n", system_get_free_heap_size());
	cJSON_Delete(pJsonRoot);
	os_printf("get freeHeap5: %d \n\n", system_get_free_heap_size());
}

void creatJson() {

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

