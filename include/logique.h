/**
 * @file logique.h
 * @author M3tex
 * @brief Header pour logique.c
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LOGIQUE_HEADER
#define LOGIQUE_HEADER


#include "types.h"

unsigned char compte_voisin(Grille *grille, unsigned int x, unsigned int y);
void maj_grille(Jeu *jeu);


#endif
