#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- [1] DB 테이블별 데이터 구조체 --- */

typedef struct {
    int sen_id; int wp_id; float temp; float humd; time_t time;
} THData;

typedef struct {
    int sen_id; int wp_id; float sk_temp; float hr; time_t time;
} VitalData;

typedef struct {
    int sen_id; int wp_id; char detail[201]; time_t time;
} SituData;

typedef struct {
    int dept_id;
    int wp_id;
    int status_code;      // 추가: 테스트 코드와의 일관성을 위해 정수형 코드 추가
    char state_code[101]; 
    char detail[201];
    time_t time;
} EventData;

/* --- [2] 통합 데이터 패키지 --- */

typedef enum { TYPE_TH, TYPE_VITAL, TYPE_SITU, TYPE_EVENT } DataType;

typedef struct {
    DataType type;
    union {
        THData th; VitalData vital; SituData situ; EventData event;
    } payload;
} SensorPacket;

/* --- [3] Thread-Safe Queue 관련 --- */

#define QUEUE_SIZE 20
typedef struct {
    void* data[QUEUE_SIZE];
    int head, tail, count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty, not_full;
} TSQueue;

extern TSQueue q_th, q_vital, q_db, q_send;

void q_init(TSQueue *q);
void q_push(TSQueue *q, void* item);
void* q_pop(TSQueue *q);

/* 모듈 진입 함수 */
extern void* th_module(void* arg);
extern void* vital_module(void* arg);
extern void* rule_module(void* arg);
extern void* db_module(void* arg);
extern void* send_module(void* arg);

#endif