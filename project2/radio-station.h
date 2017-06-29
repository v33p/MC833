#include "common.h"

#include<pthread.h>

#define BACKLOG 10

typedef struct {
    int *clients;
    int *num_clients;
} Handler;

// Functions
void *connection_handler(void *);
