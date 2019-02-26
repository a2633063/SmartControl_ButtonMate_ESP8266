#ifndef __USER_MQTT_H__
#define __USER_MQTT_H__


BOOL user_mqtt_send( const char* topic, const char* data);
void ICACHE_FLASH_ATTR user_mqtt_connect(void);
void ICACHE_FLASH_ATTR user_mqtt_disconnect(void);
void ICACHE_FLASH_ATTR user_mqtt_init(void);

#endif

