#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "shared.h"

#define NUM_MODULES 5

typedef struct {
    char *name;
    char *path;
    int is_python; // 파이썬 파일만 다르게 처리하기 위함
} Module;

// 모듈 목록
Module modules[NUM_MODULES] = {
    {"TH_Module", "./th_module", 0},     // 0번 C
    {"Vital_Module", "./vital_module", 0}, // 1번 C
    {"Rule_Module", "rule_module.py", 1}, // 2번 Py -> 3번 째 인자는 파이썬 파일인지 표시하기 위함
    {"DB_Module", "./db_module", 0},     // 3번 C
    {"Send_Module", "./send_module", 0}   // 4번 C
};

pid_t child_pids[NUM_MODULES]; // 자식 프로세스 죽었는지 확인하기 위한 배열

// 자식 프로세스 실행 함수
void run_module(int idx) { // modules[]의 몇 번째 모듈인지 인자로 받아옴
    if (modules[idx].is_python) { // 파이썬 파일이면
        execlp("python3", "python3", modules[idx].path, NULL);
    } else { // 그 외(C 파일)이면
        execl(modules[idx].path, modules[idx].path, NULL);
    }
    perror("모듈 실행 실패");
    exit(1);
}

int main() {
    printf("[Master] 산업 안전 모니터링 시스템 기동 (Jetson Orin NX)\n");

    // 1. 4개의 메시지 큐 초기화 (존재하면 열고, 없으면 생성)
    struct mq_attr raw_attr = { .mq_maxmsg = 10, .mq_msgsize = sizeof(RawData) }; // 일단 th, vital 메시지 큐 사이즈 통일
    struct mq_attr db_attr = { .mq_maxmsg = 10, .mq_msgsize = sizeof(DBData) }; // 일단 db, send 메시지 큐 사이즈 통일
    // mq_maxmsg는 최대 메시지 개수, mq_msgsize는 메시지 당 최대 바이트를 의미
    
    // 메시지 큐 열기
    mq_open(MQ_TH, O_CREAT | O_RDWR, 0666, &raw_attr);
    mq_open(MQ_VITAL, O_CREAT | O_RDWR, 0666, &raw_attr);
    mq_open(MQ_DB, O_CREAT | O_RDWR, 0666, &db_attr);
    mq_open(MQ_SEND, O_CREAT | O_RDWR, 0666, &db_attr);

    // 2. 5개 모듈 Fork 및 실행
    for (int i = 0; i < NUM_MODULES; i++) {
        pid_t pid = fork();
        if (pid == 0) run_module(i); // 자식 프로세스는 run_module() 진입
        else child_pids[i] = pid;
    }

    // 3. Watchdog: 자식 프로세스 생존 감시 및 자동 재시작
    while (1) {
        int status; // 상태 정의
        pid_t dead_pid = waitpid(-1, &status, 0); // 프로세스 종료 대기

        for (int i = 0; i < NUM_MODULES; i++) { // 종료된 자식 프로세스 찾기
            if (child_pids[i] == dead_pid) { // 대조해서 발견하면
                printf("[Master] 경고: %s(PID:%d) 종료됨. 재시작 중...\n", modules[i].name, dead_pid); // 어떤 프로세스가 종료됐는지 표시
                sleep(1); // 즉시 재시작 전 짧은 대기
                
                pid_t new_pid = fork();
                if (new_pid == 0) run_module(i); // 다시 생성된 자식 프로세스가 종료된 모듈 재실행 -> i번 째 모듈이 종료됐던 모듈일 것임
                else child_pids[i] = new_pid; // 새롭게 생성된 자식 프로세스의 pid 재기록
                break;
            }
        }
    }

    return 0;
}
