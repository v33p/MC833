#include "common.h"
#include <pthread.h>

#define MAX_LINE 10
#define CLOCK 0.1
#define UPDATE_TIMER 2

Car self;
int s;

// ENTERTAINMENT
void *setupEntertainmentLayer(void *pointer);

// SECURITY
// NETWORK
void setupSecurityLayer(struct hostent *host_address);
void connectedToServer ();
void desconnectFromServer ();
void receivingMsg (Order o);
void sendingMsg (Message m);

// ALGORITIHM
void initializeCar ();
void driving ();
void adjustSpeed (int new_speed);
void update ();
