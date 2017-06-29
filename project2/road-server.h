#include "common.h"

#define MAX_PENDING 5
#define BACKLOG 10

#undef FD_SETSIZE
#define FD_SETSIZE 10

// STRUCTS

// DEFINITIONS
#define MAXCARS 100

// NETWORK FUNCTIONS
int setupServer ();
void newCarConnected (Car new_car);
void receivingMsgCar (Car car, int type);
void sendingMsgCar (Car car, int type, int order);

// ALGORITHM FUNCTIONS
void initiateMap ();
void calculateIntervals (map road, Car* cars);
void updateCarPosition (map road, Car c, float newpostion, float delay);
map readMap ();
void printMap (map road);
int setupServer();
