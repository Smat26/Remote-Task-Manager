#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
int connectz(int port, char ip[] );


void * listener(void * ptr);
bool is_connected = 0;

int main(int argc, char *argv[])
{
    char ip[INET_ADDRSTRLEN];
    int port;
    int sockfd = -1;
    


    char input[500];
    int com;
    pthread_t thread1;
    union sigval value;
    value.sival_int = 2;

    //sigqueue(6146, SIGUSR1, value);
    write(STDOUT_FILENO, "Please connect\n", sizeof("Please connect\n"));
    while(!is_connected){
        com = read(STDIN_FILENO, input, sizeof(input));
        if(com<1){
            continue;
        }
        input[com]= '\0';
        sscanf(input,"connect %s %d", ip, &port);

        sockfd = connectz(port, ip);
        if(sockfd <0){
            write(STDOUT_FILENO, "Wrong input", sizeof("Wrong input"));
            continue;
        }
        else{
            is_connected  =1;
            pthread_create(&thread1, NULL, listener, (void *) sockfd);
            pthread_detach(thread1);
            }
        while((strcmp(input,"quit\n")!=0) && is_connected){
            com = read(STDIN_FILENO, input, 500);
            input[com] = '\0';
            if(com>1){
                if(strcmp("disconnect\n", input)==0){
                    is_connected = 0;
                }
                write(sockfd, input, com);
            }
            if(!is_connected){

            }
        }
        if((strcmp(input,"quit\n")==0)){
            break;
        }
        if(is_connected){
            close(sockfd);
            pthread_cancel(thread1);
            is_connected = 0;
        }
        
        
    }
    
    exit(1);
}

void * listener(void * ptr){
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
        if((strcmp(input,"server disconnecting\n") == 0)|| (strcmp(input,"server quitting\n") == 0)){
            //close(sockfd);
            is_connected = 0;
            write(STDOUT_FILENO, "Please connect\n", sizeof("Please connect\n"));
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
    if (sockfd < 0){
        perror("ERROR opening socket");
        exit(0);
    }
    server = gethostbyname(ip);
    if (server == NULL) {
        write(STDOUT_FILENO, "ERROR, no such host... Closing Client\n",38);
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        write(STDOUT_FILENO, "Error Connecting... Closing Client\n",34);
    write(STDOUT_FILENO,"Connected\n> ",12);

    return sockfd;
}
