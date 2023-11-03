#include <stdio.h>
#include <stdlib.h>
#include "rabbit_island.h"

int main()
{
    genrand_real1();
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

int mois_suivant(simu* s)
{

    // Si au moins un lapin ancore vivant
    if (s->total_lapin_vivant)
    {
        // Mois de l'année en cours
        int mois = s->temps % 12;

        // Lapin adultes veillissent, en premier, sinon les nouvelles femelles vont veillir 2 fois.
        for (int i = MAX_AGE_ADULTE - 1; i > 0; i--)
        {
            int age = i + 5;
            s->lapin_femelle[i] = s->lapin_femelle[i-1];
            s->lapin_male[i]    = s->lapin_male[i-1];
        }
        

        // Lapin enfants veillissent



        // Rectification du planning des naissances 

        // Naissance des lapereaux du mois précédent

        // Lapin adultes procréent


        s->temps++;
        return 1;
    }
    else
    { 
        s->temps++;
        return 0; 
    }

}

int veillir_lapin(int age)
{
    // genrand_real1()
}
