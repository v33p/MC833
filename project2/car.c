#include "car.h"

int main(int argc, char * argv[]) {
    char *host;
    
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
    int s;
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
        
        while (1) {
            // TODO: MUST CALCULATE CAR NEW POSITION
            adjustPosition();
            car.speed       = 10;
            car.position    = 0;
            car.length      = 1;
            car.x_direction = 0;
            car.y_direction = 0;
            car.time_in     = 0;
            car.time_out    = 0;
            
            print("Sending params:", &car, socket_address);
            
            if (write(s, &car, sizeof(Car)) < 0) {
                printf("error: writing problem\n");
            }
            
            sleep(1);
        }
        
    } else {
        // Process for waiting orders from server
        
        while (1) {
            
            printTitle("Waiting for orders...");
            // TODO MUST BE DEFINED HOW ORDERS WILL BE MADE
            if ( (len = read(s, &order, sizeof(Order))) == 0) {
                // disconnected
                printTitle("Disconnected!");
                exit(0);
            } else {
                // atualize velocidade do carro, de acordo com instrucoes recebidas
                printTitle("Adjusting speed!");
                adjustSpeed(&car, order);
            }
            
        }
        
    }
    
    printTitle("STOPPED");
    
    // fechando a sessao
    close(s);
    exit(0);
}

void adjustSpeed(Car *car, Order order) {
    switch (order.acceleration) {
        case 1:
            printf("must accelerate\n");
            break;
        case -1:
            printf("must brake\n");
            break;
        default:
            printf("do nothing\n");
            break;
    }
}

void adjustPosition(Car *car) {
    
}

