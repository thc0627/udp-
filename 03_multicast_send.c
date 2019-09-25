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

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_addr.s_addr = inet_addr(argv[1]);
    sendaddr.sin_port = htons(atoi(argv[2]));

    while(1)
    {
        printf("请输入要发送的信息:");
        fgets(msg, sizeof(msg), stdin);
        msg[strlen(msg)-1] = '\0';
        
        if(sendto(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&sendaddr, addrlen) < 0)
        {
            ERROR_LOG("fail to send");
        }
    }

    close(sockfd);

    return 0;
}
