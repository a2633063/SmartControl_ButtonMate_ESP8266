#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "driver\key.h"
#include "user_key.h"
#include "user_led.h"
#include "user_wifi.h"

LOCAL struct keys_param keys;
LOCAL struct single_key_param *single_key[GPIO_KEY_NUM];

LOCAL unsigned char key_press_flag = 0;	//按键长按标志位,防止按键长按后松开时执行短按代码

LOCAL void ICACHE_FLASH_ATTR
user_key_short_press(void) {
	if (key_press_flag == 1) {	//防止按键长按后松开时执行短按代码
		key_press_flag = 0;
		return;
	}
	user_smartconfig_stop();
	os_printf("key_short_press\n");
//	user_set_led(!user_get_led());
	user_rudder_press(0);
}

LOCAL void ICACHE_FLASH_ATTR
user_key_long_press(void) {
	key_press_flag = 1;
	os_printf("key_long_press\n");
	user_smartconfig();
}

void ICACHE_FLASH_ATTR
user_key_init(void) {
	single_key[0] = key_init_single(GPIO_KEY_0_IO_NUM, GPIO_KEY_0_IO_MUX,
	GPIO_KEY_0_IO_FUNC, user_key_long_press, user_key_short_press);

	keys.key_num = GPIO_KEY_NUM;
	keys.single_key = single_key;
	key_init(&keys);
}
