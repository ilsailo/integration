#include "shared.h"
#include <assert.h>

extern void* rule_module(void* arg);
TSQueue q_th, q_db, q_vital, q_send; // 필요한 큐 정의

int main() {
    printf("[Test] Rule Module 로직 검증 시작...\n");
    q_init(&q_th); q_init(&q_db); q_init(&q_vital); q_init(&q_send);

    // 1. 정상 데이터 주입
    THData* normal_data = malloc(sizeof(THData));
    normal_data->temp = 25.0f;
    normal_data->wp_id = 1;
    q_push(&q_th, normal_data);

    // 2. 모듈 실행
    pthread_t tid;
    pthread_create(&tid, NULL, rule_module, NULL);

    // 3. 결과 확인
    SensorPacket* result = (SensorPacket*)q_pop(&q_db);
    printf("[Check] 정상 온도 테스트: Status %d\n", result->payload.event.status_code);
    free(result);

    // 4. 위험 데이터 주입
    THData* danger_data = malloc(sizeof(THData));
    danger_data->temp = 50.0f;
    q_push(&q_th, danger_data);

    result = (SensorPacket*)q_pop(&q_db);
    printf("[Check] 고온 위험 테스트: Status %d (%s)\n", 
            result->payload.event.status_code, result->payload.event.detail);
    free(result);

    printf("[Test] Rule Module 로직 검증 완료!\n");
    return 0;
}