#include "car.h"

int main(int argc, char * argv[]) {
    Car car;
    
    pid_t pid;
    
    struct hostent *host_address;
    struct sockaddr_in socket_address;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;
    
    // Verificacao de argumentos
    if (argc != 2) {
        // default: localhost
        host = "localhost";
    } else {
        // atribuicao do host como paramentro
        host = argv[1];
    }
    
    // resolvendo nomes
    host_address = gethostbyname(host);
    
    if (!host_address) {
        printf("error: couldn't resolve name");
        return 1;
    }
    
    // copiando endereco para string
    struct in_addr a;
    while (*host_address->h_addr_list) {
        bcopy(*host_address->h_addr_list++, (char *) &a, sizeof(a));
    }
    
    // criacao do socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error: socket problem\n");
        return 1;
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
        return 2;
    }
    
    print("Connected!", NULL, socket_address);
    
    
    if ((pid = fork()) < 0) {
        perror("error forking");
        exit(1);
    }
    
    if (pid == 0) {
        
        // ler e enviar as linhas de texto
        printTitle("PRESS s to stop: ");
        while (1) {
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
        
        while (1) {
            
            printTitle("Waiting for orders...");
            if ( (len = read(s, &car, sizeof(Car))) == 0) {
                // disconnected
                printTitle("Disconnected!");
                exit(0);
            } else {
                // atualize velocidade do carro, de acordo com instrucoes recebidas
                printTitle("Adjusting speed!");
            }
            
        }
        
    }
    
    printTitle("STOPPED");
    
    // fechando a sessao
    close(s);
    return 0;
}
