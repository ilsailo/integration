#include "shared.h"

// 큐 및 동기화 도구 초기화
void q_init(TSQueue *q) {
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

// 데이터를 큐에 삽입 (생산자 역할)
void q_push(TSQueue *q, void* item) {
    pthread_mutex_lock(&q->lock); // 1. 큐 접근권 획득

    // 2. 큐가 꽉 찼다면 빈자리가 날 때까지 대기
    // Spurious Wakeup(허위 깨어남) 방지를 위해 while 루프 사용
    while (q->count == QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->lock); 
    }

    // 3. 데이터 삽입 및 인덱스 이동 (원형 큐 방식)
    q->data[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    // 4. "데이터가 들어왔음"을 대기 중인 소비자에게 알림
    pthread_cond_signal(&q->not_empty);

    pthread_mutex_unlock(&q->lock); // 5. 자물쇠 해제
}

// 큐에서 데이터를 추출 (소비자 역할)
void* q_pop(TSQueue *q) {
    pthread_mutex_lock(&q->lock);

    // 1. 큐가 비어있다면 데이터가 들어올 때까지 대기
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    // 2. 데이터 추출 및 인덱스 이동
    void* item = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    // 3. "빈자리가 생겼음"을 대기 중인 생산자에게 알림
    pthread_cond_signal(&q->not_full);

    pthread_mutex_unlock(&q->lock);
    return item; // 추출된 데이터 포인터 반환
}
