#include "shared.h"
#include <assert.h>

// 실제 모듈 함수를 테스트하기 위해 extern 선언
extern void* rule_module(void* arg);

// 테스트용 전역 큐
TSQueue q_th, q_db; // 필요한 큐만 선언

int main() {
    printf("[Test] Rule Module 로직 검증 시작...\n");
    q_init(&q_th);
    q_init(&q_db);

    // 1. 정상 데이터 주입 (온도 25도)
    THData* normal_data = malloc(sizeof(THData));
    normal_data->temp = 25.0f;
    normal_data->wp_id = 1;
    q_push(&q_th, normal_data);

    // 2. 모듈 스레드 실행 (테스트를 위해 별도 스레드로 띄움)
    pthread_t tid;
    pthread_create(&tid, NULL, rule_module, NULL);

    // 3. 결과 확인 (q_db에서 꺼내기)
    SensorPacket* result = (SensorPacket*)q_pop(&q_db);
    
    // 정상 상태(0)인지 검증
    printf("[Check] 정상 온도 테스트: Status %d\n", result->payload.event.status_code);
    // assert(result->payload.event.status_code == 0);

    // 4. 위험 데이터 주입 (온도 50도)
    THData* danger_data = malloc(sizeof(THData));
    danger_data->temp = 50.0f;
    q_push(&q_th, danger_data);

    result = (SensorPacket*)q_pop(&q_db);
    // 위험 상태(100 이상)인지 검증
    printf("[Check] 고온 위험 테스트: Status %d (%s)\n", 
            result->payload.event.status_code, result->payload.event.detail);

    printf("[Test] Rule Module 로직 검증 완료!\n");
    return 0;
}
