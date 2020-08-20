#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{

    char message[BUFF_SIZE], user_input[BUFF_SIZE], end_of_file[2] = {0x4, 0x0};
    int server, portNumber, pid, n;

    // server socket address
    struct sockaddr_in servAdd;

    // argument validation
    if(argc != 3)
    {
        printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
        exit(0);
    }

    // create socket
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket: ");
        exit(1);
    }

    // configure server socket address parameter
    servAdd.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);

    // changing the presesntation of IP
    if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0)
    {
        perror("inet_pton() failed to assign the address");
        exit(2);
    }

    // establishing a connection with server
    if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0)
    {
        perror("Sorry mate, I couldn't connect");
        exit(3);
    }

    // reading server's message initially
    read(server, message, BUFF_SIZE);
    message[strlen(message)] = '\0';
    printf("\n\nMessage received from server: \n%s"), message;


    while (1)
    {
        char *filename;
        char dummy[1];
        dummy[0] = 0;
        write(server, dummy, 1);
        memset(user_input, '\0', BUFF_SIZE);
        fgets(user_input, BUFF_SIZE, stdin);
        user_input[strlen(user_input) - 1] = '\0';
        printf("%s\n", user_input);
        write(server, user_input, strlen(user_input) + 1);
        
        //wait for server's response to see if the user's input is successfully evaluated
        read(server, message, BUFF_SIZE);
        message[strlen(message)] = '\0';
        if(message[0] != '0')
        {
            printf("Please enter 'get filename', 'put filename', or 'quit'\n");
            continue;
        }

        
        if(user_input[0] == 'q' && user_input[1] == 'u' && user_input[2] == 'i' && user_input[3] == 't')
        {
            printf("We hope you enjoyed using our awesome client-server application! :)\n");
            close(server);
            return 0;
        }

        filename = user_input + 4;

        if(user_input[0] == 'g' && user_input[1] == 'e' && user_input[2] == 't')
        {
            int fd;
            // ask the server if the user's requested file is a valid one
            ssize_t len = send(server, filename, strlen(filename), 0);
            
            read(server, message, BUFF_SIZE);
            message[strlen(message)] = '\0';
        
            if(message[0] != '0')
            {
                printf("Err: File was not found on server!\n");
                continue;
            }

            printf("The file you requested was found on the server.\n");
            filename[len] = '\0';
            printf("FILENAME: %s\n" , filename);
            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            memset(message, '\0', BUFF_SIZE);
            //sleep(3);
            //send(server, message, 1, 0);
            len = read(server, message, BUFF_SIZE);
            // printf("message: %s\n", message);
            // printf("%d\n", len);
            message[len] = '\0';
            while(message[0] != 0x4)
            {
                printf("FILE CONTENT: %s\n", message);
                fflush(stdout);
                write(fd, message, strlen(message));
                len = read(server, message, BUFF_SIZE);
                message[len] = '\0';
            }
            //sleep(5);
            write(server, dummy, 1);
            sleep(4);
            read(server, message, BUFF_SIZE + 1);
            message[strlen(message)] = '\0';
            printf("Message from server:\n%s\n", message);
            fflush(stdout);
            write(fd, end_of_file, 1);
            close(fd);
            continue;
        }

        // if(user_input[0] == 'p' && user_input[1] == 'u' && user_input[2] == 't')
        // {
        //     int fd;
        //     char buff[BUFF_SIZE];

        //     if((fd = open(filename, O_RDONLY)) == -1)
        //     {
        //         perror("Error While Opening File: ");
        //         continue;
        //     }
            

        //     while((read(fd, buff, BUFF_SIZE)) > 0)
        //     {
        //         buff[strlen(buff)] = '\0'; 
        //         send(server, buff, strlen(buff), 0);
        //     }

        //     send(server, end_of_file, 1, 0);
        //     close(fd);
        //     //finish the stream for now
        //}
        
    }

}




