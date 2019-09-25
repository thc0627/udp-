#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

typedef struct 
{
    char ip[16];
    short port;
    char text[256];
}MSG;

typedef struct addr
{
    struct sockaddr_in addr;
    struct addr *next;
}ADDR;

ADDR* insert_addr(ADDR *head, struct sockaddr_in sendaddr)
{
    ADDR *addr_tmp = (ADDR *)calloc(1, sizeof(ADDR));
    addr_tmp->addr = sendaddr;
    addr_tmp->next = NULL;

    if(head == NULL)
    {
        head = addr_tmp;
    }
    else
    {
        ADDR *pb = head;
        ADDR *pf = head;
        while((pb != NULL) && (pb->addr.sin_port != sendaddr.sin_port))
        {
            pf = pb;
            pb = pb->next;
        }

        if(pb == NULL)
        {
            pf->next = addr_tmp;
        }
    }  

    return head;
}

int main(int argc, char const *argv[])
{
    int sockfd;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

    if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
    {
        perror("bind");
        exit(1);
    }

    MSG msg;
    ADDR *head = NULL;
    unsigned char buf[256] = "";
    struct sockaddr_in sendaddr;
    struct sockaddr_in recvdaddr;
 
    while (1)
    {
        if(recvfrom(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        //printf("[%s--%d]:%s\n", inet_ntoa(sendaddr.sin_addr), ntohs(sendaddr.sin_port), buf);

        head = insert_addr(head, sendaddr);
        ADDR *pb = head;

        while (pb != NULL)
        {
            if(pb->addr.sin_port != sendaddr.sin_port)
            {
                recvdaddr.sin_family = AF_INET;
                recvdaddr.sin_addr.s_addr = pb->addr.sin_addr.s_addr;
                recvdaddr.sin_port = pb->addr.sin_port;

                strcpy(msg.ip, inet_ntoa(sendaddr.sin_addr));
                msg.port = ntohs(sendaddr.sin_port);
                strcpy(msg.text, buf);
                printf("[%s - %d]:%s\n", msg.ip, msg.port, msg.text);

                if(sendto(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&recvdaddr, addrlen) < 0)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            pb = pb->next;
        }
    }
    
    return 0;
}
