#include<signal.h>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>


#define BUFF_SIZE 1024


bool inputValidation(char *input)
{

    if((input[0] == 'g' && input[1] == 'e' && input[2] == 't') || (input[0] == 'p' && input[1] == 'u' && input[2] == 't'))
        return true;
    
    return false;
    
}


bool checkExistingFile(char *filename)
{

}


void putFile(char *filename, int sock)
{
    int fd;
    char buff[BUFF_SIZE];
    if((fd = open(filename, O_WRONLY | O_CREAT)) == -1)
    {
        perror("Error While Opening File: ");
        return;
    }
    

    recv(sock,&buff, BUFF_SIZE - 1,0);
    while(buff[0] != 0x4)
    {
        write(fd, buff, strlen(buff));
        recv(sock,&buff, BUFF_SIZE - 1,0);
    }
     
}


void getFile(char *filename, int sock)
{
    int fd;
    char buff[BUFF_SIZE];
    if((fd = open(filename, O_RDONLY)) == -1)
    {
        perror("Error While Opening File: ");
        return;
    }
    
    if(read(fd, buff, BUFF_SIZE) != BUFF_SIZE)
    {
        perror("Error While Reading File: ");
        return;
    }

    while(buff[0] != 0x4)
    {
        if(send(sock, buff, strlen(buff), 0) != strlen(buff))
        {
            perror("Error While Writing on Socket: ");
            return;
        }

        if(read(fd, buff, BUFF_SIZE) != BUFF_SIZE)
    {
        perror("Error While Reading File: ");
        return;
    }
        
    }
}

void clientServer(int sock)
{
    char how_to_use_msg[512], filename[512];
    char buff[BUFF_SIZE];
    memset(how_to_use_msg, '\0', 512);
    memset(filename, '\0', 512);
    strcpy(how_to_use_msg, "Please enter 'get fileName', 'put fileName', or 'quit'\nAny other command will not be executed\n");
    send(sock, how_to_use_msg, strlen(how_to_use_msg), 0);

    while(24)
    {
        memset(buff, '\0', BUFF_SIZE);
        recv(sock,&buf, BUFF_SIZE - 1,0);

        if(strcmp(buf, "quit") == 0)
        {
            close(sock);
            return;
        }

        // input validation
        if(!inputValidation(buff))
        {
            send(sock, how_to_use_msg, strlen(how_to_use_msg), 0);
            continue;
        }

        for(int i = 0; buff[i] , i++)
            filename[i] = buff[i + 4];
        
        if(!checkExistingFile(filename))
        {
            char file_not_found_msg[256];
            memset(file_not_found_msg, '\0', 256);
            strcpy(file_not_found_msg, "The file you are looking for does not exist!\n");
            send(sock, file_not_found_msg, strlen(file_not_found_msg), 0);
            continue;
        }


    }
    

    printf("here 1\n");
//    snprintf(buf, sizeof buf, "hi %d", counter);
    send(sock, buf, strlen(buf), 0);
    close(sock);
}

int main()
{

    struct sockaddr_in myaddr ,clientaddr;
    int sockid, newsockid, len;
    sockid = socket(AF_INET,SOCK_STREAM,0);
    memset(&myaddr,'0',sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(54621);
    myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(sockid == -1)
        perror("Socket Error: ");
    
    len = sizeof(myaddr);

    if(bind(sockid,( struct sockaddr*)&myaddr,len)==-1)
        perror("Error While Binding: ");

    if(listen(sockid,10)==-1)
        perror("Error While Listening: ");
    
    

    for(;;)
    {
        int pid, new_sock;
        new_sock = accept(sockid, (struct sockaddr *)&clientaddr, &len);

        if ((pid = fork()) == -1)
        {
            close(new_sock);
            perror("Error While Creating Child Process: ");
            continue;
        }

        else if(pid > 0)
            continue;

        else if(pid == 0)
            clientServer(new_sock);

    }
    printf("here3");
    close(sockid);
    return 0;
}
