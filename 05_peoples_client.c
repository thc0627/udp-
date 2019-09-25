#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

int sockfd;
struct sockaddr_in sendaddr;
struct sockaddr_in serveraddr;
socklen_t addrlen = sizeof(struct sockaddr_in);

typedef struct 
{
    char ip[16];
    short port;
    char text[256];
}MSG;

void* receive_fun(void *arg)
{
    MSG msg;

    while(1)
    {
        if(recvfrom(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&serveraddr, &addrlen) < 0)
        {
            perror("recvfrom");
            exit(1);
        }

        printf("[%s - %d]:%s\n", msg.ip, msg.port, msg.text);
    }
    
    return NULL;
}

int main(int argc, char const *argv[])
{

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_addr.s_addr = inet_addr(argv[3]);
    sendaddr.sin_port = htons(atoi(argv[4]));

    if(bind(sockfd, (struct sockaddr *)&sendaddr, addrlen) < 0)
    {
        perror("bind");
        exit(1);
    }

    pthread_t thread;
    if(pthread_create(&thread, NULL, receive_fun, NULL) != 0)
    {
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(thread);

    while (1)
    {
        char msg[256] = "";
        printf("请输入要发送的数据:\n");

        fgets(msg, sizeof(msg), stdin);
        msg[strlen(msg) - 1] = 0;

        if(sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *)&serveraddr, addrlen) < 0)
        {
            perror("sendto");
            exit(1);
        }
    }

    close(sockfd);
    
    return 0;
}
