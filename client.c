#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){

    int i;
    int forkval;

    forkval = fork();

    if(forkval == 0){ // proces na scitavanie

        //scitac
        exit(1);

    } else {

        forkval = fork();

        if (forkval == 0) { //proces na vypisanie

            //vypisovac
            exit(1);

        } else {

            for (i = 0; i < 4; i++) { //vytvaranie klientov

                forkval = fork();

                if (forkval == 0) { //aby sa nemnozili

                    break;

                }

            }

            if (forkval == 0) {  //klienti

                int Dh = i * 25;
                int Hh =

                        printf("Ja som child %d a mam interval %d az %d\n", i, i * 25, i * 25 + 25);

                exit(1); //zabijanie

            } else {

                printf("\nJa som parent\n"); //server

            }

            return 0;
        }
    }
}