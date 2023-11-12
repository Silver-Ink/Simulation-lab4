#include "rabbit_island.h"


int main(int argc, char* argv[])
{
    int t = 12;
    if (argc >= 2)
    {
        t = atoi(argv[1]);
    }
    printf("Debut : simulation pour %d mois\n", t);
    long long max = 0;

        long long s = simulation(t);
        if (s > max){max = s;}
    printf(" MAX = %d\n", max);

    return 0;
}

int simulation(int max_temps)
{
    simu s;
    init_simu(&s, 5, 5, 0, 0);
    // print_simu_simple(&s);
    int i = 0;
    while( i < max_temps && mois_suivant(&s))
    {
        print_simu_simple(&s);
        i++;
    }
    // print_simu(&s);
    return s.nb_enfant + s.nb_femelle + s.nb_femelle;

}


void init_simu(simu* s, int nb_femelle, int nb_male, int nb_enfant, int nb_porte)
{
    s->temps = 0;

    s->nb_porte_mois_suivant = nb_porte;


    for (int i = 0; i < MAX_MATURITE_SEXUELLE; i++)
    {
        s->lapin_enfant[i] = nb_enfant;
    }
    s->nb_enfant = nb_enfant * MAX_MATURITE_SEXUELLE;
    

    for (int i = 0; i < MAX_AGE_ADULTE; i++)
    {
        s->lapin_femelle[i] = 0;
        s->lapin_male[i] = 0;
    }
    s->lapin_femelle[0] = nb_femelle;
    s->nb_femelle = nb_femelle;

    s->lapin_male[0] = nb_male;
    s->nb_male = nb_male;
}

