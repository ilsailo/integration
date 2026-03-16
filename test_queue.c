#include "shared.h"
#include <assert.h>

TSQueue test_q;

void* producer_test(void* arg) {
    (void)arg; 
    for (int i = 0; i < 100; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        q_push(&test_q, val);
    }
    return NULL;
}

void* consumer_test(void* arg) {
    (void)arg;
    for (int i = 0; i < 100; i++) {
        int* val = (int*)q_pop(&test_q);
        assert(*val >= 0 && *val < 100); 
        free(val);
    }
    return NULL;
}

int main() {
    printf("[Test] 큐 동기화 테스트 시작...\n");
    q_init(&test_q);
    pthread_t p, c;
    pthread_create(&p, NULL, producer_test, NULL);
    pthread_create(&c, NULL, consumer_test, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    printf("[Test] 큐 동기화 테스트 성공!\n");
    return 0;
}