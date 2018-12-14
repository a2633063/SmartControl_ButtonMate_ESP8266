#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"

#include "../cJson/cJSON.h"

#include "user_json.h"
#include "user_setting.h"

void ICACHE_FLASH_ATTR user_domoticz_mqtt_analysis(u8* jsonRoot) {
	//os_printf("get freeHeap1: %d \n\n", system_get_free_heap_size());

	//���������ж��Ƿ�Ϊһ��json��ʽ������
	cJSON *pJsonRoot = cJSON_Parse(jsonRoot);
	//����Ƿ�json��ʽ����
	if (pJsonRoot !=NULL) {


		char *s = cJSON_Print(pJsonRoot);
		os_printf("pJsonRoot: %s\r\n", s);
		cJSON_free((void *) s);



		//����idx�ֶ�int����
		cJSON *p_idx = cJSON_GetObjectItem(pJsonRoot, "idx");
		cJSON *p_description = cJSON_GetObjectItem(pJsonRoot, "description");
		//
		if (
				(p_idx && cJSON_IsNumber(p_idx) && p_idx->valueint==idx)
				||(p_description && cJSON_IsString(p_description) && os_strcmp(p_description->valuestring, mqtt_device_id) == 0)
		){
			cJSON *p_nvalue = cJSON_GetObjectItem(pJsonRoot, "nvalue");
			if(p_nvalue) user_rudder_press(p_nvalue->valueint);
		}




//
//		//����mac�ֶ��ַ�������
//		cJSON *pMacAdress = cJSON_GetObjectItem(pJsonRoot, "mac");
//		//�ж�mac�ֶ��Ƿ�json��ʽ
//		if (pMacAdress) {
//			//�ж�mac�ֶ��Ƿ�string����
//			if (cJSON_IsString(pMacAdress))
//				os_printf("get MacAdress:%s \n", pMacAdress->valuestring);
//
//		} else
//			os_printf("get MacAdress failed \n");
//
//
//
//		//����number�ֶ�int����
//		cJSON *pNumber = cJSON_GetObjectItem(pJsonRoot, "number");
//		//�ж�number�ֶ��Ƿ����
//		if (pNumber){
//			if (cJSON_IsNumber(pNumber))
//			os_printf("get Number:%d \n", pNumber->valueint);
//		}
//		else
//			os_printf("get Number failed \n");
//
//		//����value�ֶ����ݣ��ж��Ƿ�Ϊjson
//		cJSON *pValue = cJSON_GetObjectItem(pJsonRoot, "value");
//		if (pValue) {
//			//��һ�����������name�ֶ�:ע��������ڵ��� pValue
//			cJSON *pName = cJSON_GetObjectItem(pValue, "name");
//			if (pName)
//				if (cJSON_IsString(pName))
//					os_printf("get value->Name : %s \n", pName->valuestring);
//
//
//			//��һ�����������age�ֶ�:ע��������ڵ��� pValue
//			cJSON *pAge = cJSON_GetObjectItem(pValue, "age");
//			if (pAge)
//				if (cJSON_IsNumber(pAge))
//					os_printf("get value->Age : %d \n", pAge->valueint);
//
//
//			//��һ�����������blog�ֶ�:ע��������ڵ��� pValue
//			cJSON *pBlog= cJSON_GetObjectItem(pValue, "blog");
//			if (pBlog)
//				if (cJSON_IsString(pBlog))
//				os_printf("get value->pBlog	 : %s \n", pBlog->valuestring);
//		}
//
//		//����
//		cJSON *pArry = cJSON_GetObjectItem(pJsonRoot, "hexArry");
//		if (pArry) {
//			//��ȡ���鳤��
//			int arryLength = cJSON_GetArraySize(pArry);
//			os_printf("get arryLength : %d \n", arryLength);
//			//�����ӡ
//			int i ;
//			for (i = 0; i < arryLength; i++)
//				os_printf("cJSON_GetArrayItem(pArry, %d)= %d \n",i,cJSON_GetArrayItem(pArry, i)->valueint);
//		}


	} else {
		os_printf("this is not a json data:\r\n%s\r\n",jsonRoot);
	}


	cJSON_Delete(pJsonRoot);
	//os_printf("get freeHeap2: %d \n\n", system_get_free_heap_size());
}


void creatJson(){

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


	//ȡһ�±��ص�station��mac��ַ


	cJSON *pRoot = cJSON_CreateObject();
	cJSON *pValue = cJSON_CreateObject();

	uint8 tempMessage[6];
	tempMessage[0]=1;
	tempMessage[1]=2;
	tempMessage[2]=3;
	tempMessage[3]=4;
	tempMessage[4]=5;
	tempMessage[5]=6;
	cJSON_AddStringToObject(pRoot,"mac",tempMessage);
	cJSON_AddNumberToObject(pRoot,"number",2);

	cJSON_AddStringToObject(pValue,"mac","xuhongv");
	cJSON_AddNumberToObject(pValue,"age",18);
	cJSON_AddStringToObject(pValue,"mac","https://blog.csdn.net/xh870189248");

	cJSON_AddItemToObject(pRoot, "value",pValue);

    int hex[5]={51,15,63,22,96};
	cJSON *pHex = cJSON_CreateIntArray(hex,5);
	cJSON_AddItemToObject(pRoot,"hex",pHex);

	char *s = cJSON_Print(pRoot);
	os_printf("\r\n creatJson : %s\r\n", s);
	cJSON_free((void *) s);

	cJSON_Delete(pRoot);
}


