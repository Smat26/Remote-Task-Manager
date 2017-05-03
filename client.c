#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void listener(void *);

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    char input[500];
    int com;
    pthread_t thread1;
    pthread_create(&thread1, NULL, listener, (void *) sockfd);
    pthread_detach(thread1);
    while(strcmp(input,"quit\n")!=0){
        com = read(STDIN_FILENO, input, 500);
        input[com] = '\0';
        if(com>1){
            write(sockfd, input, com);
        }
    }
    pthread_cancel(&thread1);
    close(sockfd);
    
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
