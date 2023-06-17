/**
 * @file types.c
 * @author M3tex
 * @brief Contient toutes les fonctions permettant
 * d'initialiser / détruire les types définis dans types.h
 * @version 0.1
 * @date 2022-11-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include "utilitaires.h"



/**
 * @brief Initialise la caméra avec les données saisies par l'utilisateur.
 * 
 * @param taille La taille saisie par l'utilisateur.
 * @param taille_max La taille maximale, i.e la taille de la grille complète.
 */
Camera *init_camera(unsigned int taille, unsigned int taille_max)
{
    Camera *cam = (Camera *) malloc(sizeof(Camera));
    if (cam == NULL) quitter("Impossible d'allouer de la mémoire pour la Caméra\n", 2);

    cam -> width = taille;
    cam -> max_width = taille_max;
    
    // On centre la caméra dans la grille
    cam -> centre_x = taille_max / 2;
    cam -> centre_y = taille_max / 2;
    cam -> origin_x = cam -> centre_x - (taille / 2);
    cam -> origin_y = cam -> centre_y - (taille / 2);
    return cam;
}




/**
 * @brief Initialise la structure Jeu.
 * 
 * @param taille_choisie La taille choisie par l'utilisateur (définit le zoom
 * initial).
 * @param largeur_f La largeur de la fenêtre 
 * @param hauteur_f La hauteur de la fenêtre
 * @return Jeu Une instance de la structure jeu
 */
Jeu *init_jeu(unsigned int taille_choisie, unsigned int largeur_f, unsigned int hauteur_f)
{
    Jeu *jeu = (Jeu *) malloc(sizeof(Jeu));
    if (jeu == NULL) quitter("Impossible d'allouer de la mémoire pour le Jeu\n", 2);

    jeu -> grille = init_grille(min_uint(largeur_f, hauteur_f));
    jeu -> cam = init_camera(taille_choisie, jeu -> grille -> taille);

    jeu -> statistiques = init_stats();

    // On initialisera dans init_GUI()
    jeu -> fenetre = NULL;
    jeu -> renderer = NULL;

    jeu -> estPause = 0;
    jeu -> estCouleur = 0;
    jeu -> estQuadrille = 0;
    jeu -> delay_ms = 50;
    jeu -> largeur_cell = (jeu -> grille -> taille) / taille_choisie;
    return jeu;
}



/**
 * @brief Permet d'initialiser une matrice carrée de cellules.
 * Met toutes les cellules à 0.
 * Attention: il faudra libérer la mémoire allouée.
 *
 * @param taille La taille de la matrice
 * @return unsigned int** Un pointeur sur la matrice créée
 */
cellule **init_matrice(unsigned int taille)
{
    // On alloue de la mémoire
    cellule **grille = (cellule **) malloc(sizeof(cellule *) * taille);
    if (grille == NULL) quitter("Impossible d'allouer de la mémoire pour la matrice", 1);

    for (unsigned int i = 0; i < taille; i++)
    {
        grille[i] = (cellule *) calloc(taille, sizeof(cellule));
        if (grille[i] == NULL) quitter("Impossible d'allouer de la mémoire pour la matrice", 1);
    }

    return grille;
}




/**
 * @brief Initialise une instance de la struct Grille.
 * 
 * @param taille La taille maximale de la grille (= taille
 * de la fenetre)
 * @return Grille* Un pointeur sur une Grille
 */
Grille *init_grille(unsigned int taille)
{
    Grille *result = (Grille *) malloc(sizeof(Grille));
    if (result == NULL) quitter("Impossible d'allouer de la mémoire pour la grille", 1);

    result -> matrice = init_matrice(taille);
    result -> taille = taille;
    return result;
}




/**
 * @brief Initialise une instance de la struct Stats
 * 
 * @param nb_cell_debut Le nombre de cellules en vie avant le 
 * début du jeu
 * @return Stats* Un pointeur sur une Stats
 */
Stats *init_stats()
{
    Stats *to_return = (Stats *) malloc(sizeof(Stats));
    if (to_return == NULL) quitter("Impossible d'allouer de la mémoire pour les Stats\n", 2);

    to_return -> nb_cell_mortes = 0;
    to_return -> nb_cell_nes = 0;
    to_return -> nb_cell_originelles = 0;
    to_return -> nb_cellules_depart = 0;
    to_return -> en_vie = 0;
    return to_return;
}



/**
 * @brief Permet de copier une grille
 *
 * @param grille Le pointeur sur la grille a copier
 * @return Grille* Le pointeur vers la nouvelle grille
 */
Grille *copie_grille(Grille *grille)
{
    // + lisible
    unsigned int taille = grille -> taille;
    cellule **matrice = grille -> matrice;

    // On alloue de la mémoire
    Grille *result = init_grille(taille);
    cellule **new_matrice = result -> matrice;

    for (unsigned int i = 0; i < taille; i++)
    {
        for (unsigned int j = 0; j < taille; j++)
        {
            new_matrice[i][j] = matrice[i][j];
        }
    }

    return result;
}



/**
 * @brief Affiche les statistiques à la fin de la partie.
 * 
 * @param statistiques Un pointeur sur la structure contenant les stats
 */
void affiche_stats(Stats *statistiques)
{
    printf("En %lu générations et avec %lu cellules de départ:\n", statistiques -> generations, statistiques -> nb_cellules_depart);
    printf("  - %lu cellules sont nées\n", statistiques -> nb_cell_nes);
    printf("  - %lu cellules sont mortes\n", statistiques -> nb_cell_mortes);
    printf("  - %lu cellules étaient en vie à la fin de la simulation\n", statistiques -> en_vie);
    printf("  - %lu de ces %lu cellules sont des cellules originelles\n", statistiques -> nb_cell_originelles, statistiques -> en_vie);
}



/**
 * @brief Libère la mémoire allouée dans init_grille()
 * 
 * @param grille Un pointeur sur la Grille à libérer
 */
void free_grille(Grille *grille)
{
    free_matrice(grille -> matrice, grille -> taille);
    free(grille);
    grille = NULL;
}



/**
 * @brief Libère la mémoire allouée dans init_jeu()
 * 
 * @param jeu Un pointeur sur le Jeu à libérer
 */
void free_jeu(Jeu *jeu)
{
    free(jeu -> cam);
    free_grille(jeu -> grille);
    free(jeu -> statistiques);

    SDL_DestroyRenderer(jeu -> renderer);
    SDL_DestroyWindow(jeu -> fenetre);

    free(jeu);
    jeu = NULL;
}



/**
 * @brief Permet de libérer la mémoire allouée dans init_matrice()
 *
 * @param matrice La matrice à libérer
 * @param taille La taille de la grille à libérer
 */
void free_matrice(cellule **matrice, unsigned int taille)
{
    for (unsigned int i = 0; i < taille; i++)
    {
        free(matrice[i]);
        matrice[i] = NULL;
    }
    free(matrice);
    matrice = NULL;
}