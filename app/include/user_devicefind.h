#ifndef __USER_DEVICEFIND_H__
#define __USER_DEVICEFIND_H__

const char *device_find_request;

extern const char *device_find_response_ok;
extern const char *setting_pwm_max;
extern const char *setting_pwm_min;
extern const char *setting_pwm_middle;
extern const char *setting_pwm_middle_delay;
extern const char *setting_pwm_test;
extern const char *setting_get_all;
extern const char *setting_update;

void user_devicefind_init(int port);

#endif
