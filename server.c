#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFF_SIZE 500


struct time;
struct process;
struct client;
struct time{
    int hour;
    int minute;
    int second;

};

struct process{
    int PID;
    struct time start;
    struct time stop;
    char name[100];
    bool running;
};

struct client{
  char ip[INET_ADDRSTRLEN];
  int port;
  int fd[2];
  int pid;
  int socket;
  bool running;

};



struct process creator(char word[], int fd);
void help(int fd);
void listprocess(struct process p[], int counter, int fd);
int killprocess( int pid, int fd);
void listactive(struct process p[], int counter, int fd);
void killname(char *token, int fd);
void killall( int fd);
int makeServer();
void sig_handler(int sig, siginfo_t *a, void *notused);
void commands();
void listener();
int getfd(struct client c[], char input[]);
bool checkfd(int fd);
int getsocket(struct client c[], char input[]);

struct client c[100];
int no_of_clients = 0;
int withserverfd[2];
struct process p[500];
int counter = 0;    



void sig_handler(int sig, siginfo_t *a, void *notused){
    if (sig == SIGUSR1){
      write(STDOUT_FILENO,"SIGUSR1 received", 16);
      if(a->si_int== 2){
        write(STDOUT_FILENO,"2", 1);
      }
    }
    if (sig == SIGCHLD){
      write(STDOUT_FILENO,"SIGCHLD received", 16);
      int status;
            for(int i=0;i< no_of_clients ;i++){
                if(c[i].running==1){
                    if(waitpid(c[i].pid, &status, WNOHANG)>0){
                        c[i].running = 0;
                    }
                }
            }
    }
}



int main()
{
    int no_use;
    struct sigaction handler;
    handler.sa_flags = SA_RESTART | SA_SIGINFO;
    handler.sa_sigaction = &sig_handler;
    sigaction (SIGUSR1, &handler, NULL);
    sigaction( SIGCHLD, &handler, NULL );
   
    pthread_t listenz, command;
    pthread_create(&command, NULL, commands, (void *) &no_use);
    pthread_detach(command);

    int fd = makeServer();
    pthread_create(&listenz, NULL, listener, (void *) fd);
    pthread_detach(listenz);
    write(STDOUT_FILENO, "Client Connected", strlen("Client Connected"));


    pid_t pidc;
    pid_t client;
    char inp[BUFF_SIZE];
    char temp[100] = "temp";
    char input [BUFF_SIZE];
    char *token = "initialize";
    char conv[10];

    //file descriptor of help
    int helpfd;
    char app[200];
    int com;
    float temp1;
    float temp2;
  
  while(strcmp(token,"quit\n")!=0){
    temp1 = 0;
    temp2 = 1;

    com = read(fd, input, BUFF_SIZE);

    input[com] = '\0';
    token = strtok(input, " ");

    if((strcmp(token,"list")==0)||(strcmp(token,"list\n")==0)){
      token = strtok(NULL, " ");
      if(token != NULL){
        if( strcmp(token,"all\n")==0 ){
          listprocess(p, counter, fd);
          continue;
        }
        else{
          listactive(p, counter, fd);
          continue;
        }
      }
      token = "abc";
        
    }

    if (strcmp(token,"run")==0){
      token = strtok(NULL, "\n"); 
      struct process temp = creator(token, fd);
      if(temp.PID != 0){
          p[counter] = temp;
          counter++;
      }
      token = "abc";
      continue;
    }




    if(strcmp(token,"kill")==0){
      token = strtok(NULL, "\n");
      int pid = atoi(token);
      if(pid != NULL){
          int temp = kill(pid, SIGKILL);  
          if(temp==-1){
            perror("KILL");
          }
          continue;
      }
      else if(strcmp(token,"all")==0){
        killall(fd);
      }
      else{
          killname(token, fd);
      }
    }


    if (strcmp(token,"print")==0){
      token = strtok(NULL, " ");
      while( token != NULL ){
        sleep(2);
        write(fd, token, strlen(token));
        write(fd, "\n", sizeof("\n"));
        token = strtok(NULL, " ");
      }
      token = "abc";
      continue;
    }


    if (strcmp(token,"help\n")==0){
      helpfd = open("help.txt",O_RDONLY);
      while((com = read(helpfd,input, BUFF_SIZE))>0){
        write(fd, input, com);
      }
      token = "abc";
      continue;
    }


    if(strcmp(token,"add")==0){
      while( token != NULL ){
        //write(STDOUT_FILENO, token, strlen(token));
        temp1 = atof(token)+ temp1;
        token = strtok(NULL, " ");
      }
        sprintf(conv,"%f",temp1);
        write(fd, conv, strlen(conv));
        write(fd, "\n", sizeof("\n"));
        token = "abc";
        continue;
    }

    if (strcmp(token,"sub")==0){
      temp1 = atof(strtok(NULL, " "));
      while( token != NULL ){
        token = strtok(NULL, " ");
        if(token!= NULL){
          temp1 -= atof(token);
        }
        
      }
      sprintf(conv,"%f",temp1);
      write(fd, conv, strlen(conv));
      write(fd, "\n", sizeof("\n"));
      token = "abc";
      continue;
    }

    if(strcmp(token,"mul")==0){
      token = strtok(NULL, " ");
      while( token != NULL )
      {
        temp2 *= atof(token);
        token = strtok(NULL, " ");
      }
      sprintf(conv,"%f",temp2);
      write(fd, conv, strlen(conv));
      write(fd, "\n", sizeof("\n"));
      token = "abc";
      continue;

    }

    if (strcmp(token,"div")==0){
      temp2 = atof(strtok(NULL, " "));
      while( token != NULL )
      {
        token = strtok(NULL, " ");
        if(token != NULL){
          temp2 /= atof(token);
        }
      }
      sprintf(conv,"%f",temp2);
      write(fd, conv, strlen(conv));
      write(fd, "\n", sizeof("\n"));
      token = "abc";
      continue;
    }
    if(strcmp(token,"disconnect\n")==0){
      write(fd, "server disconnecting\n",21);
      close(fd);
      exit(1);
    }
    if(strcmp(token,"quit\n")!=0){
      write(fd, "Invalid Command. Type \"help\" to see the list of commands\n",57);
    }

    
  }

  write(fd, "server quitting", sizeof("server quitting"));
  killall(fd);
  close(fd);
  exit(1);
}

