#include "radio-station.h"

#include <sys/socket.h>

int main(int argc , char *argv[]) {
    int message;
    
    
    struct sockaddr_in socket_address;
    unsigned int len;
    int s, new_s;
    
    int i, j, sockfd, num_clients, maxfd, nready, clients[FD_SETSIZE];
    fd_set todos_fds, novo_set;
    
    Handler handler;
    handler.num_clients = &num_clients;
    handler.clients = clients;
    
    
    // criacao de socket passivo
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error: socket problem\n");
        return 1;
    }
    
    // criacao da estrutura de dados de endereco
    bzero((char *)&socket_address, sizeof(socket_address));
    
    // associar socket ao descritor
    socket_address.sin_family = AF_INET;
    socket_address.sin_port   = htons(RADIO);
    
    if (INADDR_ANY)
        socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(s, (struct sockaddr *)&socket_address, sizeof socket_address) < 0) {
        perror("error: bind problem\n");
        return 2;
    }
    
    // criando escuta
    listen(s, BACKLOG);
    
    maxfd = s;
    num_clients = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        clients[i] = -1;
    
    FD_ZERO(&todos_fds);
    FD_SET(s, &todos_fds);
    
    // send message to all clients
    pthread_t sniffer_thread;
    if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) &handler ) < 0) {
        perror("could not create thread");
        return 1;
    }
    
    // aguardando e aceitando conexao
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
            
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clients[i] < 0) {
                    clients[i] = new_s; 	//guarda descritor
                    break;
                }
            }
            
            if (i == FD_SETSIZE) {
                perror("Numero maximo de clientes atingido.");
                exit(1);
            }
            
            // adiciona novo descritor ao conjunto
            print("Accepted new connection!", NULL, socket_address);
            FD_SET(new_s, &todos_fds);
            
            // para o select
            if (new_s > maxfd) maxfd = new_s;
            
            // índice máximo no vetor clientes[]
            if (i > num_clients) num_clients = i;
            
            // não existem mais descritores para serem lidos
            if (--nready <= 0) continue;
        }
        
        for (i = 0; i <= num_clients; i++) {
            // existe conexão na posição i?
            if ( (sockfd = clients[i] ) < 0) continue;
            
            if (FD_ISSET(sockfd, &novo_set)) {
                if ( (len = recv(sockfd, &message, sizeof(int), 0)) == 0) {
                    //conexão encerrada pelo cliente
                    print("Client terminated!", NULL, socket_address);
                    close(sockfd);
                    FD_CLR(sockfd, &todos_fds);
                    clients[i] = -1;
                }
                
                // não há mais descritores para ler
                if (--nready <= 0) break;
            }
        }
        
    }
    
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *pointer) {
    Handler *handler = (Handler*) pointer;
    char *message;
    
    int i;
    int socket;
    
    while (1) {
        for (i = 0; i <= *(handler->num_clients); i++) {
            if ( (socket = (handler->clients)[i] ) < 0) continue;
            
            message = "Now type something and i shall repeat what you type \n";
            write(socket , message , strlen(message));
        }
        
        sleep(1);
    }
    
    return 0;
}
