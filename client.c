#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
int k=0;
void * find(void *udaj) {

    int i, prvocislo = 2, j, l;
    int prvocisla[100];
    int *output;
    int *hranice = (int *) (udaj); //novy pointer na int

    int Prve_prvocislo = *(hranice); //nahram dolnu hranicu
    int Posledne_prvocislo = *(hranice + 1); //nahram hornu hranicu
    for (i =  2; i <= Posledne_prvocislo;) //hladam prvocisla
    {
        if (prvocislo >= Posledne_prvocislo) //aby to skoncilo tak kde malo
            break;
        for (j = 2; j <= prvocislo; j++) {
            if (prvocislo % j == 0) // zistujem delitelnosti cisla
                break;
        }
        if (j == prvocislo) //
        {
            if (prvocislo >= Prve_prvocislo) //zapisovanie az od kedy by malo
            {
                prvocisla[k] = prvocislo; // zapisanie prvocisel do pola
                k++;
            }
            i++;
        }
        prvocislo++;
    }
    output = (int *) malloc(k * sizeof(int));
    if (output == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
    }
    for (l = 0; l < k; l++) {
        output[l] = prvocisla[l];
    }
    return (void *) output;
}

 void *sum(void *rec)
 {
  int *pole=(int*)rec;
  int *suma;
   suma = (int*)malloc(sizeof(int));
   suma[0]=0;
   int i;
   for (i=0;i<k;i++)
       suma[0] += pole[i];
     return (void*)suma;
 }



int main()
 {

    int id_klienta,dolna_hranica,horna_hranica,rozsah;
    int Dolna_podhranica,Horna_podhranica;
    int forkval;

    printf("Zadaj rozsah na vypocet, velkost rozsahu musi byt delitelna 4 a dolna hranica parna!\n");
    scanf("%d %d",&dolna_hranica,&horna_hranica);
    rozsah = horna_hranica - dolna_hranica;
    if( (rozsah % 4 == 0) && (dolna_hranica % 2 == 0)) //ci je splnena podmienka
    {
    forkval = fork();

    if(forkval == 0)
      {                            // proces na scitavanie
        //scitac
        exit(1);

      } else
          {

        forkval = fork();

        if (forkval == 0)
          {                      //proces na vypisanie

            //vypisovac
            exit(1);

          } else
              {

            for (id_klienta = 0; id_klienta < 4; id_klienta++)
            {                        //vytvaranie klientov

                forkval = fork();

                if (forkval == 0)
                {                    //aby sa nemnozili
                    break;
                }





            }

            if (forkval == 0)
            {                        //klienti

                Dolna_podhranica = id_klienta*(rozsah/4) + dolna_hranica+1;
                Horna_podhranica = id_klienta*(rozsah/4) + (rozsah/4) + dolna_hranica;

                       printf("Ja som child %d a mam interval %d az %d\n", id_klienta,Dolna_podhranica,Horna_podhranica);
                int udaje[2];
                udaje[0]=Dolna_podhranica;
                udaje[1]=Horna_podhranica;
                void *udaj;
                void *rec;
                void *rec2;
                int *message;
                udaj=(&udaje);
                pthread_t tid1;
                pthread_t tid2;
                pthread_create(&tid1, NULL, find ,(void*)udaj);//spustenie vlakna
                pthread_join(tid1,&rec);
                pthread_create(&tid2,NULL,sum,(void*)rec);
                pthread_join(tid2,&rec2);
                message=(int*)rec2;
                printf("vysledok: %d\n",*message);
                int messages_to_server = *message;
                //pipojenie na sockety..............................................................................

                    // vytvorenie socketu
                int sock_desc = socket(AF_INET,SOCK_STREAM,0);
                if (sock_desc == -1)
                {
                    printf("cannot create socket!\n");
                    return 0;
                }
                    //nastavenie socketu
                struct sockaddr_in client;
                memset(&client,0, sizeof(client));
                client.sin_family = AF_INET;
                client.sin_addr.s_addr = inet_addr("127.0.0.1");
                client.sin_port=htons(59836);
                    //pripojenie socketu
                if (connect(sock_desc,(struct sockaddr*)&client, sizeof(client)) != 0)
                {
                    printf("cannot conncet to server\n");
                    close(sock_desc);
                }
                int len = sizeof(messages_to_server);
                int sendbytes;
                int *p_buf;
                p_buf = messages_to_server;
                while (len > 0)
                {
                    sendbytes = send(sock_desc,p_buf,len,0); // posielanie dat
                    if (sendbytes == -1)
                    {
                        printf("cannot write to server\n");
                        break;
                    }
                    p_buf += sendbytes;
                    len -= sendbytes;
                }

                exit(1); //zabijanie

            } else
                {
                printf("\nJa som parent\n"); //server
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

                }

            //return 0;
                }
          }
    } else printf("Zadal si zly interval!\n");


    return 0;
}
