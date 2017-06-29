#include "common.h"
#include <pthread.h>

#define MAX_LINE 10


// Functions
void setupSecurityLayer(struct hostent *host_address);
void adjustSpeed(Car *car, Order order);
void adjustPosition();

void *setupEntertainmentLayer(void *pointer);
