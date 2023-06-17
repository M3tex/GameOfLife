/**
 * @file logique.c
 * @author M3tex
 * @brief Fichier contenant toutes les fonctions utile à la logique
 * interne du jeu 'Game of Life', indépendamment des fonctions d'affichage.
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "logique.h"
#include "utilitaires.h"




/**
 * @brief Compte le nombre de voisins d'une cellule
 * 
 * @param grille La structure contenant la grille
 * @param x L'abscisse de la cellule concernée
 * @param y L'ordonnée de la cellule concernée
 * @return unsigned char Le nombre de voisins de la cellule.
 */
unsigned char compte_voisin(Grille *grille, unsigned int x, unsigned int y)
{
    // + lisible (évite les -> partout)
    unsigned int taille = grille -> taille;
    cellule **matrice = grille -> matrice;

    /* Obligé de tester car les coords sont en unsigned int -> si x = 0, x - 1 = 2^(sizeof(int) * 8)
    De même, si x = 2^(sizeof(int) * 8), x + 1 = 0.*/
    unsigned int debut_x = x - 1 > x ? x : x - 1;
    unsigned int lim_x = x + 1 < x ? x : x + 1;

    // Idem pour y
    unsigned int debut_y = y - 1 > y ? y : y - 1;
    unsigned int lim_y = y + 1 < y ? y : y + 1;

    char cpt = 0;
    for (unsigned int i = debut_x; i <= lim_x; i++)
    {
        for (unsigned int j = debut_y; j <= lim_y; j++)
        {
            // On vérifie qu'on ne sort pas de la grille ou qu'on ne compte pas la cellule actuelle
            if (!(i < taille && j < taille) || (i == x && j == y)) continue;
            cpt += matrice[i][j] != 0;  
        }
    }
    return cpt;
}



/**
 * @brief Utilise les règles du jeu pour calculer l'itération
 * suivante à partir de la grille actuelle.
 *
 * @param grille Un pointeur sur la grille à mettre à jour.
 * @param taille La taille de la grille.
 */
void maj_grille(Jeu *jeu)
{
    // + lisible
    unsigned int taille = jeu -> grille -> taille;
    cellule **matrice = jeu -> grille -> matrice;
    Stats *statistiques = jeu -> statistiques;
    
    Grille *next_it = copie_grille(jeu -> grille);
    statistiques -> en_vie = 0;
    statistiques -> nb_cell_originelles = 0;
    
    /* On va compter le nombre de voisin pour chaque cellule et en
    déduire l'état de la cellule à l'itération suivante */
    char tmp_voisins;
    for (unsigned int i = 0; i < taille; i++)
    {
        for (unsigned int j = 0; j < taille; j++)
        {
            tmp_voisins = compte_voisin(jeu -> grille, i, j);

            // Si cellule morte et 3 voisins -> passe à vivante
            if (tmp_voisins == 3 && !matrice[i][j])
            {
                (next_it -> matrice)[i][j] = 1;
                
                // On met à jour les stats
                statistiques -> nb_cell_nes += 1;
            }

            /* Si vivante et 2 ou 3 voisins -> elle reste vivante.
            Dans tous les autres cas elle meurt*/
            if (matrice[i][j])
            {
                // On met à jour les stats
                statistiques -> en_vie += 1;
                if (matrice[i][j] & (1 << 7)) statistiques -> nb_cell_originelles++;

                /* On augmente l'âge de la cellule.
                & 127 car 127 en binaire: 01111111 */
                unsigned char age = (matrice[i][j] & 127) + 1;

                /* Si l'age ne dépasse pas nos 7 bits on met à jour l'âge de la cellule
                Si ça dépasse elle garde son âge actuel (127 générations) */
                if (!(age & (1 << 7)))
                {
                    /* & (1 << 7) pour garder le 8ème bit servant à déterminer si une cellule
                    est originelle ou non. */
                    (next_it -> matrice)[i][j] = (matrice[i][j] & (1 << 7)) | age;
                }

                // On applique la règle du jeu de la vie
                if (!(tmp_voisins == 2 || tmp_voisins == 3))
                {
                    (next_it -> matrice)[i][j] = 0;
                    statistiques -> nb_cell_mortes += 1;
                }
            }
        }
    }

    // On libère l'ancienne grille et on la remplace par la nouvelle
    free_grille(jeu -> grille);
    jeu -> grille = next_it;
}

