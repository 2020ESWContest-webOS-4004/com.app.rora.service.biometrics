#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <glib-object.h>
#include <lunaservice.h>
#include <luna-service2/lunaservice.h>
#include <pbnjson.h>

// 생체인증 gpio pin
#define LEFT_WINDOW_LED 6
#define LEFT_SHOCK_SENSOR 12

#define BUF_SIZE 64

// biometrics 서비스에서 사용하는 메소드 선언
int gpio_init();
bool echo(LSHandle *sh, LSMessage *message, void *data);
void *knock_check_thread();