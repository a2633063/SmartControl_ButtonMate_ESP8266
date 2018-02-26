/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"


#include "user_key.h"
#include "user_sntp.h"
#include "user_pwm.h"

LOCAL os_timer_t timer_rudder;

#if SNTP_ENABLE
static unsigned int tim=55,tim1=0;
#endif
void ICACHE_FLASH_ATTR user_os_timer_func(void *arg)
{
#if SNTP_ENABLE
	if(++tim1>=50){
		tim1=0;
		if(++tim>60-time.second)//10s
		{
			if( wifi_station_get_connect_status()== STATION_GOT_IP )
			{
				tim=1;
				user_check_sntp_stamp();
			}
		}
	}
#endif
	//舵机返回平衡位置
	sw_pwm_timer_cb();
}


void ICACHE_FLASH_ATTR
user_os_timer_init(void)
{
	os_timer_disarm(&timer_rudder);
	os_timer_setfn(&timer_rudder, (os_timer_func_t *) user_os_timer_func, NULL);
	os_timer_arm(&timer_rudder, 20, 1);
}