struct process creator(char word[], int fd){
    int no_use_pipe[2];
    int com;
    pipe(no_use_pipe);
    int pidc = fork();
    if(pidc == -1)
    {
        perror("Fork Failed");
        write(fd, "Fork Failed\n", sizeof("Fork Failed\n"));
        struct process p;
        p.PID = 0;
        return p;
    }

      if(pidc==0){
        execvp(word,NULL);
        perror("Could not execute");
        exit(1);
      }
      else{
       
                time_t t = time(0);
                struct tm * now = localtime( & t );

                struct process p;
                p.PID = pidc;
                strcpy(p.name,word);
                p.start.hour = now->tm_hour;
                p.start.minute = now->tm_min;
                p.start.second = now->tm_sec;
                p.running = 1;

                //write(STDOUT_FILENO, "Starting"+ word +"\n", 8+sizeof(word)+1);
                write(fd, "Application Executed Successfully\n", sizeof("Application Executed Successfully\n"));

                return p;
        //write(fd, "Application Failed to Execute\n", 30);

        
    }

    // Start the child process.
    

}
void help(int fd){
  int com;
  char input [BUFF_SIZE];
  int helpfd = open("help.txt",O_RDONLY);
  while((com = read(helpfd,input, BUFF_SIZE))>0){
    write(fd, input, com);
  }

}

