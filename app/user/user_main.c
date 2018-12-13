/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
 *******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"

#include "user_setting.h"
#include "user_os_timer.h"
#include "user_sntp.h"
#include "user_pwm.h"
#include "user_devicefind.h"
//#include "user_webserver.h"
#include "user_tcp.h"
#include "user_webserver.h"




void user_rf_pre_init(void) {
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void user_init(void) {
	int i, j;
	uint32 x;
	uart_init(115200, 115200);
	os_printf("\n\nStart user%d.bin\n", system_upgrade_userbin_check() + 1);
	os_printf("SDK version:%s\n", system_get_sdk_version());

	user_setting_init();
	user_wifi_init();
	user_key_init();
	user_led_init();
	user_pwm_init();
#if SNTP_ENABLE
	user_sntp_init();
#endif
	//UDP��ʼ��,�����˿�10191,�����յ��ض��ַ���ʱ,���ر��豸IP��MAC��ַ
	user_devicefind_init(10191);
	//TCP��ʼ��,�����˿�10191
	user_tcp_init(10191);
	//TCP��ʼ�� 80�˿� webserver
	//user_webserver_init(80);



}

