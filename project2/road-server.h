#include "common.h"

#define MAX_PENDING 5
#define BACKLOG 10

#undef FD_SETSIZE
#define FD_SETSIZE 10

// DEFINITIONS

#define MAXCARS 100

// FUNCTIONS
map readMap ();
void printMap (map road);
int setupServer();