void listprocess(struct process p[], int counter, int fd){
  char buff[BUFF_SIZE];
  write(fd,"\n",1);
  write(fd," +-----------------------+-----+----------+----------+-----------+\n",67);
  write(fd," |         Name          | PID |  Started |   Ended  |  Running  |\n",67);
  write(fd," +-----------------------+-----+----------+----------+-----------+\n",67);
  for(int i=0;i<counter;i++){
          if(p[i].PID == 0){
              //printf("End");
              break;
          }
      sprintf(buff, " |%23s|%5d|",p[i].name,p[i].PID);
      write(fd,buff, strlen(buff));
      sprintf(buff, " %2d:%2d:%2d |", p[i].start.hour,p[i].start.minute,p[i].start.second);
      write(fd,buff, strlen(buff));
      if(p[i].running==0){
      sprintf(buff, " %2d:%2d:%2d |Not Active |\n", p[i].stop.hour,p[i].stop.minute,p[i].stop.second);
      }
      else{
          sprintf(buff, "          |   Active  |\n");
      }
      write(fd,buff, strlen(buff));
  }
  write(fd," +-----------------------+-----+----------+----------+-----------+\n",67);

}

void listactive(struct process p[], int counter, int fd){
  char buff[BUFF_SIZE];
  write(fd,"\n",1);
  write(fd," +-----------------------+-----+----------+\n",44);
  write(fd," |         Name          | PID |  Started |\n",44);
  write(fd," +-----------------------+-----+----------+\n",44);
  for(int i=0;i<counter;i++){
      if(p[i].PID ==0){
          //printf("End");
          break;
      }
      if(p[i].running==1){
      
      sprintf(buff, " |%23s|%5d| ",p[i].name,p[i].PID);
      write(fd,buff, strlen(buff));
      sprintf(buff, " %2d:%2d:%2d|\n", p[i].start.hour,p[i].start.minute,p[i].start.second);
      write(fd,buff, strlen(buff));

      }

  }
  write(fd," +-----------------------+-----+----------+\n",44);

}

int killprocess(int pid, int fd){
char buff[BUFF_SIZE];
    for(int i=0;i<counter;i++){

        if(p[i].PID == pid && p[i].running==1){

          kill(pid, SIGKILL);

            //Updating list
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            p[i].stop.hour = now->tm_hour;
            p[i].stop.minute = now->tm_min;
            p[i].stop.second = now->tm_sec;
            p[i].running = 0;

            sprintf(buff, "Killed process \"%s\" having PID of %d\n", p[i].name,pid);
            write(fd,buff, strlen(buff));
            return 1;
        }

    }
    
    sprintf(buff, "No running process with PID %d exist\n",pid);
    write(fd,buff, strlen(buff));
    close(fd);
    return 0;
  }


void killname( char *token, int fd){
    int killed= 0;  
    char buff[BUFF_SIZE];
    for(int i=0;i<counter;i++){

        if (strcmp(p[i].name,token)==0 && p[i].running==1){

           kill(p[i].PID, SIGKILL);

            //Updating list
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            p[i].stop.hour = now->tm_hour;
            p[i].stop.minute = now->tm_min;
            p[i].stop.second = now->tm_sec;
            p[i].running = 0;
            killed++;
        }

    }
    sprintf(buff, "Killed %d instance(s) of %s\n",killed,token);
    write(fd,buff, strlen(buff));

}
void killall(int fd){
    int killed= 0;  
    char buff[BUFF_SIZE];
    for(int i=0;i<counter;i++){

        if (p[i].running==1){

           kill(p[i].PID, SIGKILL);

            //Updating list
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            p[i].stop.hour = now->tm_hour;
            p[i].stop.minute = now->tm_min;
            p[i].stop.second = now->tm_sec;
            p[i].running = 0;
            killed++;
        }

    }
    sprintf(buff, "Killed %d processes\n",19);
    write(fd,buff, strlen(buff));

}

int makeServer(){
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 2626;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR on binding");
    while(1){
        int fd[2];
        pipe(fd);
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        inet_ntop(AF_INET, &(cli_addr.sin_addr), c[no_of_clients].ip, INET_ADDRSTRLEN);
        c[no_of_clients].port = cli_addr.sin_port;
        c[no_of_clients].socket = newsockfd;
        c[no_of_clients].running = 1;
        int pid = fork();
        if (pid==0){
            if (newsockfd < 0){
            perror("ERROR on accept");
            // Tell parent to decrement count HERE
        }

        write(STDOUT_FILENO,"after\n",6);
        withserverfd[0] = fd[0];
        withserverfd[1] = fd[1];
        return newsockfd; 
       }
       else{
        c[no_of_clients].fd[0] = fd[0];
        c[no_of_clients].fd[1] = fd[1];
        c[no_of_clients].pid = pid;
        no_of_clients++;
       }
   }
}

