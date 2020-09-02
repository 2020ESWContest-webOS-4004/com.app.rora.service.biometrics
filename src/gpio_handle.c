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

static bool get_auth_callback(LSHandle* sh, LSMessage* msg, void* ctx)
{
    JSchemaInfo schemaInfo;
    LSError lserror;
    LSErrorInit(&lserror);

    char *s1 = "helo";

    FILE *fp = fopen("/home/root/abcd", "w");
    fwrite(s1, sizeof(s1), 1, fp);
    auth_flag = true;

    printf("%s\n", s1);

    fclose(fp);    // 파일 포인터 닫기*/
    return true;
    /*
    if (successCb && result_auth) // 이미 인증이 완료된 상태일 경우
        return false;
    else                         // 인증이 필요한 경우
        //LSCall(sh, "luna://com.app.rora.service.webcontrol/set_auth", "{\"value\":\"test!!\"}", "", NULL, NULL, &lserror);
        auth_flag = true;
        return true;
    */
}

//test code
bool echo(LSHandle *sh, LSMessage *message, void *data)
{
    LSError lserror;
    JSchemaInfo schemaInfo;
    jvalue_ref parsed = {0}, value = {0};
    jvalue_ref jobj = {0}, jreturnValue = {0};
    const char *input = NULL;
    char buf[BUF_SIZE] = {0, };

    LSErrorInit(&lserror);

    // Initialize schema
    jschema_info_init (&schemaInfo, jschema_all(), NULL, NULL);

    // get message from LS2 and parsing to make object
    parsed = jdom_parse(j_cstr_to_buffer(LSMessageGetPayload(message)), DOMOPT_NOOPT, &schemaInfo);

    if (jis_null(parsed)) {
        j_release(&parsed);
        return true;
    }

    // Get value from payload.input
    value = jobject_get(parsed, j_cstr_to_buffer("input"));

    // JSON Object to string without schema validation check
    input = jvalue_tostring_simple(value);

    /**
     * JSON create test
     */
    jobj = jobject_create();
    if (jis_null(jobj)) {
        j_release(&jobj);
        return true;
    }

    jreturnValue = jboolean_create(TRUE);
    jobject_set(jobj, j_cstr_to_buffer("returnValue"), jreturnValue);
    jobject_set(jobj, j_cstr_to_buffer("echoMessage"), value);

    LSMessageReply(sh, message, jvalue_tostring_simple(jobj), &lserror);

    j_release(&parsed);
    return true;
}

int tempcallback()
{
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

        fp = fopen(auth_path, "r");

        nResult = access(auth_path, 0);

        if(nResult == 0)
        {
            fgets(auth_data, sizeof(auth_data), fp);

            if(strlen(auth_data) > 0)   //인증된 상태
                auth_flag = true;
            else    // 미인증 상태
                auth_flag = false;
            fclose(fp);
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
                //인증 진행 요청 기능 구현예정
                //LSCall(sh, "luna://com.app.rora.service.webcontrol/set_auth", "{\"value\":\"test!!\"}", "", NULL, NULL, &lserror);
            }
            sleep(1);
        }
    }
}