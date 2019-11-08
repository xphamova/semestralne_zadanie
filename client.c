#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{

    int id_klienta,dolna_hranica,horna_hranica,rozsah;
    int Dolna_podhranica,Horna_podhranica;
    int forkval;

    printf("Zadaj rozsah na vypocet, velkost rozsahu musi byt delitelna 4 a dolna hranica parna!\n");
    scanf("%d %d",&dolna_hranica,&horna_hranica);
    rozsah = horna_hranica - dolna_hranica;
    if( (rozsah % 4 == 0) && (dolna_hranica % 2 == 0))
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

                exit(1); //zabijanie

            } else
                {
                printf("\nJa som parent\n"); //server
                }

            //return 0;
                }
          }
    } else printf("Zadal si zly interval!\n");

    return 0;
}