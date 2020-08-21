#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 1024

#define BUFF_SIZE 1024



bool inputValidation(char *input)
{

    if((strncmp(input, "get", 3) == 0) || (strncmp(input, "put", 3) == 0) || (strncmp(input, "quit", 4) == 0))
        return true;

    return false;

}


bool checkExistingFile(char *filename)
{
    if(access(filename, F_OK ) != -1 )
        return true;
    else
        return false;
}


void putFile(int sockfd, char *filename){
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


void getFile(FILE *fp, int sockfd){
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

void clientServer(int sock)
{
    char how_to_use_msg[512], user_command[5], filename[512], comm_ok[2], goodbye_msg[128], file_found[2];
    char buff[BUFF_SIZE];
    memset(how_to_use_msg, '\0', 512);
    memset(filename, '\0', 512);
    comm_ok[0] = '\0';
    comm_ok[1] = '\0';

    file_found[0] = '\0';
    file_found[1] = '\0';

    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&sock;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char clientAddString[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, clientAddString, INET_ADDRSTRLEN );

    printf("A new client[%s] is connected\n", clientAddString);
    

    while(24)
    {
        //read(sock, buff, 1);
        memset(buff, '\0', BUFF_SIZE);
        read(sock, buff, BUFF_SIZE);
        buff[strlen(buff)] = '\0';
        // printf("HERE\n");
        printf("buff content:|%s|\n",buff);
        

        if(!inputValidation(buff))
            return;
        
        if((strncmp(buff, "quit", 4)) == 0 )
            return;

        
        if((strncmp(buff, "put", 3)) == 0)
        {
            
            char *filename = buff + 4;
            putFile(sock, filename);            
        }

        if((strncmp(buff, "get", 3)) == 0)
        {
            FILE *fp;
            char *filename = buff + 4;
            fp = fopen(filename, "r");
            if (fp == NULL) {
                perror("[-]Error in reading file.");
                exit(1);
            }       

            getFile(fp, sock);
            fclose(fp);
            
        }
    }
}



int main()
{
  int port = 1212;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, clientaddr;
  socklen_t addr_size;
  char buffer[SIZE];
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  
  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

  if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		perror("[-]Error in listening");
    exit(1);
	}


  for(;;)
    {
        int pid, new_sock, clientlen;
        clientlen = sizeof(clientaddr);
        new_sock = accept(sockfd, (struct sockaddr *)&clientaddr, &clientlen);

        if ((pid = fork()) == -1)
        {
            close(new_sock);
            perror("Error While Creating Child Process: ");
            exit(3);
        }


        else if(pid == 0)
        {
            close(sockfd);
            clientServer(new_sock);
            exit(0);
        }
    }
    close(new_sock);

//   addr_size = sizeof(new_addr);
//   new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
//   write_file(new_sock);
//   printf("[+]Data written in the file successfully.\n");

  return 0;
}
