#include "road-server.h"

int main (int argc, char** argv) {
    setupServer();
    
//    map road = readMap();    
//    car* cars[MAXCARS];
//       
//    printMap (road); 

    return EXIT_SUCCESS;
}

void calculateIntervals (map road, car* cars) {
    for (int i = 0; i < MAXCARS; i++) {
        if (cars[i].xdirection != 0) {
            cars[i].time_in = float(road.xcross - cars[i].position)/float(cars[i].speed);
            cars[i].time_out = cars[i].time_in + (1.0/float(cars[i].speed));
        }
        else {
            cars[i].time_in = float(road.ycross - cars[i].position)/float(cars[i].speed);
            cars[i].time_out = cars[i].time_in + (1.0/float(cars[i].speed));
        }
    }
}

void updateCarPosition (map road, car c, newposition, delay) {
    c.position = newposition + (2 * delay * float(c.speed));
}

void updateMapPosition (map road, car* cars) {
   for (int i = 0; i < MAXCARS; i++) {
       if (cars[i].xdirection != 0) {
            
       }
       else {
            
       }
   }
}

int setupServer() {
    
    struct sockaddr_in socket_address;
    unsigned int len;
    int s, new_s;
    
    Car car;
    
    int i, j;
    
    int sockfd, cliente_num, maxfd, nready, clients[FD_SETSIZE];
    fd_set todos_fds, novo_set;
    
    // criacao de socket passivo
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error: socket problem\n");
        return 1;
    }
    
    // criacao da estrutura de dados de endereco
    bzero((char *)&socket_address, sizeof(socket_address));
    
    // associar socket ao descritor
    socket_address.sin_family = AF_INET;
    socket_address.sin_port   = htons(PORT);
    
    if (INADDR_ANY)
        socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(s, (struct sockaddr *)&socket_address, sizeof socket_address) < 0) {
        perror("error: bind problem\n");
        return 2;
    }
    
    // criando escuta
    listen(s, BACKLOG);
    
    maxfd = s;
    cliente_num = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        clients[i] = -1;
    
    FD_ZERO(&todos_fds);
    FD_SET(s, &todos_fds);
    
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
            if (i > cliente_num) cliente_num = i;
            
            // não existem mais descritores para serem lidos
            if (--nready <= 0) continue;
        }
        
        char ipstr[INET6_ADDRSTRLEN];
        int port;
        // verificar se há dados em todos os clientes
        for (i = 0; i <= cliente_num; i++) {
            
            // existe conexão na posição i?
            if ( (sockfd = clients[i] ) < 0) continue;
            
            if (FD_ISSET(sockfd, &novo_set)) {
                if ( (len = recv(sockfd, &car, sizeof(Car), 0)) == 0) {
                    //conexão encerrada pelo cliente
                    print("Client terminated!", NULL, socket_address);
                    close(sockfd);
                    FD_CLR(sockfd, &todos_fds);
                    clients[i] = -1;
                } else {
                    /* imprime ip e porta do cliente, e as informacoes do carro */
                    print("Received new message!", &car, socket_address);
                    
                    
                    // TODO: Voce pode manter um vetor de carros, e sempre que receber um message, atualiza na posicao correspondente (utilize variavel i para isso). Depois, calcular os conflitos e enviar mensagem para os carros acelerarem ou frearem, quando necessario
                    
                    for (j = 0; j <= cliente_num; j++) {
                        if ( (sockfd = clients[j] ) < 0) continue;
                        
                        printf("andasjkdnsadnjsal\n");
                    }
                }
                
                // não há mais descritores para ler
                if (--nready <= 0) break;
            }
        }
    }
    
    return 0;
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
