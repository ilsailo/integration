#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "cJSON.h" // JSON 파싱용
#include "integrated_types.h"

SystemState g_state;
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// [외부 파일 호출 함수] fork/exec를 통한 멀티 프로세싱 구현
void call_external_file(const char* file_name) {
    if (fork() == 0) {
        printf("🚀 [Master] 외부 파일 호출 중: %s\n", file_name);
        execlp("python3", "python3", file_name, NULL);
        perror("파일 실행 실패");
        exit(1);
    }
}

// ---------------------------------------------------------
// [모듈 1 & 2 데이터 수집부] FIFO 리더 스레드
// ---------------------------------------------------------
void* thread_collector_sync(void* arg) {
    const char* FIFO_PATH = "/tmp/th_fifo"; //
    mkfifo(FIFO_PATH, 0666);
    FILE* fp = fopen(FIFO_PATH, "r");
    char line[2048];

    while (fgets(line, sizeof(line), fp)) {
        cJSON *json = cJSON_Parse(line);
        if (!json) continue;

        pthread_mutex_lock(&state_mutex);
        // JSON 필드 동기화
        cJSON *t = cJSON_GetObjectItem(json, "temperature");
        if (t) g_state.temperature = t->valuedouble;
        cJSON *hr = cJSON_GetObjectItem(json, "heartRate");
        if (hr) g_state.heartRate = hr->valuedouble;

        clock_gettime(CLOCK_MONOTONIC, &g_state.last_sync);
        pthread_mutex_unlock(&state_mutex);
        cJSON_Delete(json);
    }
    return NULL;
}

// ---------------------------------------------------------
// [모듈 3] 룰 베이스 판단 모듈 (Internal Thread)
// ---------------------------------------------------------
void* thread_rule_engine(void* arg) {
    while (1) {
        pthread_mutex_lock(&state_mutex);
        // 판단 로직: 센서/워치 최신 데이터 기반
        if (g_state.heartRate > 120.0 || g_state.temperature > 35.0) {
            g_state.emergency_detected = true;
        } else {
            g_state.emergency_detected = false;
        }
        pthread_mutex_unlock(&state_mutex);
        usleep(500000); // 0.5초 주기
    }
}

// ---------------------------------------------------------
// [모듈 4] 내외부 DB 적재 모듈 (Internal Thread)
// ---------------------------------------------------------
void* thread_db_manager(void* arg) {
    while (1) {
        pthread_mutex_lock(&state_mutex);
        // MySQL Wire Protocol 기반 적재 로직 위치
        printf("💾 [DB] Data Logged: Temp(%.1f), HR(%.1f)\n", 
               g_state.temperature, g_state.heartRate);
        pthread_mutex_unlock(&state_mutex);
        sleep(5); // 5초 주기 저장
    }
}

// ---------------------------------------------------------
// [모듈 5] 판단 정보 전송 모듈 (Internal Thread)
// ---------------------------------------------------------
void* thread_sender(void* arg) {
    while (1) {
        pthread_mutex_lock(&state_mutex);
        if (g_state.emergency_detected) {
            // 워치 등으로 TCP 전송 수행
            printf("📲 [Watch] ALERT! Emergency Status Sent.\n");
        }
        pthread_mutex_unlock(&state_mutex);
        sleep(1);
    }
}

int main() {
    printf("🏗️ [Master] Jetson Orin NX 통합 시스템 기동\n");

    // [Step 1] 외부 파일 호출 (모듈 1, 2)
    call_external_file("./sensor_node.py");  //
    call_external_file("./watch_bridge.py"); //

    // [Step 2] 내부 스레드 호출 (모듈 3, 4, 5 + 동기화 리더)
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, thread_collector_sync, NULL); // 데이터 수신/동기화
    pthread_create(&t2, NULL, thread_rule_engine, NULL);    // 모듈 3
    pthread_create(&t3, NULL, thread_db_manager, NULL);     // 모듈 4
    pthread_create(&t4, NULL, thread_sender, NULL);         // 모듈 5

    pthread_join(t1, NULL);
    return 0;
}