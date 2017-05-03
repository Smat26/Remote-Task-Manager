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

#define BUFF_SIZE 500


struct time;
struct process;
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

struct process creator(char word[], int fd);
struct process creator(char word[], int fd);
void help(int fd);
void listprocess(struct process p[], int counter, int fd);
int killprocess(struct process p[],int counter, int pid, int fd);
void listactive(struct process p[], int counter, int fd);
int killname(struct process p[], int counter, char *token, int fd);
int makeServer();



int main()
{

  int fd = makeServer();
  write(STDOUT_FILENO, "Client Connected", strlen("Client Connected"));
  pid_t pidc;
  pid_t client;
  char inp[BUFF_SIZE];
  char charfd[2];



  char temp[100] = "temp";
  struct process p[500];
  int counter = 0;

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
    
    //write(fd, "\n> ", sizeof("\n> "));

    com = read(fd, input, BUFF_SIZE);
    //com = read(STDIN_FILENO, input, BUFF_SIZE);
    // printf("%d\n",com );
    // printf("%s\n",input);
    input[com] = '\0';
    token = strtok(input, " ");
    //write(STDOUT_FILENO, token, strlen(token));

    //write(STDOUT_FILENO, token, strlen(token));
    //write(STDOUT_FILENO, input, sizeof(input));

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
      else{
          killname(p,counter, token, fd);
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
    if(strcmp(token,"quit\n")!=0){
      write(fd, "Invalid Command. Type \"help\" to see the list of commands\n",57);
    }
    
  }
  write(fd, "Quitting...", sizeof("Quitting..."));

  exit(1);
}

struct process creator(char word[], int fd){

    int pidc = fork();
      if(pidc==0){
        execvp(word,NULL);
        perror("Could not execute");
        struct process p;
        p.PID = 0;
        return p;
      }

    // Start the child process.
    if(pidc == -1)
    {
        perror("Fork Failed");
        write(fd, "Fork Failed\n", sizeof("Fork Failed\n"));
        struct process p;
        return p;
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
    }


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
          if(p[i].PID == NULL){
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
      if(p[i].PID == NULL){
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

int killprocess(struct process p[], int counter, int pid, int fd){
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


int killname(struct process p[], int counter, char *token, int fd){
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

int makeServer(){
   int sockfd, newsockfd, portno, clilen;
     char buffer[256];
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
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              perror("ERROR on binding");
    while(1){
     listen(sockfd,5);

     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     int pid = fork();
     if (pid==0){
      if (newsockfd < 0){
      perror("ERROR on accept");
    }
     write(STDOUT_FILENO,"after\n",6);
     return newsockfd; 
   }
   }
}
