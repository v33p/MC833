#include "road-server.h"

int main (int argc, char** argv) {
    initiateMap();
    setupServer();

    return EXIT_SUCCESS;
}

// NETWORK FUNCTIONS

int setupServer() {
    struct sockaddr_in socket_address;
    unsigned int len;
    int s, new_s;
    
    Car car;
    Message m;

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
                if ( (len = recv(sockfd, &m, sizeof(Message), 0)) == 0) {
                    //conexão encerrada pelo cliente
                    print("Client terminated!", NULL, socket_address);
                    close(sockfd);
                    FD_CLR(sockfd, &todos_fds);
                    clientDesconnected (clients[i]); 
                    clients[i] = -1;
                } else {
                    /* imprime ip e porta do cliente, e as informacoes do carro */
                    print("Received new message!", &m.car, socket_address);

                    receivingMsgCar(clients[i], m);
                }
                
                // não há mais descritores para ler
                if (--nready <= 0) break;
            }
        }
    }
    
    return 0;
}

// newCarConnected (int socket, Car new_car): Eh chamada toda vez que uma mensagem do tipo
// cadastro chega para o servidor, entao o servidor cadastra o carro.
void newCarConnected (int socket, Car new_car) {
    printTitle ("New Car Connected");
    increaseCars ();
    cars[n_cars-1] = new_car;
    cars[n_cars-1].index = current_index++;
    cars[n_cars-1].socket = socket;
}

// clientDesconnected (int socket): Quando um cliente eh desconectado, o servidor procura
// o carro dele e desconecta o carro.
void clientDesconnected (int socket) {
    printTitle ("Client Desconnected");
    for (int i = 0; i < n_cars; i++)
        if (cars[i].socket == socket) carDesconnected (cars[i]);
}

// carDesconnected (Car car): Retira o carro do cadastro de carros.
void carDesconnected (Car car) {
    printTitle ("Car Desconnected");
    decreaseCars (car.index);
}

// receivingMsgCar (int socket, Message m): Eh chamada toda vez que o server recebe uma 
// mensagem de um socket, nela o server filtra pelo tipo e executa as devidas tarefas.
void receivingMsgCar (int socket, Message m) {
    switch (m.type) {
        case 1:
            newCarConnected (socket, m.car);
            break;
        case -1:
            carDesconnected (m.car); 
            break;
        default:
            algorithmFIFO ();
            break;
    }
}

// sendingMsgCar (Car car, Order order): Essa funcao envia uma mensagem para o carro
// car com o valor de ordem
void sendingMsgCar (Car car, Order order) {
    if (write (car.socket, &order, sizeof(Order))) {
        printf ("error: writting problem\n");
    }
}

//
// ALGORITHM FUNCTIONS
//

// algorithmFIFO (): Este eh o kernel do problema de controle de cruzamento,
// a ideia do algoritmo eh uma solucao gulosa nao-otima. O server ordena os carros
// por ordem de previsao de chegada no cruzamento e o primeiro que entra sera
// o primeiro que vai sair. Se ocorrer algum conflito, o que primeiro chegou no
// cruzamento tem preferencia.
void algorithmFIFO () {
    updateAllCarsIntervals ();
    printCars ();
    qsort (cars, n_cars, sizeof(Car), compareTimeIn);
    
    float current_out_x = -1;
    float current_out_y = -1;
    for (int i = 0; i < n_cars; i++) {
        int speed;
        int old_speed = cars[i].speed;
        if (cars[i].x_direction != 0) {
            // CARRO EM X
            speed = calculateSpeedToFit (cars[i], current_out_y);
            updateCarIntervalsBySpeed (&cars[i], speed);
            updateCarSpeed (&cars[i], speed);
            current_out_x = max(current_out_x, cars[i].time_out);
        }
        else {
            // CARRO EM Y
            speed = calculateSpeedToFit (cars[i], current_out_x);
            updateCarIntervalsBySpeed (&cars[i], speed);
            updateCarSpeed (&cars[i], speed);
            current_out_y = max(current_out_y, cars[i].time_out);
        }
        Order o;
        if (old_speed < speed) {
            o.acceleration = 1;
        }
        else if (old_speed == speed) {
            o.acceleration = 0;
        }
        else {
            o.acceleration = -1;
        }
        o.speed = speed;
        o.type = 0;
        sendingMsgCar (cars[i], o);
    }
}

