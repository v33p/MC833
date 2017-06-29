#include "weather.h"

int main(int argc , char *argv[]) {
    int message;
    
    srand( (unsigned) time(NULL) );
    int visibility = rand() % (VISIBILITY + 1);
    
    struct sockaddr_in socket_address;
    unsigned int len;
    int s, new_s;
    
    int i, j, sockfd, num_clients, maxfd, nready, clients[FD_SETSIZE];
    fd_set todos_fds, novo_set;
    
    // create socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error: socket problem\n");
        return 1;
    }
    
    // create socket address
    bzero((char *)&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port   = htons(TRAFFIC);
    
    if (INADDR_ANY)
        socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(s, (struct sockaddr *)&socket_address, sizeof socket_address) < 0) {
        perror("error: bind problem\n");
        return 2;
    }
    
    // listen
    listen(s, BACKLOG);
    
    maxfd = s;
    num_clients = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        clients[i] = -1;
    
    FD_ZERO(&todos_fds);
    FD_SET(s, &todos_fds);
    
    
    // thread to set weather
    pthread_t sniffer_thread;
    if( pthread_create( &sniffer_thread , NULL ,  traffic_handler , (void*) &visibility ) < 0) {
        perror("could not create thread");
        return 1;
    }
    
    // wait for new connections
    socklen_t b;
    while (1) {
        novo_set = todos_fds;
        
        printTitle("Waiting for new connection...");
        nready = select(maxfd+1, &novo_set, NULL, NULL, NULL);
        
        if(nready < 0) {
            perror("select" );
            exit(1);
        }
        
        if(FD_ISSET(s, &novo_set)) {
            b = sizeof(socket_address);
            
            if ((new_s = accept(s, (struct sockaddr *)&socket_address, &len)) < 0) {
                perror("simplex-talk: accept");
                exit(1);
            }
            
            // save descriptor
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clients[i] < 0) {
                    clients[i] = new_s;
                    break;
                }
            }
            
            if (i == FD_SETSIZE) {
                perror("Numero maximo de clientes atingido.");
                exit(1);
            }
            
            // add new descriptor to set
            print("Accepted new connection!", NULL, socket_address);
            FD_SET(new_s, &todos_fds);
            
            // select
            if (new_s > maxfd) maxfd = new_s;
            
            // max index
            if (i > num_clients) num_clients = i;
            
            // there are no more descriptors to read
            if (--nready <= 0) continue;
        }
        
        for (i = 0; i <= num_clients; i++) {
            // is there a connection on position i?
            if ( (sockfd = clients[i] ) < 0) continue;
            
            if (FD_ISSET(sockfd, &novo_set)) {
                if ( (len = recv(sockfd, &message, sizeof(int), 0)) == 0) {
                    // connection closed
                    print("Client terminated!", NULL, socket_address);
                    close(sockfd);
                    FD_CLR(sockfd, &todos_fds);
                    clients[i] = -1;
                } else {
                    print("Driver looked at me.", NULL, socket_address);
                    
                    if (write(sockfd, &visibility, sizeof(visibility)) < 0) {
                        printf ("error: writing problem\n");
                    }
                }
                
                // there are no more descriptors to read
                if (--nready <= 0) break;
            }
        }
        
    }
    
    return 0;
}

void *weather_handler(void *pointer) {
    int *visibility = (int*) pointer;
    
    while (1) {
        if (rand() % 2) {
            // visibility are better
            *visibility = min((*visibility + 1), VISIBILITY);
        } else {
            // visibility are worse
            *visibility = max((*visibility - 1), 0);
        }
        printTitle("Weather Condition:");
        printf("( 0 <-> 10 ) %d\n", *visibility);
        
        // wait (miliseconds) to calculate new conditions
        sleep_ms(1000);
    }
    
    return 0;
}
