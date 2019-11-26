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
#include <signal.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#include <wait.h>


volatile sig_atomic_t stop = 1;//Celočíselný typ, ku ktorému je možné pristupovať ako k atómovej entite aj za prítomnosti asynchrónnych prerušení vyvolaných signálmi

timer_t make_timer(int);
void start_timer(timer_t, int);

void write_function();

void process_start();
int sigwait(const sigset_t *set,int *sig);


//1.thread...................................................................................................................................................
void *find(void *input) {
    int i, j, l, count = 0, primes[100];
    int *output;
    int *interval = (int *) (input); //new pointer on int

    //shared memory
    key_t key = 26;
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *shm = (int *) shmat(shmid, NULL, 0);
    int *s;
    s = shm;

    int First_possible_prime = *(interval); //get  low range
    int Last_possible_prime = *(interval + 1); //get up range
    int prime = First_possible_prime;

    //find primes
    for (i = First_possible_prime; i <= Last_possible_prime; i++) {

        for (j = 2; j <= prime; j++) {
            if (prime % j == 0) // find  prime
                break;
        }
        if (j == prime) //test prime
        {
            primes[count] = prime; // write prime
            count++;
        }
        prime++;
    }

    *s = count; //write into shared memory

    //create dynamic memory
    output = (int *) malloc(count * sizeof(int));
    if (output == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
    }

    //write to dynamic memory
    for (l = 0; l < count; l++) {
        output[l] = primes[l];
        printf("%d ", primes[l]);
    }

    return (void *) output;
}

//2.thread........................................................................................................................................
void *sum(void *send_primes) {
    //shared memory
    key_t key = 26;
    int shmid = shmget(key, sizeof(int), 0666);
    int *shm;
    int *s;
    shm = (int *) shmat(shmid, NULL, 0);
    s = shm;

    int *pole_prime = (int *) send_primes;
    int *sum;
    sum = (int *) malloc(sizeof(int));
    sum[0] = 0;

    for (int i = 0; i < *s; i++)
        sum[0] += pole_prime[i];

    return (void *) sum;
}


