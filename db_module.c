#include "shared.h"
#include <unistd.h>

void* db_module(void* arg) {
    (void)arg;
    while(1) { sleep(10); }
    return NULL;
}