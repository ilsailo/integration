#ifndef INTEGRATED_TYPES_H
#define INTEGRATED_TYPES_H

#include <time.h>
#include <stdbool.h>
bo
// [Shared State] 모든 모듈이 참조하는 최신 시스템 상태
typedef struct {
    double temperature;      // 모듈 1(센서) 데이터
    double humidity;         // 모듈 1(센서) 데이터
    double heartRate;        // 모듈 2(워치) 데이터
    double skin_temperature; // 모듈 2(워치) 데이터
    
    struct timespec last_sync; // 마지막 동기화 시간
    bool emergency_detected;   // 모듈 3(판단) 결과
} SystemState;

#endif