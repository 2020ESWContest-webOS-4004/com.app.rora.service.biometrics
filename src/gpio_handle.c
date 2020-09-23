/**
 * service      :   com.app.rora.service.biometrics
 * date         :   2020.08.14
 * author       :   소찬영(RORA PM)
 * description  :   생체인증 서비스에 사용되는 메소드 정의
 */

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <stdio.h>
#include <glib-object.h>
#include <lunaservice.h>
#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/gpio_handle.h"

#define AVAILABLE 0

bool auth_flag = false;

// gpio 초기화 함수
int gpio_init()
{
    LSError lserror;
    if(wiringPiSetup() == -1)
        return 1;
        
    //커넥티드카 운전자 왼쪽 창문 gpio 핀 활성화
    pinMode(LEFT_WINDOW_LED, OUTPUT);   // 상태 알림 LED
    pinMode(LEFT_SHOCK_SENSOR, INPUT);  // 왼쪽창문 knock 센서

    return 0;
}

//운전석 창문 knock 확인하는 thread 메소드
void *knock_check_thread(LSHandle  *sh)
{
    LSError lserror;
    int val;
    int nResult;
    FILE *fp;
    char auth_data[20] = {0,};
    char *auth_path = "/home/root/user_auth";

    while(1){
        if(wiringPiSetup() == -1)
            return 1;
        val = digitalRead(LEFT_SHOCK_SENSOR);   // 노크센서에 값이 들어왔는지 확인하는 변수

        nResult = access(auth_path, 0);

        if(nResult == AVAILABLE)
        {
            fp = fopen(auth_path, "r");
            fgets(auth_data, sizeof(auth_data), fp);

            if(strlen(auth_data) > 0)   //인증된 상태
                auth_flag = true;
            else    // 미인증 상태
                auth_flag = false;

            fclose(fp);

            memset(auth_data, 0, sizeof(auth_data));
        }
        else // 미인증 상태
            auth_flag = false;

        if(!auth_flag){
            if(val == HIGH){
                digitalWrite(LEFT_WINDOW_LED, HIGH);
                //  인증을 위한 요청이 필요
                LSCall(sh, "luna://com.app.rora.service.webcontrol/request_camera", "{}", NULL, sh, NULL, &lserror);
                digitalWrite(LEFT_WINDOW_LED, LOW);
            }
            else{
                digitalWrite(LEFT_WINDOW_LED, LOW);
            }
            sleep(1);
        }
    }
}