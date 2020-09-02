/**
 * service      :   com.app.rora.service.biometrics
 * date         :   2020.08.14
 * author       :   소찬영(RORA PM)
 * description  :   자동차 창문을 두드리면 바이오 인증 시스템이 동작함 (창문에 달린 충격센서 감지를 확인하는 native 서비스)
 */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <glib-object.h>
#include <lunaservice.h>
#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <pthread.h>
#include "../include/gpio_handle.h"

// 서비스 이름
#define SERVICE_NAME "com.app.rora.service.biometrics"

// Main loop for aliving background service
GMainLoop *gmainLoop;

LSHandle  *sh = NULL;
LSMessage *message;

// com.app.rora.service.biometrics 서비스 메소드 배열
LSMethod servicemethods[] = {
};

static bool testcb(LSHandle *sh, LSMessage *message, void* user_data){
    LSError lserror;
    JSchemaInfo schemaInfo;
    LSErrorInit(&lserror);
    jvalue_ref parsed = {0};

    jschema_info_init (&schemaInfo, jschema_all(), NULL, NULL);

    parsed = jdom_parse(j_cstr_to_buffer(LSMessageGetPayload(message)), DOMOPT_NOOPT, &schemaInfo);

    if (jis_null(parsed)) {
        j_release(&parsed);
        return true;
    }
    return true;

}

// 백그라운드 서비스 등록 및 초기화
int main(int argc, char* argv[])
{
    LSError lserror;
    LSHandle  *handle = NULL;
    bool bRetVal = FALSE;
    pthread_t threadId;

    LSErrorInit(&lserror);

    // GMainLoop 생성
    gmainLoop = g_main_loop_new(NULL, FALSE);

    bRetVal = LSRegister(SERVICE_NAME, &handle, &lserror);
    if (FALSE== bRetVal) {
        LSErrorFree( &lserror );
        return 0;
    }
    sh = LSMessageGetConnection(message);

    LSRegisterCategory(handle,"/",servicemethods, NULL, NULL, &lserror);
    LSGmainAttach(handle, gmainLoop, &lserror);

    gpio_init();    // gpio 초기화

    pthread_create(&threadId, NULL, knock_check_thread, handle);    // 운전석 창문 knock thread 실행 

    g_main_loop_run(gmainLoop);
    g_main_loop_unref(gmainLoop);

    return 0;
}