#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define ERROR_LOG(errorlog) do{\
                                printf("%s--%s--%d\n", __FILE__, __func__, __LINE__);\
                                perror("errorlog");\
                                exit(1);\
                              }while(0)

int main(int argc, char const *argv[])
{
    int sockfd = 0;
    int num = 1;
    struct sockaddr_in sendaddr;
    unsigned char msg[128] = ""; 
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        ERROR_LOG("fail to socket");
    }

    struct ip_mreq multi;
    multi.imr_multiaddr.s_addr = inet_addr(argv[1]);
    multi.imr_interface.s_addr = INADDR_ANY;

    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multi, sizeof(struct ip_mreq)) < 0)
    {
        ERROR_LOG("fail to setsockfd");
    }

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_addr.s_addr = inet_addr(argv[1]);
    sendaddr.sin_port = htons(atoi(argv[2]));

    if(bind(sockfd, (struct sockaddr *)&sendaddr, addrlen) < 0)
    {
        ERROR_LOG("fail to bind");
    }

    while(1)
    {
        if(recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            ERROR_LOG("fail to receive");
        }

        printf("接收到来自[%s--%d]的信息为:%s\n", inet_ntoa(sendaddr.sin_addr), ntohs(sendaddr.sin_port), msg);
    }
    
    close(sockfd);

    return 0;
}
