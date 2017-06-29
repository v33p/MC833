#include "common.h"
#include <pthread.h>

#define MAX_LINE 10
#define CLOCK 0.1
#define UPDATE_TIMER 2

// ENTERTAINMENT APPLICATION
#define ENTERTAINMENT_MAX_CLOCK 1000
#define ENTERTAINMENT_BUFFER 2000
#define ENTERTAINMENT_INFORMATION 50

Car self;
int s;

// ENTERTAINMENT
void *setupEntertainmentLayer(void *pointer);

// TRAFFIC
void *setupTrafficLayer(void *pointer);

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
void endOfTheRoad ();
void crossingRoad ();
void adjustSpeed (int new_speed);
void update ();

