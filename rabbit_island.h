#ifndef RABBIT

#define RABBIT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mt19937ar.h"

// Probabilités
#define PROBA_PAR_MOIS_ANCIENNETE (PROBA_SURVIE_ADULTE / ((double)(MAX_AGE - MIN_AGE_ANCIEN)))
#define PROBA_SURVIE_ENFANT 0.9162324528
#define PROBA_SURVIE_ADULTE 0.9583245286
#define PROBA_NAISSANCE 1.

double proba_reproduction_par_mois[12] = 
//{ .6 , .6 , .6 , .6 , .6 , .6 , .6 , .6 , .6 , .6 , .6 , .6 };
{ .03, .02, .37, .69, .92, .89, .82, .75, .48, .32, .09, .03};
// J    F    M    A    M    Jn   Jl   A    S    O    N    D

#define age_adulte(index) (index+MIN_MATURITE_SEXUELLE)
#define MAX_NB_MOIS 12*20
#define INTERVALLE_MATURITE_SEXUELLE MAX_MATURITE_SEXUELLE - MIN_MATURITE_SEXUELLE + 1
#define MIN_MATURITE_SEXUELLE 5
#define MAX_MATURITE_SEXUELLE 8
#define MIN_AGE_ANCIEN 12*10

#define MAX_AGE_ADULTE MAX_AGE - MIN_MATURITE_SEXUELLE
#define MAX_AGE 12*15

#define MIN_PORTE_PAR_AN 4
#define MAX_PORTE_PAR_AN 8


typedef struct simu
{
    // méta données
    int temps;

    int total_lapin_vivant;
    int nb_femelle;
    int nb_male;
    int nb_enfant;
    int nb_porte_mois_suivant;


    // lapins ne pouvant pas encore se reproduire, leur sexe est choisis au passage adulte
    int lapin_enfant[MAX_MATURITE_SEXUELLE];

    int lapin_femelle[MAX_AGE_ADULTE];
    int lapin_male[MAX_AGE_ADULTE];



} simu;

void init_simu(simu* s, int nb_femelle, int nb_male, int nb_enfant, int nb_porte);
int mois_suivant(simu* s);
int veillir_lapin(int age);
int simulation(int max_temps);



// fonctions de test
int veillir_lapin_test();
int nombre_de_reproduction_par_an_test(int nb_exp);

// fonctions d'affichage
void print_simu(simu* s);
void print_simu_simple(simu* s);
void print_mois(int m);




#endif