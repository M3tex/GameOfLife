/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "utilitaires.h"
#include "logique.h"
#include "affichage.h"
#include "types.h"




int main(int argc, char **argv)
{
    // On vérifie les arguments
    if (!(argc == 2 && strlen(argv[1]) == 2 && argv[1][0] == '-'))
    {
        affiche_aide();
    }

    unsigned int taille_max = min_uint(largeur_f, hauteur_f);

    // On demande à l'utilisateur la taille n de la grille
    unsigned int n = get_uint("Quelle taille pour la grille ?");
    if (n > taille_max) n = taille_max;

    // Et s'il veut un nombre de tour limite
    int nb_tours = get_int("Combien de tour maximum (-1 si pas de limite de tours) ?");

    // On vérifie que SDL s'initialise bien
    if (SDL_VideoInit(NULL) != 0 || SDL_Init(0) != 0)
    {
        printf("Error initializing SDL video:  %s\n", SDL_GetError());
        return 1;
    }

    Jeu *jeu = init_jeu(n, largeur_f, hauteur_f);


    // On utilise l'initialisation choisie par l'utilisateur
    if (argv[1][1] == 'f')
    {
        init_fichier(jeu);
    }
    else if (argv[1][1] == 't')
    {
        init_terminal(jeu);
    }
    else if (argv[1][1] == 'r')
    {
        init_rdm(jeu);
    }
    else    // Peu importe la lettre même si pas -g on lance avec le GUI
    {
        init_GUI(jeu);
    }

    /* Pas besoin d'initialiser de fenetre et renderer car déjà fait dans init_GUI().
    init_GUI() est appelée par init_fichier(), init_rdm() et init_terminal() si ces modes 
    d'initialisation sont choisis par l'utilisateur.
    On est donc assurés que jeu -> fenetre et jeu -> renderer seront bien initialisés.
    
    Je change le titre de la fenetre au cas où SDL_asprintf ne soit pas défini sur vos machines */
    SDL_SetWindowTitle(jeu -> fenetre, "Game of Life (asprintf() non définie sur votre machine)");

    // On met à jour les stats et on affiche les commandes.
    jeu -> statistiques->nb_cell_originelles = jeu -> statistiques -> nb_cellules_depart;
    jeu -> statistiques -> en_vie = jeu -> statistiques -> nb_cellules_depart;
    affiche_commandes(jeu, 0);

    // On lance la boucle de jeu
    char gameloop = 1;
    unsigned long int generation = 0;
    while (gameloop)
    {   
        /* Je pense avoir réussi à détecter si asprintf était défini.
        Si ça ne marche pas, supprimez les 6 lignes suivantes. */
        char *gen_nb_str;
        asprintf(&gen_nb_str, "Game of Life: Génération n°%lu    (Délai: %ums)", generation, jeu -> delay_ms);
        SDL_SetWindowTitle(jeu -> fenetre, gen_nb_str);
        free(gen_nb_str);
        
        // On regarde les évènements (touches pressées etc)
        SDL_Event event;
        watch_events(&event, jeu, &gameloop, 0);

        jeu -> estPause = (nb_tours != -1) ? (jeu -> estPause || generation > nb_tours - 1) : jeu -> estPause;

        // Si le jeu n'est pas en pause on calcule l'itération suivante
        if (!(jeu -> estPause))
        {
            // On calcule l'état du jeu
            maj_grille(jeu);
            generation++;
        }

        affiche_grille(jeu);

        // On attend X ms avant de passer à l'itération suivante
        SDL_Delay(jeu -> delay_ms);
    }
    jeu -> statistiques -> generations = generation;

    SDL_Quit(); // On quitte la SDL
    system(CLEAR);
    affiche_stats(jeu -> statistiques);

    // On libère toute la mémoire et on quitte.
    free_jeu(jeu);
    
    return 0;
}
