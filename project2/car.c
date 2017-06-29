#include "car.h"
#include <time.h>

// APPLICATION
// CAR
int main(int argc, char * argv[]) {
    char *host;

    initializeCar ();

    // check if host was passed as an argument
    if (argc != 2) {
        // default host is localhost
        host = "localhost";
    } else {
        // if host was passed as an argument
        host = argv[1];
    }
    
    struct hostent *host_address = gethostbyname(host);
    if (!host_address) {
        printf("error: couldn't resolve name");
        exit(1);
    }
    
    // start entertainment layer
    pthread_t sniffer_thread;
    if( pthread_create( &sniffer_thread , NULL ,  setupEntertainmentLayer , (void*) host_address ) < 0) {
        perror("could not create thread");
        return 1;
    }
    
    // start weather layer
    pthread_t sniffer_thread2;
    if( pthread_create( &sniffer_thread2 , NULL ,  setupTrafficLayer , (void*) host_address ) < 0) {
        perror("could not create thread");
        return 1;
    }
    
    // start security layer
    setupSecurityLayer(host_address);
    
    return 0;
}

// LAYER
// TRAFFIC
void *setupTrafficLayer(void *pointer) {
    
    // socket variables
    struct hostent *host_address = (struct hostent *) pointer;
    struct sockaddr_in socket_address;
    
    int sock;
    unsigned int visibility;
    
    while (1) {
        
        printTitle("Starting Traffic Layer...");
        
        // create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1) {
            printTitle("Could not create socket");
            
            sleep(5);
            continue;
        }
        
        // setting server socket address
        struct in_addr a;
        while (*host_address->h_addr_list) {
            bcopy(*host_address->h_addr_list++, (char *) &a, sizeof(a));
        }
        
        socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(a));
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons( TRAFFIC );
        
        // connect to remote server
        if (connect(sock , (struct sockaddr *)&socket_address , sizeof(socket_address)) < 0) {
            printTitle("Maybe Traffic is off. Try again!");
            exit(0);
        }
        
        // keep seeing how is traffic conditions
        print("Started Traffic Layer!", NULL, socket_address);
        while(1) {
            
            if (write(sock, &sock, sizeof(int)) < 0) {
                printf ("error: writing problem\n");
            }
            
            if( recv(sock , &visibility , sizeof(visibility), 0) > 0) {
                
                if (MIN_VISIBILITY < visibility) {
                    printTitle("Traffic Layer:\nTraffic Conditions are good. Headlights are off.");
                } else {
                    printTitle("Traffic Layer:\nTraffic Conditions are bad. Headlights are on.");
                }
                
            } else {
                printTitle("Stopped Traffic Layer.");
                break;
            }
            
            sleep(2);
            
        }
        
        // radio was disconnected, let's try connecting again
        close(sock);
        
    }
    
    // never executed. entertainment layer will be running while application is running
    return 0;
}

// LAYER
// ENTERTAINMENT
void *setupEntertainmentLayer(void *pointer) {
    
    // buffer to print entertainment
    clock_t tic;
    clock_t toc;
    char buffer[ENTERTAINMENT_BUFFER];
    
    // socket variables
    struct hostent *host_address = (struct hostent *) pointer;
    struct sockaddr_in socket_address;
    
    int sock, socklen = sizeof(socket_address);
    char information[ENTERTAINMENT_INFORMATION];
    
    while (1) {
        
        printTitle("Starting Entertainment Layer...");
        
        // create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1) {
            printTitle("Could not create socket");
            
            sleep(5);
            continue;
        }
        
        // setting server socket address
        struct in_addr a;
        while (*host_address->h_addr_list) {
            bcopy(*host_address->h_addr_list++, (char *) &a, sizeof(a));
        }
        
        socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(a));
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons( RADIO );
        
        // connect to remote server
        if (connect(sock , (struct sockaddr *)&socket_address , socklen) < 0) {
            printTitle("Maybe Radio Station is off. Sorry!");
            
            sleep(5);
            continue;
        }
        
        // keep receiving information from radio station
        print("Started Entertainment Layer!", NULL, socket_address);
        bzero(buffer, sizeof(buffer));
        tic = clock();
        
        while(1) {
            
            if( recvfrom(sock, information, ENTERTAINMENT_INFORMATION, 0, (struct sockaddr *) &socket_address, (socklen_t*)&socklen) > 0) {
                // append information to buffer
                
                strcat(buffer, information);
                toc = clock();
                
                // if 1000 clocks has passed or buffer is almost full, then print
                if (toc - tic > ENTERTAINMENT_MAX_CLOCK || strlen(buffer) > 0.9 * ENTERTAINMENT_BUFFER) {
                    printTitle("Entertainment Layer:");
                    puts(buffer);
                    
                    bzero(buffer, sizeof(buffer));
                    tic = clock();
                }
                
                
            } else {
                printTitle("Stopped Entertainment Layer.");
                break;
            }
            
        }
        
        // radio was disconnected, let's try connecting again
        close(sock);
        
    }
    
    // never executed. entertainment layer will be running while application is running
    return 0;
}

