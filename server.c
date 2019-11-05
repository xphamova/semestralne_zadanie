//
// Created by juliana on 11/4/19.
//
#include "sys/socket.h"
#include "netinet/in.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "arpa/inet.h"
#include "unistd.h"

main () {
    //vytvorenie socketu
    int sock_desc = socket(AF_INET,SOCK_STREAM,0);
    char c='\n';
    char *p_buf;
    if(sock_desc == -1)
    {
        printf("cannot create socket!\n");
        return 0;
    }
    //nastavenie socketu
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; //s ktorej ip sa da prihlasit
    server.sin_port = htons(59836);
    if (bind(sock_desc, (struct sockaddr*)&server, sizeof(server)) != 0)
    {
        printf("cannot bind socket!\n");//nepresiel port
        close(sock_desc);
        return 0;
    }
    if (listen(sock_desc,4) != 0)
    {
        printf("cannot listen on socket!\n");
        close(sock_desc);
        return 0;
    }
    struct  sockaddr_in client;
    memset(&client,0, sizeof(client));
    socklen_t len = sizeof(client);
    int temp_sock_desc = accept(sock_desc,(struct sockaddr*)&client, &len);
    if (temp_sock_desc == -1) //docastny sock_desc
    {
        printf("cannot accept client!\n");
        close(sock_desc);
        return 0;
    }
    int buf[10];
    int k;
    int Second_sock_desc = socket(AF_INET,SOCK_STREAM,0);
    if (Second_sock_desc == -1)
    {
        printf("cannot create socket!\n");
        return 0;
    }
    //nastavenie socketu
    struct sockaddr_in Second_client;
    memset(&Second_client,0, sizeof(Second_client));
    Second_client.sin_family = AF_INET;
    Second_client.sin_addr.s_addr = inet_addr("127.0.0.1");
    Second_client.sin_port = htons(49836);
    //pripojenie socketu
    if(connect(Second_sock_desc,(struct sockaddr*)&Second_client, sizeof(Second_client)) != 0)
    {
        printf("cannot conncet to server!\n");
        close(Second_sock_desc);
    }

    gets(buf);
    len = sizeof(buf);
    p_buf = buf;
    while(len > 0)
    {
       k = send(Second_sock_desc, p_buf,len,0);//posielanie dat
       if (k == -1)
       {
           printf("cannot write to server!\n");
           break;
       }
       p_buf += k;
       len -= k;
    }
    k = send(Second_sock_desc,&c,1,0);
    if (k == -1)
    {
        printf("cannot write to server!\n");
    }


    k = recv(temp_sock_desc,buf,10,0);
    if(recv == -1)
    {
        printf("\ncannot read from client!\n");
    }
    if (recv == 0)
    {
        printf("\nclient disconnected\n");
    }
    if(k > 0)
        printf("%*.*s",k,k,buf);
    close(temp_sock_desc);
    close(sock_desc);
    close(Second_sock_desc);
    printf("Server disconnected\n");
    return 0;

}