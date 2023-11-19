#include "rabbit_island.h"


int main(int argc, char* argv[])
{
    // Permier argument, nombre de mois par simulation
    int t = 240;
    if (argc >= 2)
    {
        t = atoi(argv[1]);
    }

    // Deuxieme argument, nombre de simulations
    int nb_sim = 100;
    if (argc >= 3)
    {
        nb_sim = atoi(argv[2]);
    }

    printf("Debut : simulation pour %d mois\n", t);

    #define NB_BIN 100
    entier total = 0;
    entier bin[NB_BIN]; for (int i = 0; i < NB_BIN; i++) { bin[i] = 0;}

    entier pas = (test_moyenne(100, t)  * 2.3) / NB_BIN;
    printf("pas de %llu Mds\n", pas);

    simu max;
    for (int i = 0; i < nb_sim; i++)
    {
        simu s = simulation(t);
        // print_simu_simple(&s);
        entier valeur = get_total_lapin(&s) / ORDRE_GRANDEUR;
        total += valeur;
        bin[valeur / pas]++;
        // printf("%llu\n", valeur);
    }


    for (int i = 0; i < NB_BIN; i++)
    {
        printf("%llu,%llu\n", i*pas, bin[i]);
    }
    printf("moyenne = %llu\n", total / nb_sim);
    
    return 0;
}

entier test_moyenne(int nb_simu, int t)
{

    entier total = 0;
    for (int i = 0; i < nb_simu; i++)
    {
        simu s = simulation(t);
        // print_simu_simple(&s);
        entier valeur = get_total_lapin(&s) / ORDRE_GRANDEUR;
        total += valeur;
    }
    return total / nb_simu;
}

/// @brief Effectue une simulation pour le temps donné ou jusqu'à ce que l'escpèce disparaisse
/// @param max_temps temps de simulation maximal
/// @return simulation terminée
simu simulation(int max_temps)
{
    simu s;
    init_simu(&s, 5, 5, 0, 0);
    int i = 0;
    while( i < max_temps && mois_suivant(&s))
    {
        // print_simu_simple(&s);
        i++;
    }
    // print_simu(&s);
    return s;

}
/// @brief calcule le nombre total de lapins vivants
/// @param s simulation cible
/// @return total de lapins
entier get_total_lapin(simu* s)
{
    return s->nb_enfant + s->nb_femelle + s->nb_male;
}

/// @brief Initialise une simulation a 0, 
/// en précisant divers paramètres sur la population initiale
/// @param s simulation a initialiser
/// @param nb_femelle nombre de femelles initial (age = MIN_MATURITE_SEXUELLE)
/// @param nb_male nombre de males initial (age = MIN_MATURITE_SEXUELLE)
/// @param nb_enfant nombre d'enfants de chaque tranche d'age
/// @param nb_porte nombre de portées pour le mois prochain
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

