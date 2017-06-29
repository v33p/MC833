#include "common.h"

#define MAX_PENDING 5
#define BACKLOG 10

#undef FD_SETSIZE
#define FD_SETSIZE 10

// DEFINITIONS
#define MAXCARS 100

// STRUCTS
Car* cars;
int n_cars = 0;
map road;

// NETWORK FUNCTIONS
int setupServer ();
void newCarConnected (Car new_car);
void receivingMsgCar (Car car, int type);
void sendingMsgCar (Car car, int type, int order);

// ALGORITHM FUNCTIONS
void initiateMap ();
void algorithmFIFO ();
int calculateSpeedToFit (Car c, float out);
void updateCarIntervalsBySpeed (Car* c, int new_speed);
void updateCarSpeed (Car* c, int new_speed);
void updateCarPosition (map road, Car* c, float newpostion, float delay);
void updateCarIntervals (Car* c);
void updateAllCarsIntervals ();

// AUX
int compareTimeIn (const void * a, const void * b);
void increaseCars ();
void decreaseCars (int exclude);
