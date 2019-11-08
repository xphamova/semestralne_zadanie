#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){

    int i;
    int forkval;

    forkval = fork();

    if(forkval == 0){

        //scitac
        exit(1);

    } else {

        forkval = fork();

        if (forkval == 0) {

            //vypisovac
            exit(1);

        } else {

            for (i = 0; i < 4; i++) {

                forkval = fork();

                if (forkval == 0) {

                    break;

                }

            }

            if (forkval == 0) {

                int Dh = i * 25;
                int Hh =

                        printf("Ja som child %d a mam interval %d az %d\n", i, i * 25, i * 25 + 25);

                exit(1);

            } else {

                printf("\nJa som parent\n");

            }

            return 0;
        }
    }
}