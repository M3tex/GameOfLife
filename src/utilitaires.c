/**
 * @file utilitaires.c
 * @author M3tex
 * @brief Fichier contenant toutes les fonctions utilitaires
 * non spécifiques au jeu.
 * @version 0.1
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "utilitaires.h"





/**
 * @brief Affiche un message en rouge et en gras.
 * 
 * @param msg Le message à afficher
 */
void print_redb(const char *msg)
{
    // Séquence d'échappement ANSI pour écrire en rouge et en gras.
    // https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
    printf("\033[31;1m%s\033[0m", msg);
}



/**
 * @brief Permet de stopper l'exécution du programme en affichant un
 * message.
 *
 * @param msg Le message à afficher.
 * @param code_err Le code de sortie du programme.
 */
void quitter(const char *msg, int code_err)
{
    print_redb(msg);
    exit(code_err);
}



/**
 * @brief Permet de multiplier 2 entiers si le résultat
 * de cette opération ne produit pas de int overflow.
 *
 * Si l'opération x * y est possible, l'entier pointé par
 * result reçoit la valeur x * y.
 *
 * @param x
 * @param y
 * @param result Le pointeur vers l'entier où stocker le
 * résultat
 * @return 0 si int overflow, 1 sinon.
 */
char mult_int(int x, int y, int *result)
{
    if (y != 0 && x > INT_MAX / y) return 0;
    *result = x * y;
    return 1;
}



/**
 * @brief Permet de multiplier 2 entiers non signés si le
 * résultat de cette opération ne produit pas de uint overflow.
 *
 * Si l'opération x * y est possible, l'entier non signé pointé
 * par result reçoit la valeur x * y.
 *
 * @param x
 * @param y
 * @param result Le pointeur vers l'entier non signé où stocker
 * le résultat
 * @return 0 si uint overflow, 1 sinon.
 */
char mult_uint(unsigned int x, unsigned int y, unsigned int *result)
{
    if (y != 0 && x > UINT_MAX / y) return 0;
    *result = x * y;
    return 1;
}



/**
 * @brief Permet d'additionner 2 entiers si le résultat
 * de cette opération ne produit pas de int overflow.
 *
 * Si l'opération x + y est possible, l'entier pointé par
 * result reçoit la valeur x + y.
 *
 * @param x
 * @param y
 * @param result Le pointeur vers l'entier où stocker le
 * résultat
 * @return 0 si int overflow, 1 sinon.
 */
char add_int(int x, int y, int *result)
{
    if (x > INT_MAX - y) return 0;
    *result = x + y;
    return 1;
}



/**
 * @brief Permet d'additionner 2 entiers non signés si le
 * résultat de cette opération ne produit pas de uint overflow.
 *
 * Si l'opération x + y est possible, l'entier non signé pointé
 * par result reçoit la valeur x + y.
 *
 * @param x
 * @param y
 * @param result Le pointeur vers l'entier non signé où stocker
 * le résultat
 * @return 0 si uint overflow, 1 sinon.
 */
char add_uint(unsigned int x, unsigned int y, unsigned int *result)
{
    if (x > UINT_MAX - y) return 0;
    *result = x + y;
    return 1;
}



/**
 * @brief Permet de savoir si une chaîne de caractère
 * peut-être convertie en entier.
 *
 * @param s La chaîne à tester.
 * @return int 1 si la chaîne représente un entier,
 * 0 sinon.
 */
char is_int(char *s)
{
    int taille = strlen(s);
    for (int i = 0; i < taille; i++)
    {
        if (!((s[i] >= '0' && s[i] <= '9') || (i == 0 && s[i] == '-')))
        {
            return 0;
        }
    }
    return 1;
}



/**
 * @brief Permet de convertir une chaîne de caractères en entier.
 * Prend en charge les entiers négatifs.
 * Prend en charge la chaine "0" (contrairement à atoi).
 *
 * @param s La chaîne à convertir
 * @param result Le pointeur vers l'entier où stocker le résultat.
 * @return int 1 si conversion réussie, 0 sinon.
 */