int mois_suivant(simu* s)
{

    // Si au moins un lapin ancore vivant
    if (s->nb_femelle > 0 || s->nb_enfant || s->nb_male)
    {
        // Mois de l'année en cours
        int mois = s->temps % 12;

        // Lapin adultes veillissent, en premier, sinon les nouvelles femelles vont veillir 2 fois.
        for (int i = MAX_AGE_ADULTE - 1; i > 0; i--)
        {
            // MAJ des compteurs
            s->nb_femelle         -= s->lapin_femelle[i-1];
            s->nb_male            -=    s->lapin_male[i-1];

            long long f = 0; long long m = 0;

            if (s->lapin_femelle[i-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_femelle[i-1]; j++)
                {
                    f += veillir_lapin(age_adulte(i));
                }
            }
            else
            {
                f = veillir_groupe_lapin(age_adulte(i), s->lapin_femelle[i-1]);
            }

            if (s->lapin_male   [i-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_male   [i-1]; j++)
                {
                    m += veillir_lapin(age_adulte(i));
                }
            }
            else
            {
                m = veillir_groupe_lapin(age_adulte(i), s->lapin_male   [i-1]);
            }
            
            s->lapin_femelle[i] = f;
            s->lapin_male   [i] = m;

            
            // MAJ des compteurs
            s->nb_femelle         += f;

            s->nb_male            += m;
        }
        s->lapin_femelle[0] = 0;
        s->lapin_male   [0] = 0;
        

        // Lapin enfants pouvant passer adulte veillissent
        for (int i = 1; i <= INTERVALLE_MATURITE_SEXUELLE; i++)
        {
            s->nb_enfant          -= s->lapin_enfant[MAX_MATURITE_SEXUELLE - i];

            long long adulte = 0;
            long long enfant = 0;
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

            long long femelle = 0;
            long long male    = 0;
            for (int j = 0; j < adulte; j++)
            {
                if (genrand_real1() < .5)
                { femelle++; }
                else
                { male++;    }
            }
            s->lapin_femelle[- i + INTERVALLE_MATURITE_SEXUELLE] += femelle;
            s->lapin_male   [- i + INTERVALLE_MATURITE_SEXUELLE] += male   ;

            s->nb_femelle += femelle;
            s->nb_male    += male;
            s->nb_enfant  += enfant;

        }


        // Les autres lapin enfants vivants veillissent
        for (int i = MIN_MATURITE_SEXUELLE - 1; i > 0; i--)
        {
            s->nb_enfant          -= s->lapin_enfant[i-1];

            long long enfant = 0;
            if (s->lapin_enfant[i-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_enfant[i-1]; j++)
                {
                    enfant += veillir_lapin(i);
                }
            }
            else
            {
                enfant = veillir_groupe_lapin(i, s->lapin_enfant[i-1]); 
            }
            s->lapin_enfant[i] = enfant;

            s->nb_enfant      += enfant;
        }
        
        
        

        // Naissance des lapereaux du mois précédent
        long long nouveau_ne = 0;
        for (int j = 0; j < s->nb_porte_mois_suivant; j++)
        {
            nouveau_ne += genrand_int32() % 4 + 3;
        }
        s->lapin_enfant[0] = nouveau_ne;

        s->nb_enfant += nouveau_ne;


        // Lapin adultes procréent
        long long nb_porte = 0;
        double proba_portee = proba_reproduction_par_mois[mois];
        for (int i = 0; i < MAX_AGE_ADULTE; i++)
        {

            if (s->lapin_femelle[i] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_femelle[i]; j++)
                {
                    double random = genrand_real1();
                    nb_porte += random < proba_portee;
                }
            }
            else
            {
                nb_porte += ((gen_rand_gaussienne() / ECART_TYPE) + proba_portee) * s->lapin_femelle[i];
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

double gen_rand_gaussienne()
{
    return cos(PI_2 * genrand_real2()) * sqrt(-log(genrand_real2()));
}

long long veillir_groupe_lapin(int age, long long nb)
{
    double random = gen_rand_gaussienne();
    random /= ECART_TYPE;
    

    if (age < MIN_MATURITE_SEXUELLE)
    {
        return (long long)((random + PROBA_SURVIE_ENFANT) * nb);
    }
    else if (age < MIN_AGE_ANCIEN)
    {
        return (long long)((random + PROBA_SURVIE_ADULTE) * nb);
    }
    else
    {
        double age_anciennete = age - MIN_AGE_ANCIEN;
        double proba = PROBA_SURVIE_ADULTE - age_anciennete * PROBA_PAR_MOIS_ANCIENNETE;
        return (long long)((random + proba) * nb);
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

void print_simu(simu* s)
{
    int mois = s->temps % 12;
    printf("\n=== mois num .%3d (%2d ans et %2d mois) ===\n", s->temps, s->temps / 12, mois);
    printf("e = \t%d \t porte mois s. = %d\nf = \t%d\nm = \t%d\n________\nT = \t%d\t",
            s->nb_enfant ,
            s->nb_porte_mois_suivant,
            s->nb_femelle,
            s->nb_male   ,
            s->nb_enfant + s->nb_femelle + s->nb_femelle);
    print_mois(mois);
    printf("  p_portee = %.2f\n", proba_reproduction_par_mois[mois]);
    
}

void print_simu_simple(simu* s)
{
    printf("%3d, %12lld ", s->temps, s->nb_enfant + s->nb_femelle + s->nb_femelle);
    print_mois(s->temps);
    printf("\n");
}

void print_mois(int m)
{
    m %= 12;
    switch (m)
    {
    case 0 : printf("Jan"); break;
    case 1 : printf("Fev"); break;
    case 2 : printf("Mar"); break;
    case 3 : printf("Avr"); break;
    case 4 : printf("Mai"); break;
    case 5 : printf("Jun"); break;
    case 6 : printf("Jul"); break;
    case 7 : printf("Aou"); break;
    case 8 : printf("Sep"); break;
    case 9 : printf("Oct"); break;
    case 10: printf("Nov"); break;
    case 11: printf("Dec"); break;
    }
}

