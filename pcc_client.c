#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define BUFFSIZE 1024

// Nofar Vered Nahum 315391367

int main(int argc, char *argv[])
{
    char *ipAddress;
    int port;
    char *file_name;
    FILE *file;
    int sockfd;
    struct sockaddr_in serverAddress;
    int ipB;
    uint32_t N;
    uint32_t N_net;
    char message[BUFFSIZE];
    uint32_t C;

    if (argc < 4 || argc > 4)
    {
        perror("ERROR number of arguments pcc_client.\n");
        exit(1);
    }

    ipAddress = argv[1];
    port = atoi(argv[2]);
    file_name = argv[3];
    file = fopen(file_name, "rw");
    if (file == NULL)
    {
        perror("ERROR open file .\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR sockert failed .\n");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    ipB = inet_pton(AF_INET, ipAddress, &(serverAddress.sin_addr));
    if (ipB == 0 || ipB == -1)
    {
        perror("ERROR inet_ptonipB.\n");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("ERROR conenct.\n");
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    N = (uint32_t)ftell(file);
    N_net = htonl(N);

    if (send(sockfd, &N_net, sizeof(N_net), 0) < 0)
    {
        perror("ERROR write N.\n");
        exit(1);
    }

    fseek(file, 0L, SEEK_SET);
    if (fread(message, 1, sizeof(message), file) <= 0)
    {
        perror("ERROR read file.\n");
        exit(1);
    }
    else
    {
        if (send(sockfd, &message, strlen(message), 0) < 0)
        {
            perror("ERROE send message.\n");
            exit(1);
        }
        else
        {
            if (recv(sockfd, &C, sizeof(C), 0) < 0)
            {
                perror("ERROR read C.\n");
                exit(1);
            }
        }
    }
    printf("# of printable characters: %u\n", C);
    fclose(file);
    close(sockfd);
    exit(0);
}