char string2int(char *s, int *result)
{
    // Si la chaîne ne représente pas un entier on retourne 0
    int len = strlen(s);
    if (len == 0 || !is_int(s)) return 0;

    // On stocke le signe
    int est_negatif =  (s[0] == '-');

    int tmp = 0;
    for (int i = est_negatif; i < len; i++)    // si neg on commence à l'indice 1
    {
        /* À chaque itération on réalise l'opération suivante:
        tmlp = 10 * tmp + s[i]
        On utilise des fonctions permettant d'éviter les int overflow. */
        if (mult_int(10, tmp, &tmp) + add_int(s[i] - '0', tmp, &tmp) < 2)
        {
            return 0;
        }
    }
    *result = est_negatif ? -tmp : tmp;
    return 1;
}



/**
 * @brief Permet de convertir une chaîne de caractères en entier non signé.
 * Prend en charge la chaine "0" (contrairement à atoi).
 *
 * @param s La chaîne à convertir
 * @param result Le pointeur vers l'entier non signé où stocker le résultat.
 * @return int 1 si conversion réussie, 0 sinon.
 */
char string2uint(char *s, unsigned int *result)
{
    // Si la chaîne ne représente pas un entier on retourne 0
    int len = strlen(s);
    if (len == 0 || !is_int(s)) return 0;

    unsigned int tmp = 0;
    for (int i = 0; i < len; i++)
    {
        /* À chaque itération on réalise l'opération suivante:
        tmp = 10 * tmp + s[i]
        On utilise des fonctions permettant d'éviter les int overflow. */
        if (mult_uint(10, tmp, &tmp) + add_uint(s[i] - '0', tmp, &tmp) < 2)
        {
            return 0;
        }
    }
    *result = tmp;
    return 1;
}



/**
 * @brief Demande à l'utilisateur de saisir un entier.
 *
 * @param msg Le message à afficher
 * @return int L'entier saisit
 */
int get_int(const char *msg)
{
    /* La valeur max d'un int signé est:
    vMax = 2^((sizeof(int) * 8) - 1) - 1.

    Le string le représentant fera donc au + log10(vMax) + 1 caractères. */
    int result;
    char *buffer;

    int taille_max = INT_MAX;
    unsigned char nb_chiffres = (unsigned char) log10((double) taille_max) + 1;

    buffer = (char *) malloc(sizeof(char) * (nb_chiffres + 1));     // +1 pour \0
    if (buffer == NULL) quitter("Impossible d'allouer de la mémoire pour lire l'entier", 2);

    buffer[nb_chiffres] = '\0';
    do {
        printf("%s\n", msg);
        scanf("%s", buffer);
        getchar();
    } while (!string2int(buffer, &result));

    free(buffer);
    return result;
}



/**
 * @brief Demande à l'utilisateur de saisir un entier non signé.
 *
 * @param msg Le message à afficher
 * @return unsigned int L'entier non signé saisit.
 */
unsigned int get_uint(const char *msg)
{
    /* La valeur max d'un int non signé est:
    vMax = 2^(sizeof(int) * 8) - 1.

    Le string le représentant fera donc au + log10(vMax) + 1 caractères. */
    unsigned int result;
    char *buffer;

    unsigned int val_max = UINT_MAX;
    unsigned char nb_chiffres = (unsigned char) log10((double) val_max) + 1;      
    buffer = (char *) malloc(sizeof(char) * (nb_chiffres + 1));     // +1 pour \0
    if (buffer == NULL) quitter("Impossible d'allouer de la mémoire pour lire l'entier", 2);
    
    buffer[nb_chiffres] = '\0';
    do {
        printf("%s\n", msg);
        scanf("%s", buffer);
        getchar();
    } while (!string2uint(buffer, &result));
    
    free(buffer);
    return result;
}




/**
 * @brief Retourne la valeur du minimum entre a et b
 *
 * @param a
 * @param b
 * @return unsigned int
 */
unsigned int min_uint(unsigned int a, unsigned int b)
{
    return (a < b ? a : b);
}



/**
 * @brief Permet de demander des coordonnées à l'utilisateur
 * 
 * @param x Un pointeur sur l'entier où stocker la coordonée x
 * @param y Un pointeur sur l'entier où stocker la coordonée y
 * @param taille La taille maximale de la grille (ne pas dépasser)
 */
void get_coord(int *x, int *y, unsigned int taille)
{
    *x = -1;
    *y = -1;
    char *msg = "Entrez les coordonnées (x, y):";
    do {
        printf("%s\n", msg);
        scanf("%d, %d", x, y);
        getchar();
        msg = "Coordonnées invalides, saisissez au format: x, y";
    } while ((*x < 0 || *x >= taille) || (*y < 0 || *y >= taille));
}
