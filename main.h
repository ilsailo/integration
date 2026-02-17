#ifndef SHARED_H
#define SHARED_H

#include <mqueue.h>
#include <fcntl.h>

// 4개의 독립적인 POSIX 메시지 큐 이름
#define MQ_TH      "/mq_th"
#define MQ_VITAL   "/mq_vital"
#define MQ_DB      "/mq_db"
#define MQ_SEND    "/mq_send"

// 1, 2번 모듈이 보내는 원본 데이터 구조
typedef struct {
    int type;          // 1: 온습도, 2: 워치
    float val1;        // 온도/심박수
    float val2;        // 습도/체온
    long timestamp;
// 물리적 설계에 맞춰서 향후에 수정 해야 할 것임.
} RawData;

// 3번(룰베이스)이 4번(DB)으로 넘기는 통합 데이터 구조
typedef struct {
    RawData raw;       // 원본 데이터 포함
    int status_code;   // 판단 결과
    char msg[32];      // 상태 메시지
// 물리적 설계를 보면, 원본 데이터들 + 이상상태 + 알림 + 센서데이터 패키징으로 만들어서 보내야할 것 같음
} DBData;

#endif
