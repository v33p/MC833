#include <stdio.h>
#include <stdlib.h>

// DEFINITIONS

#define MAXCARS 100 

// STRUCTS

struct car {
    int speed;
    float position;
    int xdirection;
    int ydirection;
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
void calculateIntervals (map road, car* cars);
void updateCarPosition (map road, car c, newpostion, delay);
void updateMapPosition (map road, car* cars);
map readMap ();
void printMap (map road);
