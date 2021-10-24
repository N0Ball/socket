#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 12000

char *convert(char *src) {
    char *iter = src;
    char *result = malloc(sizeof(src));
    char *it = result;
    if (iter == NULL) return iter;

    while (*iter) {
        *it++ = *iter++ & ~0x20;
    }
    return result;
}

int main(int argc , char *argv[])
{
    // message buffer
    char buf[1024] = {0};

    int socket_fd = socket(PF_INET , SOCK_DGRAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in serverAddr = {
        .sin_family =AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(serverPort)
    };
    
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    printf("Server ready!\n");

    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    while (1) {
        if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0) {
            break;
        }

        if (strcmp(buf, "exit") == 0) {
            printf("get exit order, closing the server...\n");
            break;
        }
        
        char *conv = convert(buf);
        printf("get message from [%s:%d]: ", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("%s -> %s\n", buf, conv);

        sendto(socket_fd, conv, sizeof(conv), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        memset(buf, 0, sizeof(buf));
        free(conv);
    }

    close(socket_fd);
    return 0;
}