void commands(){
    char input[BUFF_SIZE];
    int com;
    int fd,sock;
    char *token;
    while(strcmp(token,"quit")!=0){
        com = read(STDIN_FILENO, input, BUFF_SIZE);
        input[com-1] = '\0';
        if(com<2){
            write(STDOUT_FILENO, "\n> ",3);
            continue;
        }
        fd = getfd(c, input);
        sock = getsocket(c, input);
        //printf("%d",fd);

        token = strtok(input, " ");
        if (strcmp(token,"list")==0){
            char buff[BUFF_SIZE];
            write(STDOUT_FILENO,"\n",1);
            write(STDOUT_FILENO," +---------------------+------+-------+-------+\n",48);
            write(STDOUT_FILENO," |          IP         | Port |  PID  |Socket |\n",48);
            write(STDOUT_FILENO," +---------------------+------+-------+-------+\n",48);
            for(int i=0;i< no_of_clients ;i++){
                if(c[i].running==1){
                    sprintf(buff, " |%21s|%6d|%7d|%7d|\n",c[i].ip,c[i].port,c[i].pid, c[i].socket);
                    write(STDOUT_FILENO,buff, strlen(buff));
                }   
            }
            write(STDOUT_FILENO," +---------------------+------+-------+-------+\n",48);
            continue;
            //  token = "abc";
        }
        if(strcmp(token,"kill")==0){
            token = strtok(input, " ");
            token = strtok(input, " ");
            token = strtok(input, " ");
        }
        if(strcmp(token,"message")==0){
            if(checkfd(fd)){
                token = strtok(NULL, " ");
                token = strtok(NULL, " ");
                token = strtok(NULL, " ");
                write(STDOUT_FILENO,token, strlen(token));
                write(sock,token, strlen(token));
            }
            
           }
        if (strcmp(token,"quit")==0)
        {
            //Disconnect all client code:
            exit(1);
        }
    }
    exit(1);
    
}

int getfd(struct client c[], char input[]){
    char *token;
    char ip[INET_ADDRSTRLEN];
    int port;

    token = strtok(input, " ");
    if (token != NULL)
    {
        token = strtok(NULL, " ");
        
        if (token != NULL)
        {
            strcpy(ip,token);
            token = strtok(NULL, " ");
            if(token != NULL){
                port = atoi(token);
            }
        }
        else{
            return -1;
        }
    }
    else{
        return 1;
    }

    for(int i=0;i< no_of_clients ;i++){
        if(c[i].running==1){
            if((strcmp(ip,c[i].ip)==0)&& port==c[i].port){
                return c[i].fd[1];
            }
        }   
    }
    return -1;
}

int getsocket(struct client c[], char input[]){
    char *token;
    char ip[INET_ADDRSTRLEN];
    int port;

    token = strtok(input, " ");
    if (token != NULL)
    {
        token = strtok(NULL, " ");
        
        if (token != NULL)
        {
            token = strtok(NULL, " ");
            if(token != NULL){
                port = atoi(token);
            }
        }
        else{
            return -1;
        }
    }
    else{
        return 1;
    }

    for(int i=0;i< no_of_clients ;i++){
        if(c[i].running==1){
            if((strcmp(ip,c[i].ip)==0)&& port==c[i].port){
                return c[i].socket;
            }
        }   
    }
    return -1;


}


bool checkfd(int fd){
    if(fd == -1){
        write(STDOUT_FILENO, "Invalid Client", 14);
        return 0;
    }
    else{
        return 1;
    }
}

void listener(void * ptr){

    int fd = ptr;
    char input[200];
    char *token; 
    int com;
    while(1){
        write(STDOUT_FILENO,"Listening",9);
        read(withserverfd[0],input, sizeof(input));
        input[com] = '\0';

        write(STDOUT_FILENO, input, com);


    }
}