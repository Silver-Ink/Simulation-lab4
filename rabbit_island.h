#ifndef RABBIT
#include "mt19937ar.h"

#define RABBIT

#define MAX_NB_MOIS 12*20
#define MIN_MATURITE_SEXUELLE 5
#define MAX_MATURITE_SEXUELLE 8

#define MAX_AGE_ADULTE MAX_AGE - MIN_MATURITE_SEXUELLE
#define MAX_AGE 12*15

#define MIN_PORTE_PAR_AN 4
#define MAX_PORTE_PAR_AN 8

// Probabilités
#define PORBA_SURVIE_ENFANT 35 
#define PORBA_SURVIE_ADULTE 60


typedef struct simu
{
    // méta données
    int temps;

    int total_lapin_vivant;
    int total_lapin_cumul;
    int nb_femelle;
    int nb_male;


    // lapins ne pouvant pas encore se reproduire, leur sexe est choisis au passage adulte
    int lapin_enfant[MAX_MATURITE_SEXUELLE];

    int lapin_femelle[MAX_AGE_ADULTE];
    int lapin_male[MAX_AGE_ADULTE];

    // l'index indique dans combien de moi le lapin naitra
    // à l'index 0 se trouve les lapereaux naissant au mois suivant
    int planning_naissances[12];

} simu;

void init_simu(simu* s, int nb_femelle, int nb_male, int nb_enfant);
int mois_suivant(simu* s);
int veillir_lapin(int age);


#endif