#include <stdio.h>
#include <stdlib.h>

// includes for networking
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 10000

// STRUCTS

typedef struct {
    int speed;
    float position;
    int length;
    int x_direction;
    int y_direction;
    float time_in;
    float time_out;
} Car;

typedef struct {
    int acceleration;
} Order;

struct map { 
    int x_size;
    int y_size;
    int x_cross;
    int y_cross;
    int* x;
    int* y;
};
typedef struct map map;


void printTitle(char *title) {
    printf("-----------------------------------\n");
    printf("%s\n", title);
}

void print(char *title, Car *car, struct sockaddr_in socket_address) {
    printTitle(title);
    printf("at address  : %d.%d.%d.%d\n",
           socket_address.sin_addr.s_addr&0xFF,
           (socket_address.sin_addr.s_addr&0xFF00)>>8,
           (socket_address.sin_addr.s_addr&0xFF0000)>>16,
           (socket_address.sin_addr.s_addr&0xFF000000)>>24);
    printf("at port     : %d\n", socket_address.sin_port);
    if (car != NULL) {
        printf("\nCar info:\n");
        
        printf("Speed       : %d\n", car->speed);
        printf("Position    : %d\n", car->position);
        printf("Length      : %d\n", car->length);
        printf("X           : %d\n", car->x_direction);
        printf("Y           : %d\n", car->y_direction);
        printf("Time_in     : %f\n", car->time_in);
        printf("Time_out    : %f\n", car->time_out);
    }
}
