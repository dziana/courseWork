#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MESSAGE_SIZE 50
char message[50];
/*
 * искать символ окончани команды
 * создавать новый процесс
 * */
char buf[MESSAGE_SIZE];
void showMenu()
{
    printf("1 - pulse\n");
    printf("2 - sine\n");
    printf("3 - sawtooth\n");
    printf("4 - triangle\n");
    printf("5 - square\n");
    printf("6 - exit\n");
}
int main()
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3422);
    addr.sin_addr.s_addr = inet_addr("127.0.1.1");
    int c;
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    while(1)
    {
        strcpy(message,"");
        showMenu();

        while (scanf("%d", &c) != 1)
            scanf("%*s");


        switch(c)
        {
            case 1:
            {
                strcat(message,"pulse");
                break;
            }
            case 2:
            {
                strcat(message,"sine");
                break;
            }
            case 3:
            {
                strcat(message,"sawtooth");
                break;
            }
            case 4:
            {
                strcat(message,"triangle");
                break;
            }
            case 5:
            {
                strcat(message,"square");
                break;
            }
            case 6:
            {
                close(sock);
                return 0;
            }
        }

        send(sock, message, MESSAGE_SIZE, 0);
        recv(sock, buf, MESSAGE_SIZE, 0);

        printf(buf);
        printf("\n");
    }
}