/// @brief Avance une simulation d'un mois
/// @param s simulation a faire avancer
/// @return 1 si la simulation est un succés, 0 si plus l'espèce est éteinte :(
int mois_suivant(simu* s)
{

    // Si au moins un lapin ancore vivant
    if (s->nb_femelle > 0 || s->nb_enfant || s->nb_male)
    {
        // Mois de l'année en cours
        int mois = s->temps % 12;

        // Lapin adultes veillissent, en premier, sinon les nouvelles femelles vont veillir 2 fois.
        for (int age = MAX_AGE_ADULTE - 1; age > 0; age--)
        {
            // MAJ des compteurs
            s->nb_femelle         -= s->lapin_femelle[age-1];
            s->nb_male            -=    s->lapin_male[age-1];

            entier f = 0; entier m = 0;

            if (s->lapin_femelle[age-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_femelle[age-1]; j++)
                {
                    f += veillir_lapin(age_adulte(age));
                }
            }
            else
            {
                f = veillir_groupe_lapin(age_adulte(age), s->lapin_femelle[age-1]);
            }

            if (s->lapin_male   [age-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_male   [age-1]; j++)
                {
                    m += veillir_lapin(age_adulte(age));
                }
            }
            else
            {
                m = veillir_groupe_lapin(age_adulte(age), s->lapin_male   [age-1]);
            }
            
            s->lapin_femelle[age] = f;
            s->lapin_male   [age] = m;

            
            // MAJ des compteurs
            s->nb_femelle         += f;

            s->nb_male            += m;
        }
        s->lapin_femelle[0] = 0;
        s->lapin_male   [0] = 0;
        

        // Lapin enfants pouvant passer adulte veillissent
        for (int age = 1; age <= INTERVALLE_MATURITE_SEXUELLE; age++)
        {
            // Parcours des ages dans le sens décroissant
            s->nb_enfant          -= s->lapin_enfant[MAX_MATURITE_SEXUELLE - age];

            entier adulte = 0;
            entier enfant = 0;
            double proba_adulte = 1. / (double)age;

            if (s->lapin_enfant[MAX_MATURITE_SEXUELLE - age] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_enfant[MAX_MATURITE_SEXUELLE - age]; j++)
                {
                    if (veillir_lapin(MAX_MATURITE_SEXUELLE - age))
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
            }
            else
            {
                double ratio_survivants = gen_rand_gaussienne(ECART_TYPE, PROBA_SURVIE_ENFANT);
                if (ratio_survivants < 0) {ratio_survivants = 0;}
                if (ratio_survivants > 1) {ratio_survivants = 1;}
                double ratio_adulte = gen_rand_gaussienne(ECART_TYPE, proba_adulte);
                if (ratio_adulte < 0) {ratio_adulte = 0;}
                if (ratio_adulte > 1) {ratio_adulte = 1;}
                // printf("%2.5f %2.5f\n",ratio_survivants, ratio_adulte);

                entier survivants = s->lapin_enfant[MAX_MATURITE_SEXUELLE - age] * ratio_survivants;
                adulte = survivants * ratio_adulte;
                enfant = survivants - adulte;
            }

            

            if (age != 1)
            {
                s->lapin_enfant[MAX_MATURITE_SEXUELLE - age + 1] = enfant;
            }

            entier femelle = 0;
            entier male    = 0;

            if (adulte < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < adulte; j++)
                {
                    if (genrand_real1() < .5)
                    { femelle++; }
                    else
                    { male++;    }
                }
            }
            else
            {
                double ratio_fm = gen_rand_gaussienne( ECART_TYPE , 0.5);
                femelle = ratio_fm * adulte ;
                male    = adulte   - femelle;
            }

            s->lapin_femelle[- age + INTERVALLE_MATURITE_SEXUELLE] += femelle;
            s->lapin_male   [- age + INTERVALLE_MATURITE_SEXUELLE] += male   ;

            s->nb_femelle += femelle;
            s->nb_male    += male;
            s->nb_enfant  += enfant;

        }


        // Les autres lapin enfants vivants veillissent
        for (int age = MIN_MATURITE_SEXUELLE - 1; age > 0; age--)
        {
            s->nb_enfant -= s->lapin_enfant[age-1];

            entier enfant = 0;
            if (s->lapin_enfant[age-1] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_enfant[age-1]; j++)
                {
                    enfant += veillir_lapin(age);
                }
            }
            else
            {
                enfant = veillir_groupe_lapin(age, s->lapin_enfant[age-1]); 
            }
            s->lapin_enfant[age] = enfant;
            s->nb_enfant        += enfant;
        }
        
        
        

        // Naissance des lapereaux du mois précédent
        entier nouveau_ne = 0;

        if (s->nb_porte_mois_suivant < MAX_SIMU_COMPLETE)
        {
            for (int j = 0; j < s->nb_porte_mois_suivant; j++)
            {
                nouveau_ne += genrand_int32() % (int)INTERVALLE_ENFANT_PAR_PORTE + MIN_ENFANT_PAR_PORTE;
            }
        }
        else
        {
            double ratio_portee = gen_rand_gaussienne(ECART_TYPE, MIN_ENFANT_PAR_PORTE + INTERVALLE_ENFANT_PAR_PORTE / 2.) ;
            // if (ratio_portee < MIN_ENFANT_PAR_PORTE) {ratio_portee = MIN_ENFANT_PAR_PORTE;}
            // if (ratio_portee > MAX_ENFANT_PAR_PORTE) {ratio_portee = MAX_ENFANT_PAR_PORTE;}
            // printf("ratio par portee : %f\n",  ratio_portee);
            nouveau_ne = ratio_portee * s->nb_porte_mois_suivant;

        }
        s->lapin_enfant[0] = nouveau_ne;

        s->nb_enfant += nouveau_ne;


        // Lapin adultes procréent
        entier nb_porte = 0;
        double proba_portee = proba_reproduction_par_mois[mois];
        for (int age = 0; age < MAX_AGE_ADULTE; age++)
        {

            if (s->lapin_femelle[age] < MAX_SIMU_COMPLETE)
            {
                for (int j = 0; j < s->lapin_femelle[age]; j++)
                {
                    double random = genrand_real1();
                    nb_porte += random < proba_portee;
                }
            }
            else
            {
                nb_porte += gen_rand_gaussienne(ECART_TYPE, proba_portee)  * s->lapin_femelle[age];
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

/// @brief veillis un lapin selon son age
/// @param age determine la probabilité que le lapin sorte de cette fonction vivant
/// @return 1 si le lapin est vivant, sinon 0
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

/// @brief Génére un nombre floatant aléatoire réparti selon
///      une gaussienne selon la méthode de Box et Muller
/// @return nombre aléatoire d'une gaussienne centrée et réduite
double gen_rand_gaussienne(double ecart_type, double moyenne)
{
    return (cos(PI_2 * genrand_real2()) * sqrt(-log(genrand_real2()))) / ecart_type + moyenne;
}

/// @brief Veillis un nombre de lapin d'un coup, selon une  répartition de gaussienne
/// @param age age du groupe de lapins
/// @param nb taille du groupe de lapin a faire veillir
/// @return nombre de lapin aillant survécus à la veilliesse (plus pour longtemps)
entier veillir_groupe_lapin(int age, entier nb)
{
    if (age < MIN_MATURITE_SEXUELLE)
    {
        return (entier)((gen_rand_gaussienne(ECART_TYPE, PROBA_SURVIE_ENFANT) ) * nb);
    }
    else if (age < MIN_AGE_ANCIEN)
    {
        return (entier)((gen_rand_gaussienne(ECART_TYPE, PROBA_SURVIE_ADULTE)) * nb);
    }
    else
    {
        double age_anciennete = age - MIN_AGE_ANCIEN;
        double proba = PROBA_SURVIE_ADULTE - age_anciennete * PROBA_PAR_MOIS_ANCIENNETE;
        return (entier)((gen_rand_gaussienne(ECART_TYPE, proba)) * nb);
    }
}

// /// @brief Permet de tester le bon fonctionnement de 
// /// @return 
// int veillir_lapin_test()
// {
//     for (int age = 0; age < MAX_AGE + 12; age++)
//     {
//         double proba;
//         if      (age < MIN_MATURITE_SEXUELLE)
//         {
//             proba = PROBA_SURVIE_ENFANT;
//         }
//         else if (age < MIN_AGE_ANCIEN)
//         {
//             proba = PROBA_SURVIE_ADULTE;
//         }
//         else
//         {
//             double age_anciennete = age - MIN_AGE_ANCIEN ;
//             proba = PROBA_SURVIE_ADULTE - age_anciennete * PROBA_PAR_MOIS_ANCIENNETE;
//         }
//         printf("Proba de survie pour un lapin de %2d ans et %2d mois : %f\n", age/12, age%12, proba);
//     }   
// }

/// @brief Permet de tester en donnant la moyenne sur multiples tirage,
/// des probabilités de reproduction par mois (proba_reproduction_par_mois)
/// @param nb_exp nombre de tirages
/// @return 
void nombre_de_reproduction_par_an_test(int nb_exp)
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
    printf("Moyenne de %2.3f sur %d experiences\n",
     (double)somme / (double)nb_exp, nb_exp);
}

/// @brief Affiche une version détaillée de la simulation
/// @param s simulation a afficher
void print_simu(simu* s)
{
    int mois = s->temps % 12;
    printf("\n=== mois num .%3d (%2d ans et %2d mois) ===\n", s->temps, s->temps / 12, mois);
    printf("e = \t%d \t porte mois s. = %d\nf = \t%d\nm = \t%d\n________\nT = \t%12llu\t",
            s->nb_enfant ,
            s->nb_porte_mois_suivant,
            s->nb_femelle,
            s->nb_male   ,
            s->nb_enfant + s->nb_femelle + s->nb_femelle);
    print_mois(mois);
    printf("  p_portee = %.2f\n", proba_reproduction_par_mois[mois]);
    
}

/// @brief Affiche le nombre de mois, le nombre total de lapins à cet instant,
/// le mois de l'année, et le nombre de Milliards de lapins.
/// @param s simulation a afficher
void print_simu_simple(simu* s)
{
    entier total = s->nb_enfant + s->nb_femelle + s->nb_femelle;
    printf("%3d, %20llu ", s->temps, total);
    print_mois(s->temps);
    printf("\t(%10llu Mds)", total / 1000000000);
    printf("\n");
}

/// @brief Affiche le mois de l'année correspondant à la date donnée
/// @param m date en mois a afficher
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

        