// calculateSpeedToFit (Car c, float out): Essa funcao calcula a velocidade
// necessaria do carro para ele nao conflitar com o ultimo carro que esta 
// passando no cruzamento.
int calculateSpeedToFit (Car c, float out) {
    if (out == -1) {
        return SPEED_LIMIT;
    }
    else {
        float position = c.time_in * c.speed;
        return position / out;
    }
}

// updateCarSpeed (Car *c, int new_speed): Essa funcao atualiza o valor de 
// velocidade de um carro.
void updateCarSpeed (Car* c, int new_speed) {
    c->speed = new_speed; 
}

// updateCarIntervalsBySpeed (Car* c, int new_speed): Essa funcao atualiza
// os intervalos de entrada e saida do cruzamento de um carro baseado numa 
// nova velocidade.
void updateCarIntervalsBySpeed (Car* c, int new_speed) {
    float position = c->time_in * c->speed;
    c->time_in = position / ((float) new_speed);
    c->time_out = c->time_in + ((float) c->length) / ((float) new_speed);
}

// updateCarIntervals (Car* c): Essa funcao atualiza os intervalos de entrada
// e de saida do cruzamento de um carro.
void updateCarIntervals (Car* c) {
    if (c->x_direction != 0) {
        c->time_in = (((float) road.x_cross) - (DELAY * c->position)) / ((float) (c->speed * c->x_direction));
        c->time_out = c->time_in + (((float) c->length) / ((float) c->speed));
    }
    else {
        c->time_in = (((float) road.y_cross) - (DELAY * c->position)) / ((float) (c->speed * c->y_direction));
        c->time_out = c->time_in + (((float) c->length) / ((float) c->speed));
    }
}

// updateAllCarsIntervals (): Essa funcao atualiza os intervalos de todos os
// carros.
void updateAllCarsIntervals () {
    for (int i = 0; i < n_cars; i++) {
        updateCarIntervals (&cars[i]);
    }
}

// AUX
// compareTimeIn (const void * a, const void * b): Funcao auxiliar para o 
// quicksort.
int compareTimeIn (const void * a, const void * b) {
    Car *carA = (Car *)a;
    Car *carB = (Car *)b;

    if (carA->time_in < carB->time_in) return -1;
    else if (carA->time_in == carB->time_in) return 0;
    else return 1;
}

// increaseCars (): Aumenta o tamanho do vetor de carros em 1.
void increaseCars () {
    Car* new = malloc (n_cars+1 * sizeof (Car));
    for (int i = 0; i < n_cars; i++) {
        new[i] = cars[i];
    }
    free (cars);
    cars = new;
    n_cars++;
    printCars ();
}

// decreaseCars (int exclude): Decresce o tamanho do vetor de carros em 1,
// excluindo o carro com index exclude.
void decreaseCars (int exclude) {
    Car* new = malloc (n_cars-1 * sizeof (Car));
    int found = 0;
    for (int i = 0; i < n_cars; i++) {
        if (cars[i].index == exclude) {
            found = 1;
        }
        else {
            if (found == 0) {
                new[i] = cars[i];
            }
            else {
                new[i-1] = cars[i];
            }
        }
    }
    free (cars);
    cars = new;
    n_cars--;
}

void printCars () {
    printTitle ("Car Vectors");
    for (int i = 0; i < n_cars; i++) {
        printCar (&cars[i]);
    }
    printTitle (""); 
}
