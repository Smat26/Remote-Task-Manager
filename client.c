#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <stdbool.h>

int connectz(int port, char ip[] );


void listener(void *);

int main(int argc, char *argv[])
{
    char ip[INET_ADDRSTRLEN];
    int port;
    int sockfd = -1;
    bool is_connected = 0;


    char input[500];
    int com;
    pthread_t thread1;

    while(!is_connected){
        write(STDOUT_FILENO, "Please connect\n", sizeof("Please connect\n"));
        read(STDIN_FILENO, input, sizeof(input));
        //sscanf(input,"connect %s %d", ip, port);
        write(STDOUT_FILENO, ip, sizeof(ip));
        write(STDOUT_FILENO, &port, sizeof(port));
        sockfd = connectz(2626, "127.0.0.1");
        if(sockfd <0){
            write(STDOUT_FILENO, "Wrong input", sizeof("Wrong input"));
            continue;
        }
        else{
            is_connected  =1;
            pthread_create(&thread1, NULL, listener, (void *) sockfd);
            pthread_detach(thread1);
            }
        while((strcmp(input,"quit\n")!=0)&& is_connected){
            com = read(STDIN_FILENO, input, 500);
            input[com] = '\0';
            if(com>1){
                if(strcmp("disconnect\n", input)==0){
                    is_connected = 0;
                    break;
                }
                write(sockfd, input, com);
            }
        }
        if(is_connected){
            close(sockfd);
            pthread_cancel(&thread1);
            is_connected = 0;
        }
        
        
    }
    
    return 0;
}

void listener(void * ptr){
    int com;
    int sockfd = ptr;
    char input[500];
    while(1){
        com = read(sockfd, input, 500);
        if(com>1){
            write(STDOUT_FILENO, input, com);
        }
        if(com == -1){
            //this is to prevent trailing ">" on quitting
            break;
        }
            write(STDOUT_FILENO, "\n> ", sizeof("\n> "));
    }

}

int connectz(int port, char ip[] ){
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; 
    sleep(5);
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");

    return sockfd;
}
