#include "car.h"

int main(int argc, char * argv[]) {
    char *host;

    initializeCar ();

    // Verificacao de argumentos
    if (argc != 2) {
        // default: localhost
        host = "localhost";
    } else {
        // atribuicao do host como paramentro
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
    
    // start security layer
    setupSecurityLayer(host_address);
    
    return 0;
}

// APPLICATION
// ENTERTAINMENT

void *setupEntertainmentLayer(void *pointer) {
    
    while (1) {
        
        struct hostent *host_address = (struct hostent *) pointer;
        int sock;
        
        struct sockaddr_in socket_address;
        char server_reply[2000];
        
        printTitle("Starting Entertainment Layer...");
        
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1) {
            printTitle("Could not create socket");
            
            sleep(5);
            continue;
        }
        
        // copiando endereco para string
        struct in_addr a;
        while (*host_address->h_addr_list) {
            bcopy(*host_address->h_addr_list++, (char *) &a, sizeof(a));
        }
        
        socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(a));
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons( RADIO );
        
        // connect to remote server
        if (connect(sock , (struct sockaddr *)&socket_address , sizeof(socket_address)) < 0) {
            printTitle("Maybe Radio Station is off. Sorry!");
            
            sleep(5);
            continue;
        }
        
        print("Started Entertainment Layer!", NULL, socket_address);
        
        //keep communicating with server
        while(1) {
            
            //Receive a reply from the server
            if( recv(sock , server_reply , 2000 , 0) > 0) {
                printTitle("Entertainment Layer:");
                puts(server_reply);
            } else {
                printTitle("Stopped Entertainment Layer.");
                break;
            }
            
        }
        
        close(sock);
        
    }
    
    return 0;
}

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
