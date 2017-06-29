#include "road-server.h"

int main (int argc, char** argv) {
    setupServer();
    
//    map road = readMap();    
//    car* cars[MAXCARS];
//       
//    printMap (road); 

    return EXIT_SUCCESS;
}


// NETWORK FUNCTIONS

int setupServer() {
    struct sockaddr_in socket_address;
    unsigned int len;
    int s, new_s;
    
    Car car;
    
    int i, j;
    
    int sockfd, socks, cliente_num, maxfd, nready, clients[FD_SETSIZE];
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
                    
                    // TODO: Ajustar conforme necessidades.
                    receivingMsgCar(car, 0);
                    
                    // TODO: Voce pode manter um vetor de carros, e sempre que receber um message, atualiza na posicao correspondente (utilize variavel i para isso). Depois, calcular os conflitos e enviar mensagem para os carros acelerarem ou frearem, quando necessario
                    
                    for (j = 0; j <= cliente_num; j++) {
                        if ( i == j || (socks = clients[j] ) < 0) continue;
                        
                        Order order;
                        order.acceleration = 1;
                        
                        if (write(socks, &order, sizeof(Order)) < 0) {
                            printf("error: writting problem\n");
                        }
                    }
                }
                
                // não há mais descritores para ler
                if (--nready <= 0) break;
            }
        }
    }
    
    return 0;
}

// TODO: Funcao que eh chamada toda vez que um novo carro se conecta ao servidor
void newCarConnected (Car new_car) {
    // nao precisa colocar nada aqui, só chama a funcoa no momento certo
}


// TODO: Funcao que eh chamada toda vez que um carro manda uma msg de informacao para o servidor
void receivingMsgCar (Car car, int type) {
    // nao precisa colocar nada aqui, só chama a funcao no momento certo
}

// TODO: Eu chamo essa funcoa aqui, rlx
void sendingMsgCar (Car car, int type, int order) {
    // Ajustar para enviar msg do servidor para o cliente
}


// ALGORITHM FUNCTIONS
void calculateIntervals (map road) {
    for (int i = 0; i < MAXCARS; i++) {
        if (cars[i]->x_direction != 0) {
            cars[i]->time_in = (((float) road.x_cross) - cars[i]->position)/((float) cars[i]->speed);
            cars[i]->time_out = cars[i]->time_in + (((float) cars[i]->length)/((float) cars[i]->speed));
        }
        else {
            cars[i]->time_in = (((float) road.y_cross) - cars[i]->position)/((float) cars[i]->speed);
            cars[i]->time_out = cars[i]->time_in + (((float) cars[i]->length)/((float) cars[i]->speed));
        }
    }
}

void updateCarPosition (map road, Car* c, float newposition, float delay) {
    c->position = newposition + (2 * delay * ((float) c->speed));
}

map readMap () {
    map road;
    scanf ("%d %d %d %d", &road.x_size, &road.y_size, &road.x_cross, &road.y_cross);
    road.x = malloc (road.x_size * sizeof(int));
    road.y = malloc (road.y_size * sizeof(int));
    for (int i = 0; i < road.x_size; i++) road.x[i] = -1;
    for (int i = 0; i < road.y_size; i++) road.y[i] = -1;
    return road;
}

void printMap (map road) {
    int i, j;
    for (i = 0; i < road.x_size; i++) {
        for (j = 0; j < road.y_size; j++) {
            if (i == road.x_cross && j == road.y_cross) {
                if (road.x[i] != -1 && road.y[j] != -1) printf ("C");
                else if (road.x[j] != -1) printf ("*");
                else if (road.y[i] != -1) printf ("*");
                else printf ("X");
            }
            else if (i == road.x_cross) {
                if (road.y[i] != -1) printf ("*");
                else printf ("0");
            }
            else if (j == road.y_cross) {
                if (road.x[j] != -1) printf ("*");
                else printf ("0");
            }
            else printf (" ");
        }
        printf("\n");
    }
}
