#include <stdio.h>
#include <stdlib.h>

// include for extras
#include <time.h>

// includes for networking
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 10000
#define SPEED_LIMIT 7

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _b : _a; })
#define RADIO 10001

// STRUCTS

typedef struct {
    int speed;
    float position;
    int length;
    int x_direction;
    int y_direction;
    float time_in;
    float time_out;
    int index;
    int socket;
} Car;

typedef struct {
    int acceleration;
    int speed;
    int type;
} Order;

typedef struct {
    Car car;
    int type;
} Message;

struct map { 
    int x_size;
    int y_size;
    int x_cross;
    int y_cross;
};
typedef struct map map;

map road;

void sleep_ms(int milliseconds) {
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

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
        printf("Position    : %f\n", car->position);
        printf("Length      : %d\n", car->length);
        printf("X           : %d\n", car->x_direction);
        printf("Y           : %d\n", car->y_direction);
        printf("Time_in     : %f\n", car->time_in);
        printf("Time_out    : %f\n", car->time_out);
    }
}
