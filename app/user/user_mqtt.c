#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"

#include "../mqtt/include/debug.h"
#include "../mqtt/include/mqtt.h"
#include "../mqtt/include/mqtt_config.h"

#include "user_setting.h"
#include "user_mqtt.h"
#include "user_json.h"
MQTT_Client mqttClient;

void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Connected\r\n");
    MQTT_Subscribe(client, "domoticz/out", 0);
    MQTT_Subscribe(client, mqtt_device_id, 2);



    MQTT_Publish(client, "/mqtt/topic/0", "hello0", 6, 0, 0);
    MQTT_Publish(client, "/mqtt/topic/1", "hello1", 6, 1, 0);
    MQTT_Publish(client, "/mqtt/topic/2", "hello2", 6, 2, 0);

}

void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: 断开连接\r\n");
}

void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Published\r\n");
}

void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1),
            *dataBuf = (char*)os_zalloc(data_len+1);

    MQTT_Client* client = (MQTT_Client*)args;

    os_memcpy(topicBuf, topic, topic_len);
    topicBuf[topic_len] = 0;

    os_memcpy(dataBuf, data, data_len);
    dataBuf[data_len] = 0;

    //os_printf("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
    user_domoticz_mqtt_analysis(dataBuf);
    os_free(topicBuf);
    os_free(dataBuf);
}

void ICACHE_FLASH_ATTR user_mqtt_connect(void) {
	MQTT_Connect(&mqttClient);
}

void ICACHE_FLASH_ATTR user_mqtt_disconnect(void) {
    MQTT_Disconnect(&mqttClient);
}

void ICACHE_FLASH_ATTR user_mqtt_init(void) {

	//MQTT初始化

    MQTT_InitConnection(&mqttClient, "192.168.43.247", mqtt_port, NO_TLS);

    MQTT_InitClient(&mqttClient, "Device_ID", mqtt_user, mqtt_password, MQTT_KEEPALIVE, 1);

    MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
    MQTT_OnConnected(&mqttClient, mqttConnectedCb);
    MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
    MQTT_OnPublished(&mqttClient, mqttPublishedCb);
    MQTT_OnData(&mqttClient, mqttDataCb);
}
