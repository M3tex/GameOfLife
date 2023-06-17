/**
 * @file affichage.h
 * @author M3tex
 * @brief Header pour affichage.c
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef AFFICHAGE_HEADER
#define AFFICHAGE_HEADER


#include "types.h"


// Pour les commandes du terminal en fonction des plateformes
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define CLEAR "cls templates/*.gol"
    #define LS "dir"
#else
    #define CLEAR "clear"
    #define LS "ls templates/*.gol"
#endif


// Constantes
static const unsigned int largeur_f = 800;
static const unsigned int hauteur_f = 800;


char file2grid(const char *fichier, Jeu *jeu, int x, int y);
void affiche_grille(Jeu *jeu);
void init_fichier(Jeu *jeu);
void init_terminal(Jeu *jeu);
void init_rdm(Jeu *jeu);
void init_GUI(Jeu *jeu);
void watch_events(SDL_Event *event, Jeu *jeu, char *gameloop, char estConfig);
void update_camera(Camera *cam);
void affiche_aide();
void affiche_commandes(Jeu *jeu, char estConfig);
void get_color(cellule cell, unsigned char *r, unsigned char *g, unsigned char *b);

void spectral_color(double *r,double *g,double *b,double l);


#endif
