#ifndef __USER_SETTING_H__
#define __USER_SETTING_H__

#define MQTT_IP_DEFAULT "192.168.1.2"
#define MQTT_PROT_DEFAULT 1883
#define MQTT_USER_DEFAULT "z"
#define MQTT_PASSWORD_DEFAULT "123456"

#define	SETTING_MQTT_STRING_LENGTH_MAX  32		//必须 4 字节对齐。
#define	SETTING_SAVE_MQTT_DEVICE_ID_NAME  "ButtonMate_%X%X"		//必须 4 字节对齐。

extern uint32 pwm_middle;	//舵机中间位置
extern uint32 pwm_max;		//舵机最大角度
extern uint32 pwm_min;		//舵机最小角度
extern uint32 rudder_middle_delay;		//舵机最小角度

extern int32 idx;	//domoticz mqtt数据 idx值

extern uint8 mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];	//mqtt service ip
extern uint16 mqtt_port;		//mqtt service port
extern uint8 mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user
extern uint8 mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user
extern uint8 mqtt_device_id[SETTING_MQTT_STRING_LENGTH_MAX];		//mqtt service user

#define	SETTING_SAVE_PWM_MAX_ADDR  0x80
#define	SETTING_SAVE_PWM_MIN_ADDR	0x81
#define	SETTING_SAVE_PWM_MIDDLE_ADDR	0x82
#define	SETTING_SAVE_PWM_MIDDLE_DELAY_ADDR	0x83

#define	SETTING_SAVE_IDX_ADDR  0x84

#define	SETTING_SAVE_MQTT_IP_ADDR  0x90
#define	SETTING_SAVE_MQTT_PORT_ADDR  0x91
#define	SETTING_SAVE_MQTT_USER_ADDR  0x92
#define	SETTING_SAVE_MQTT_PASSWORD_ADDR  0x96
#define	SETTING_SAVE_MQTT_DEVICE_ID_ADDR  0x9a



void ser_setting_init(void);

void user_setting_set_pwm_max(uint32 val);
void user_setting_set_pwm_min(uint32 val);
void user_setting_set_pwm_middle(uint32 val);
void user_setting_set_pwm_middle_delay(uint32 val);
uint32 user_setting_get_pwm_max(void);
uint32 user_setting_get_pwm_min(void);
uint32 user_setting_get_pwm_middle(void);
uint32 user_setting_get_pwm_middle_delay(void);
uint32 user_setting_get_idx(void);

void user_setting_set_idx(uint32 val);

void user_setting_set_mqtt_ip(uint8 * p);
void user_setting_get_mqtt_ip();
void user_setting_set_mqtt_port(uint32 val);
uint32 user_setting_get_mqtt_port(void);
void user_setting_set_mqtt_user(uint8 * p);
void user_setting_get_mqtt_user(void);
void user_setting_set_mqtt_password(uint8 * p);
void user_setting_get_mqtt_password(void);
void user_setting_set_mqtt_device_id(uint8 * p);
void user_setting_get_mqtt_device_id(void);
#endif
