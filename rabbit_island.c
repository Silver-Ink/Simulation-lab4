#include <stdio.h>
#include <stdlib.h>
#include "rabbit_island.h"
#include "mt19937ar.h"


int main()
{
    simu s;
    init_simu(&s, 1000, 1000, 0);
    mois_suivant(&s);
    printf("->%d\n", s.nb_porte_mois_suivant);

    return 0;
}


void init_simu(simu* s, int nb_femelle, int nb_male, int nb_enfant)
{
    s->temps = 0;

    s->total_lapin_vivant = nb_enfant + nb_femelle + nb_male;
    s->total_lapin_cumul = s->total_lapin_vivant;
    s->nb_porte_mois_suivant = 0;


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
            int f = 0;
            int m = 0;
            for (int j = 0; j < s->lapin_femelle[i-1]; j++)
            {
                f += veillir_lapin(age_adulte(i));
            }
            for (int j = 0; j < s->lapin_male[i-1]; j++)
            {
                m += veillir_lapin(age_adulte(i));
            }
            
            s->lapin_femelle[i] = f;
            s->lapin_male[i]    = m;
        }
        

        // Lapin enfants pouvant passer adulte veillissent
        for (int i = 1; i <= INTERVALLE_MATURITE_SEXUELLE; i++)
        {


            int adulte = 0;
            int enfant = 0;
            double proba_adulte = 1. / (double)i;
            for (int j = 0; j < s->lapin_enfant[MAX_MATURITE_SEXUELLE - i]; j++)
            {
                if (veillir_lapin(MAX_MATURITE_SEXUELLE - i))
                {
                    if (proba_adulte != 1.)
                    {
                        double random_adulte = genrand_real1();
                        if (random_adulte < proba_adulte)
                        {adulte++;}
                        else
                        {enfant++;}

                    }
                    else
                    {
                        adulte++;
                    }
                }
            }
            if (i != 1)
            {
                s->lapin_enfant[MAX_MATURITE_SEXUELLE - i + 1] = enfant;
            }



            int femelle = 0;
            int male    = 0;
            for (int j = 0; j < adulte; j++)
            {
                if (genrand_real1() < .5)
                { femelle++; }
                else
                { male++;    }
            }
            s->lapin_femelle[- i + INTERVALLE_MATURITE_SEXUELLE] += femelle;
            s->lapin_male   [- i + INTERVALLE_MATURITE_SEXUELLE] += male   ;
        }



        for (int i = MIN_MATURITE_SEXUELLE - 2; i >= 0; i--)
        {
            int enfant = 0; 
            for (int j = 0; j < s->lapin_enfant[i]; j++)
            {
                enfant += veillir_lapin(i);
            }
            s->lapin_enfant[i+1] = enfant;
        }
        
        
        

        // Naissance des lapereaux du mois précédent
        int nouveau_ne = 0;
        for (int j = 0; j < s->nb_porte_mois_suivant; j++)
        {
            nouveau_ne += genrand_int32() % 4 + 3;
        }
        s->lapin_enfant[0] = nouveau_ne;


        // Lapin adultes procréent
        int nb_porte = 0;
        double proba_portee = proba_reproduction_par_mois[mois];
        for (int i = 0; i < MAX_AGE_ADULTE; i++)
        {
            // Rajouter un condition sur les males
            for (int j = 0; j < s->lapin_femelle[i]; j++)
            {
                double random = genrand_real1();
                nb_porte += random < proba_portee;
            }
        }
        s->nb_porte_mois_suivant = nb_porte;


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
    double random = genrand_real1();
    if      (age < MIN_MATURITE_SEXUELLE)
    {
        return random < PROBA_SURVIE_ENFANT;
    }
    else if (age < MIN_AGE_ANCIEN)
    {
        return random < PROBA_SURVIE_ADULTE;
    }
    else
    {
        double age_anciennete = age - MIN_AGE_ANCIEN;
        return random < PROBA_SURVIE_ADULTE - age_anciennete * PROBA_PAR_MOIS_ANCIENNETE;
    }
}

int veillir_lapin_test()
{
    for (int age = 0; age < MAX_AGE + 12; age++)
    {
        double proba;
        if      (age < MIN_MATURITE_SEXUELLE)
        {
            proba = PROBA_SURVIE_ENFANT;
        }
        else if (age < MIN_AGE_ANCIEN)
        {
            proba = PROBA_SURVIE_ADULTE;
        }
        else
        {
            double age_anciennete = age - MIN_AGE_ANCIEN ;
            proba = PROBA_SURVIE_ADULTE - age_anciennete * PROBA_PAR_MOIS_ANCIENNETE;
        }
        printf("Proba de survie pour un lapin de %2d ans et %2d mois : %f\n", age/12, age%12, proba);
    }   
}

int nombre_de_reproduction_par_an_test(int nb_exp)
{
    int somme = 0;
    for (int e = 0; e < nb_exp; e++)
    {
        int p = 0;
        for (int i = 0; i < 12; i++)
        {
            double random = genrand_real1();
            p += random < proba_reproduction_par_mois[i];
        }
        printf("%d reproduction cette annee.\n", p);
        somme += p;
    }
    printf("Moyenne de %2.3f sur %d experiences\n", (double)somme / (double)nb_exp,
                                                    nb_exp);
}