// LAYER
// SECURITY
void setupSecurityLayer(struct hostent *host_address) {
    
    // Lock
    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed\n");
        exit(1);
    }
    
    Car car;
    Order order;
    
    pid_t pid;
    
    struct sockaddr_in socket_address;
    char buf[MAX_LINE];
    int len;
    
    // copiando endereco para string
    struct in_addr a;
    while (*host_address->h_addr_list) {
        bcopy(*host_address->h_addr_list++, (char *) &a, sizeof(a));
    }
    
    // criacao do socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error: socket problem\n");
        exit(1);
    }
    
    // criacao da estrutura de dados de endereco
    bzero((char *)&socket_address, sizeof(socket_address));
    
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(PORT);
    socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(a));
    
    // estabelecimento da conexao
    if (connect(s, (struct sockaddr *)&socket_address, sizeof socket_address) < 0) {
        printf("error: couldn't connect\n");
        close(s);
        exit(2);
    }
    
    print("Connected!", NULL, socket_address);
    
    // try fork
    if ((pid = fork()) < 0) {
        perror("error forking");
        exit(1);
    }
    
    if (pid == 0) {
        // Process to calculate my new position
        
        time_t start_time = time (NULL);
        while (1) {
            
            // LOOP DE MOVIMENTO
            
            time_t now = time(NULL);
            time_t diff = now - start_time;

            if (((diff * 10) % (int) (CLOCK * 10)) == 0) {
                driving(); 
            }

            if ((diff % UPDATE_TIMER) == 0) {
                update();
            }
            
            //sleep(1);
        }
        
    } else {
        // Process for waiting orders from server
         
        while (1) {
            // WAITING FOR ORDERS
            len = read (s, &order, sizeof(Order));
            receivingMsg (order); 
        }
        
    }
    
    printTitle("STOPPED");
    
    // fechando a sessao
    close(s);
    exit(0);
}

void connectedToServer () {
    Message m;
    m.type = 1;
    m.car = self;
    sendingMsg (m);
}

void desconnectFromServer () {
    Message m;
    m.type = -1;
    m.car = self;
    sendingMsg (m);
    exit (0);
}

void receivingMsg (Order o) {
    switch (o.type) {
        case -1:
            exit(0);
            break;
        default:
            adjustSpeed (o.speed);
            break;
    }
}

void sendingMsg (Message m) {
    if (write(s, &m, sizeof(Message)) < 0) {
        printf ("error: writing problem\n");
    }
}


// ALGORITHM
void initializeCar () {
    srand (time(NULL));

    self.speed = rand() % 7 + 1;
    self.length = rand() % 5 + 1;
    if (rand() % 2 == 0) {
        if (rand() % 2 == 0) {
            self.x_direction = 1;
            self.position = 0;
        }
        else {
            self.x_direction = -1;
            self.position = (float) road.x_size;
        }
    }
    else {
        if (rand() % 2 == 0) {
            self.y_direction = 1;
            self.position = 0;
        }
        else {
            self.y_direction = -1;
            self.position = (float) road.y_size;
        }
    }

}

void driving () {
    if (self.x_direction != 0) {
        self.position = self.position + (CLOCK * ((float) self.speed * self.x_direction));
        if (self.position - (float) road.x_cross < (float) self.length / 2 || 
                (float) road.x_cross - self.position < (float) self.length / 2) {
            // esta no cruzamento
            printf ("Crossing!\n"); 
        }
        if (self.position > (float) road.x_size || self.position < 0) desconnectFromServer ();
    }
    else {
        self.position = self.position + (CLOCK * ((float) self.speed * self.y_direction));
        if (self.position - (float) road.y_cross < (float) self.length / 2 || 
                (float) road.y_cross - self.position < (float) self.length / 2) {
            // esta no cruzamento
            printf ("Crossing!\n"); 
        }
        if (self.position > (float) road.y_size || self.position < 0) desconnectFromServer ();
    }
}

void adjustSpeed (int new_speed) {
    self.speed = new_speed;
}

void update () {
    Message m;
    m.car = self;
    m.type = 0;
    sendingMsg (m);
}