//----------------------------------------------------------------------------------------------------------------------
int main() {

    int  low_range, up_range, range, client_low_range, client_up_range;
    int forkval;
    int child_pid[4];
    int id_client;


    printf("Zadaj rozsah na vypocet, velkost rozsahu musi byt delitelna 4!\n");
    scanf("%d %d", &low_range, &up_range);
    printf("prvocisla\n");

    range = up_range - low_range;
    if ((range % 4 == 0)) //check

    {

        forkval = fork();
        int pid_v = forkval;
        if (forkval == 0) {                     //write process-------------------------------------------------------------------------
            while (1)
            signal(SIGHUP, write_function);
            exit(0);


            //.......................................................................................
        } else {

            //make client
            for (id_client = 0; id_client < 4; id_client++) {
                child_pid[id_client]=  forkval= fork();

                if (forkval == 0) {                    //stop other process
                    forkval = 0;
                    break;
                }

            }

            if (forkval == 0) {
                //clients__________________________________________________________________________________________________________

                client_low_range = ((id_client) * (range / 4)) + low_range + 1;
                client_up_range = ((id_client) * (range / 4)) + (range / 4) + low_range;
               // printf("Ja som child %d  a mam interval %d az %d\n", id_client, client_low_range, client_up_range);
               // printf("aaaaaaaaa\n");
                //wait on server
               while (stop)
                   signal(SIGHUP,process_start);


                int interval_range[2];
                interval_range[0] = client_low_range;
                interval_range[1] = client_up_range;
                void *send_range;
                void *receive_primes;
                void *client_sum;
                int *message;
                send_range = (&interval_range);

                pthread_t tid1, tid2;
                pthread_create(&tid1, NULL, find, (void *) send_range);//start thread
                pthread_join(tid1, &receive_primes);
                pthread_create(&tid2, NULL, sum, (void *) receive_primes);
                pthread_join(tid2, &client_sum);
                message = (int *) client_sum;
                //      printf("vysledok %d: %d\n",id_client,*message);

                //connect on socket..................................................................................

                // make socket
                int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
                if (sock_desc == -1) {
                    printf("cannot create socket!\n");
                    return 0;
                }

                //settings socket
                struct sockaddr_in client;
                memset(&client, 0, sizeof(client));
                client.sin_family = AF_INET;
                client.sin_addr.s_addr = inet_addr("127.0.0.1");
                client.sin_port = htons(59837);

                //connect socket
                if (connect(sock_desc, (struct sockaddr *) &client, sizeof(client)) != 0) {
                    printf("cannot connect to server\n");
                    close(sock_desc);
                }

                //send message

                send(sock_desc, message, sizeof(message), 0);

                exit(1); //kill

            } else {
                //server__________________________________________________________________________________________________________



                //make socket

                int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
                if (sock_desc == -1) {
                    printf("cannot create socket!\n");
                    return 0;
                }

                //setting socket
                struct sockaddr_in server;
                memset(&server, 0, sizeof(server)); //filling zero
                server.sin_family = AF_INET; //aky
                server.sin_addr.s_addr = INADDR_ANY; //who can connect
                server.sin_port = htons(59837); //setting port
                //bind
              //  printf("ni");
                if (bind(sock_desc, (struct sockaddr *) &server, sizeof(server)) != 0) // true - busy port
                {
                    printf("cannot bind socket!\n");
                    close(sock_desc);
                    return 0;
                }
                //listen
                if (listen(sock_desc, 4) != 0) {
                    printf("cannot listen on socket!\n");
                    close(sock_desc);
                    return 0;
                }
               // printf("nieco");
              //   printf("poslal");
                int accept_client[6], id;
                int client_response[6];
                int *finally_sum;
                finally_sum = (int *) malloc(sizeof(int));
                finally_sum[0] = 0;


                //accept,recv


                for (id = 0; id < 4; id++) {
                    kill(child_pid[id], SIGHUP);
                    accept_client[id] = accept(sock_desc, NULL, NULL); //accept client
                 //   printf("acce\n");
                    recv(accept_client[id], &client_response[id], sizeof(client_response[id]), 0); //received message
                    finally_sum[0] += client_response[id];//sum
              //      kill(child_pid[id+1], SIGHUP);//start other clients
                    //     printf("%d  ",client_response[id]);
                }

                // printf("\nfinal result: %d \n",finally_sum[0]);

                //shared memory
                key_t key = 26;
                int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
                int *shm = (int *) shmat(shmid, NULL, 0);
                int *s;
                s = shm;
                *s = finally_sum[0];

                //start write process
                kill(pid_v, SIGHUP);
                close(sock_desc);

                // printf("server disconnected\n");
                timer_t timer;
                timer = make_timer(SIGKILL);//id
                start_timer(timer, 5);
                while (1) {
                    sleep(1);
                    printf("\nCakam na siqkill");
                }
                exit(1);
            }
        }

    } else printf("Zadal si zly interval!\n");

    return 0;
}

void write_function() {
    //shared memory
    key_t key = 26;
    int *s;
    int shmid = shmget(key, sizeof(int), 0666);
    int *shm;
    shm = (int *) shmat(shmid, NULL, 0);
    s = shm;

    printf("\nSucet prvocisel je: %d", *s);
    shmctl(shmid, IPC_RMID, NULL);
    exit(1);

}


void process_start()
{
    signal(SIGHUP,process_start);
    stop = 0;
}

timer_t make_timer(int signal) {
    struct sigevent where;
    where.sigev_notify = SIGEV_SIGNAL; //SIGEV_SIGNAl upon timer expiration, generate the signal sigev_signo
    where.sigev_signo = signal;

    timer_t timer;
    timer_create(CLOCK_REALTIME, &where, &timer);
    return (timer);//id
}

void start_timer(timer_t timer, int seconds) {
    struct itimerspec time;
    time.it_value.tv_sec = seconds;
    time.it_value.tv_nsec = 0;
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_nsec = 0;
    timer_settime(timer, CLOCK_REALTIME, &time, NULL);
}