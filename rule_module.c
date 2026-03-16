#include "shared.h"

void* rule_module(void* arg) {
    (void)arg; // 경고 방지
    while(1) {
        // q_th에서 데이터를 꺼내서 판단 후 q_db로 넘기는 로직 예시
        THData* data = (THData*)q_pop(&q_th);
        
        SensorPacket* packet = (SensorPacket*)malloc(sizeof(SensorPacket));
        packet->type = TYPE_EVENT;
        packet->payload.event.status_code = (data->temp > 40.0f) ? 100 : 0;
        
        if(packet->payload.event.status_code == 100) {
            strcpy(packet->payload.event.detail, "고온 위험 발생!");
        } else {
            strcpy(packet->payload.event.detail, "정상");
        }

        q_push(&q_db, packet);
        free(data); // 사용한 데이터 해제
    }
    return NULL;
}