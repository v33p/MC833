#include "common.h"

#define MAX_PENDING 5
#define BACKLOG 10

#undef FD_SETSIZE
#define FD_SETSIZE 10

// DEFINITIONS
#define DELAY 10.0
#define MAXCARS 100

// STRUCTS
Car* cars;
int n_cars = 0;
int current_index = 0;

// NETWORK FUNCTIONS
int setupServer ();
void newCarConnected (int socket, Car new_car);
void clientDesconnected (int socket);
void carDesconnected (Car car);
void receivingMsgCar (int socket, Car car, int type);
void sendingMsgCar (Car car, Order order);

// ALGORITHM FUNCTIONS
void initiateMap ();
void algorithmFIFO ();
int calculateSpeedToFit (Car c, float out);
void updateCarIntervalsBySpeed (Car* c, int new_speed);
void updateCarSpeed (Car* c, int new_speed);
void updateCarIntervals (Car* c);
void updateAllCarsIntervals ();

// AUX
int compareTimeIn (const void * a, const void * b);
void increaseCars ();
void decreaseCars (int exclude);
