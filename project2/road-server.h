#include <stdio.h>
#include <stdlib.h>

// DEFINITIONS

#define MAXCARS 100 

// STRUCTS

struct car {
    int speed;
    int position;
    int x_direction;
    int y_direction;
    float time_in;
    float time_out;
};
typedef struct car car;

struct map { 
    int xsize;
    int ysize;
    int xcross;
    int ycross;
    int* x;
    int* y;
};
typedef struct map map;

// FUNCTIONS
map readMap ();
void printMap (map road);
