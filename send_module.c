#include "shared.h"
#include <unistd.h>

void* send_module(void* arg) { // 파일명에 맞춰 함수명 변경 (예: th_module)
    (void)arg;
    while(1) { sleep(10); }
    return NULL;
}