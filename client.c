#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SIZE 1024

#define BUFF_SIZE 1024



bool inputValidation(char *input)
{

    if((strncmp(input, "get", 3) == 0) || (strncmp(input, "put", 3) == 0) || (strncmp(input, "quit", 4) == 0))
        return true;

    return false;

}



void putFile(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};
  char end_of_file[2] = {0x4, 0x0};

  while(fgets(data, SIZE, fp) != NULL) {
      printf("SENT %s\n", data);
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
    send(sockfd, end_of_file, 2, 0);
    return;
  
}

void getFile(int sockfd, char *filename){
  int n;
  FILE *fp;
  //char *filename = "recv.txt";
  char buffer[SIZE];

  fp = fopen(filename, "wb+");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if(buffer[0] == 0x4)
        return;
    
    if(buffer[0] == 0x0)
        return;
    printf("RECEIVED: %s\n", buffer);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s\n", buffer);
    
    bzero(buffer, SIZE);
  }
  fclose(fp);
  return;
}


int main()
{
  char *ip = "137.207.82.52";
  int port = 1212;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons((uint16_t)port);
  
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) < 0)
    {
        perror("inet_pton() failed to assign the address");
        exit(2);
    }

    if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
    {
        perror("Sorry mate, I couldn't connect");
        exit(3);
    }

	printf("[+]Connected to Server.\n");


    while(19)
    {
        char *filename, user_input[BUFF_SIZE];
        memset(user_input, '\0', BUFF_SIZE);
        fgets(user_input, BUFF_SIZE, stdin);
        user_input[strlen(user_input) - 1] = '\0';
        // printf("%s\n", user_input);
        write(sockfd, user_input, strlen(user_input) + 1);
        
        if((strncmp(user_input, "quit", 4)) == 0)
        {
            printf("We hope you enjoyed using our awesome client-server application! :)\n");
            close(sockfd);
            return 0;
        }

        filename = user_input + 4;

        if((strncmp(user_input, "put", 3)) == 0)
        {
            filename = user_input + 4;
            fp = fopen(filename, "r");
            if (fp == NULL) {
                perror("[-]Error in reading file.");
                exit(1);
            }       

            putFile(fp, sockfd);
            fclose(fp);

        }

        if((strncmp(user_input, "get", 3)) == 0)
        {
            filename = user_input + 4;
            getFile(sockfd, filename);

        }

    }
//   fp = fopen(filename, "r");
//   if (fp == NULL) {
//     perror("[-]Error in reading file.");
//     exit(1);
//   }

//   send_file(fp, sockfd);
//   printf("[+]File data sent successfully.\n");

// 	printf("[+]Closing the connection.\n");
//   close(sockfd);

  return 0;
}
