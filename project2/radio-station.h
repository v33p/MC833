#include "common.h"

#include<pthread.h>

#define BACKLOG 10
#define RADIO_BUFFER 5

typedef struct {
    int *clients;
    int *num_clients;
    struct sockaddr_in *socket_address;
} Handler;

// Functions
void *connection_handler(void *);
