#include "common.h"

#define MAX_PENDING 5
#define BACKLOG 10

#undef FD_SETSIZE
#define FD_SETSIZE 10

// STRUCTS

// DEFINITIONS
#define MAXCARS 100

// FUNCTIONS
void calculateIntervals (map road, car* cars);
void updateCarPosition (map road, car c, newpostion, delay);
void updateMapPosition (map road, car* cars);
map readMap ();
void printMap (map road);
int setupServer();
