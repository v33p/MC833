#include "road-server.h"

int main (int argc, char** argv) {
    
    map road = readMap();    
    car* cars[MAXCARS];
       
    printMap (road); 

    return EXIT_SUCCESS;
}

void predictCross (map road, car* C) {
    
}

map readMap () {
    map road;
    scanf ("%d %d %d %d", &road.xsize, &road.ysize, &road.xcross, &road.ycross);
    road.x = malloc (road.xsize * sizeof(int));
    road.y = malloc (road.ysize * sizeof(int));
    for (int i = 0; i < road.xsize; i++) road.x[i] = -1;
    for (int i = 0; i < road.ysize; i++) road.y[i] = -1;
    return road;
}

void printMap (map road) {
    int i, j;
    for (i = 0; i < road.xsize; i++) {
        for (j = 0; j < road.ysize; j++) {
            if (i == road.xcross && j == road.ycross) {
                if (road.x[i] != -1 && road.y[j] != -1) printf ("C");
                else if (road.x[j] != -1) printf ("*");
                else if (road.y[i] != -1) printf ("*");
                else printf ("X");
            }
            else if (i == road.xcross) {
                if (road.y[i] != -1) printf ("*");
                else printf ("0");
            }
            else if (j == road.ycross) {
                if (road.x[j] != -1) printf ("*");
                else printf ("0");
            }
            else printf (" ");
        }
        printf("\n");
    }
}
