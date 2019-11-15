//
// Created by juliana on 11/15/19.
//

int main()
{
    int k=0;prvocislo=2;Dolna_podhranica=2,Horna_podhranica=39,j,i;
    int prvocisla[];
    for (i=Dolna_podhranica;i<= Horna_podhranica;i++ )
    {
        for(j= Dolna_podhranica; j<= prvocislo,j++)
        {
            if(prvocislo%j==0)
                break;
        }
        if (j == prvocislo)
        {
            prvocisla[k] = provislo;
            k++;
            Dolna_podhranica++;
        }
        prvocislo++;
    }
    printf("%s",prvocisla)
    return 0;
}