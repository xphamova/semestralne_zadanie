#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
sig_atomic_t volatile stop = 1;
void vypis();
void proces();



//1.vlakno...................................................................................................................................................
void * find(void *input) {


    int i, prvocislo = 2, j, l,k=0;
    int prvocisla[100];
    int *output;
    int *hranice = (int *) (input); //novy pointer na int

    int Prve_prvocislo = *(hranice); //nahram dolnu hranicu
    int Posledne_prvocislo = *(hranice + 1); //nahram hornu hranicu
    for (i =  2; i <= Posledne_prvocislo;) //hladam prvocisla
    {
        if (prvocislo >= Posledne_prvocislo+1) //aby to skoncilo tak kde malo
            break;
        for (j = 2; j <= prvocislo; j++) {
            if (prvocislo % j == 0) // zistujem delitelnosti cisla
                break;
        }
        if (j == prvocislo) //
        {
            if (prvocislo >= Prve_prvocislo-1) //zapisovanie az od kedy by malo
            {
                prvocisla[k] = prvocislo; // zapisanie prvocisel do pola
                k++;
            }
            i++;
        }
        prvocislo++;
       // printf("%d",k);
        key_t key = 27;
        int shmid_sec = shmget(key, sizeof(int),0666|IPC_CREAT);
        int *shm_sec = (int*)shmat(shmid_sec,NULL,0);
        int *s_sec;
        s_sec=shm_sec;
        *s_sec = k;



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
//2.vlakno........................................................................................................................................
 void *sum(void *rec)
 {
     key_t key= 28;
     int *s_sec;
     int shmid_sec = shmget(key, sizeof(int),0666);
     int *shm_sec;
     shm_sec = (int *) shmat(shmid_sec, NULL, 0);
     s_sec =  shm_sec;

  int *pole=(int*)rec;
  int *suma;
   suma = (int*)malloc(sizeof(int));
   suma[0]=0;
   int k = s_sec;
   //printf("%d",k);

   int i;
   for (i=0;i < k; i++)
       suma[0] += pole[i];


     return (void*)suma;
 }


//----------------------------------------------------------------------------------------------------------------------
int main()
 {

    int id_klienta,dolna_hranica,horna_hranica,rozsah,Dolna_podhranica,Horna_podhranica,forkval;




    printf("Zadaj rozsah na vypocet, velkost rozsahu musi byt delitelna 4!\n");
    scanf("%d %d",&dolna_hranica,&horna_hranica);
    rozsah = horna_hranica - dolna_hranica;
    if( (rozsah % 4 == 0) ) //ci je splnena podmienka
    {
        forkval = fork();
        int pid_v = forkval;
        if (forkval == 0)
          {                     //proces na vypisanie-------------------------------------------------------------------
          //  sleep(2);
                while (1)
              signal(SIGHUP,vypis);


           exit(1);
//.....................................................................................................................
          } else
              {

            int child_pid[4];
            for (id_klienta = 0; id_klienta < 4; id_klienta++)
            {                        //vytvaranie klientov
                forkval = fork();
                child_pid[id_klienta]=forkval;
                if (forkval == 0)
                {                    //aby sa nemnozili

                    break;
                }

            }

            if (forkval == 0)
            {                        //klienti_________________________________________________________________________

                Dolna_podhranica = id_klienta*(rozsah/4) + dolna_hranica+1;
                Horna_podhranica = id_klienta*(rozsah/4) + (rozsah/4) + dolna_hranica;

                  //     printf("Ja som child %d a mam interval %d az %d\n", id_klienta,Dolna_podhranica,Horna_podhranica);
                while(stop)
                    signal(SIGHUP,proces);
                int udaje[2];
                    udaje[0]=Dolna_podhranica;
                    udaje[1]=Horna_podhranica;
                void *udaj;
                void *rec;
                void *rec2;
                int *message;
                udaj=(&udaje);
                pthread_t tid1,tid2;
                    pthread_create(&tid1, NULL, find ,(void*)udaj);//spustenie vlakna
                        pthread_join(tid1,&rec);
                    pthread_create(&tid2,NULL,sum,(void*)rec);
                        pthread_join(tid2,&rec2);
                message=(int*)rec2;
                printf("vysledok %d: %d\n",id_klienta,*message);


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
                    printf("cannot connect to server\n");
                    close(sock_desc);
                }

                send(sock_desc,message, sizeof(message),0); // posielanie dat
                exit(1); //zabijanie

            } else
                {
               // printf("\nJa som parent\n"); //server____________________________________________________________
                    //vytvorenie socketu
                    kill(child_pid[0],SIGHUP);
                    int sock_desc = socket(AF_INET,SOCK_STREAM,0);
                    if(sock_desc == -1)
                    {
                        printf("cannot create socket!\n");
                        return 0;
                    }
                    //nastavenie socketu
                    struct sockaddr_in server;
                    memset(&server,0,sizeof(server)); //naplni nulami
                    server.sin_family = AF_INET; //aky
                    server.sin_addr.s_addr = INADDR_ANY; //s ktorej ip sa da prihlasit
                    server.sin_port = htons(59836); //nastavenie portu
                    if (bind(sock_desc, (struct sockaddr*)&server, sizeof(server)) != 0) // ked neprejde port je obsadeny
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
                    int client[4],id;
                    int client_response[4];
                    int *finally_sum;
                    finally_sum = (int*)malloc(sizeof(int));
                    finally_sum[0]=0;
                    for (id = 0;id < 4;id++)
                    {
                        client[id]= accept(sock_desc,NULL,NULL); //akceptovanie klientov
                        recv(client[id],&client_response[id], sizeof(client_response[id]),0); //pijatie sprav
                        finally_sum[0] += client_response[id];
                        kill(child_pid[id+1],SIGHUP);
                   //     printf("%d  ",client_response[id]);
                    }
                    printf("\nfinal result: %d \n",finally_sum[0]);
                    key_t key = 26;
                    int shmid = shmget(key, sizeof(int),0666|IPC_CREAT);
                    int *shm = (int*)shmat(shmid,NULL,0);
                   int *s;
                   s=shm;
                   *s = finally_sum[0];
                   kill(pid_v,SIGHUP);
                    close(sock_desc);
                    printf("server disconnected\n");
                    sleep(2);
                    exit(1);

                }
                }

    } else printf("Zadal si zly interval!\n");


    return 0;
}
void vypis()
{

   key_t key= 26;
    int *s;
    int shmid = shmget(key, sizeof(int),0666);
    int *shm;
    shm = (int *) shmat(shmid, NULL, 0);
    s =  shm;
    printf("Sucer prvocisel je: %d",*s);
    exit(1);
}

void proces()
{
    stop = 0;
}