#include <stdio.h>
#include <stdlib.h>
#include "rabbit_island.h"

int main()
{

    return 0;
}


void init_simu(simu* s, int nb_femelle, int nb_male, int nb_enfant)
{
    s->temps = 0;

    s->total_lapin_vivant = nb_enfant + nb_femelle + nb_male;
    s->total_lapin_cumul = s->total_lapin_vivant;


    for (int i = 0; i < MAX_MATURITE_SEXUELLE; i++)
    {
        s->lapin_enfant[i] = 0;
    }
    s->lapin_enfant[0] = nb_enfant;
    

    for (int i = 0; i < MAX_AGE_ADULTE; i++)
    {
        s->lapin_femelle[i] = 0;
        s->lapin_male[i] = 0;
    }
    s->lapin_femelle[0] = nb_femelle;
    s->lapin_male[0] = nb_male;
}

