#ifndef TYPES_HEADER
#define TYPES_HEADER


#include <SDL2/SDL.h>


typedef unsigned char cellule;


/**
 * @brief Structure contenant les statistiques du jeu.
 * nb_cell_nes contient le nombre de cellules nées au total.
 * 
 * nb_cell_mortes contient le nombre de cellules mortes au total.
 * 
 * nb_cell_originelles contient le nombre de cellules dites 
 * 'originelles', i.e celles placées lors de la configuration initiale.
 * 
 * nb_cellules_depart contient le nombre de cellules placées au départ
 * 
 * en_vie contient le nombre de cellules actuellement en vie
 * 
 * generations contient le nombre de générations du jeu
 */
typedef struct Stats {
    unsigned long int nb_cell_nes;
    unsigned long int nb_cell_mortes;
    unsigned long int nb_cell_originelles;
    unsigned long int nb_cellules_depart;
    unsigned long int en_vie;
    unsigned long int generations;
} Stats;

/**
 * @brief Structure représentant la grille
 * du jeu.
 * La 'grille' où les cellules évoluent sera représentée par une matrice carrée.
 *
 *  Cette matrice contiendra des char, + petit type disponible en C (1 octet).
 *  Nous avons théoriquement besoin d'un seul bit pour chaque cellule (0 ou 1 selon
 *  si la cellule est "vivante" ou non), mais nous mettrons à profit les bits superflus
 *  en mettant en place un sytème de couleur:
 *  
 *  Ce système permettra de visualiser "l'âge" des cellules -> la couleur des cellules seront fonction
 *  de leur âge -> à la 'naissance' elles seront bleues, et elles tendront vers le rouge en vieillissant
 *  On utilisera 7 bits pour stocker l'âge de la cellule (au max 128 générations), puis cet âge permettra
 *  de déterminer la couleur de la cellule.
 *  
 *  Enfin le dernier bit sera utilisé pour savoir si une cellule est "originelle" i.e si elle
 *  a été placée par l'utilisateur -> pour faire des stats sur le nombre de cellules générées
 *  directement par les cellules originelles etc.
 *  
 *  Nous utiliserons donc les 8 bits du char:
 *  - 7 pour l'âge (= la couleur)
 *  - 1 pour l'origine (originelle ou non) -> bit de poids fort
 *  La disposition (bit de poids faible à droite):
 *  O A A A A A A A
 *  Avec A les 7bits d'âge et O le bit 'd'origine'.
 */
typedef struct Grille {
    cellule **matrice;
    unsigned int taille;
} Grille;



/**
 * @brief Structure représentant la 'caméra' dans la grille.
 * Permet d'afficher qu'une partie de la grille, pour simuler un zoom.
 * 
 * (centre_x, centre_y) est le point représentant le centre de la caméra
 * 
 * (origin_x, origin_y) est le point représentant l'angle supérieur gauche de la caméra
 * 
 * width est la largeur de la caméra, i.e la longueur/hauteur de la sous grille à afficher
 * 
 * max_width est la largeur maximale, i.e la taille réelle de la grille.
 */
typedef struct Camera {
    unsigned int centre_x;
    unsigned int centre_y;

    unsigned int width;
    unsigned int max_width;

    unsigned int origin_x;
    unsigned int origin_y;
} Camera;



/**
 * @brief Structure contenant toutes les informations
 * nécessaires à l'affichage du jeu.
 * 
 * cam: Un pointeur sur la camera dans la fenêtre actuelle
 * 
 * grille: Un pointeur sur la structure qui contient la grille de jeu
 * 
 * statistiques: Un pointeur sur la structure qui permet de compter combien
 * de cellules naissent, meurent, sont en vie à un tour t etc.
 * 
 * fenetre Un pointeur sur la structure contenant la fenetre 
 * actuellement affichée à l'écran
 * 
 * de même pour renderer
 * 
 * estPause: 1 si le jeu est 'en pause', 0 sinon
 * 
 * estCouleur: 1 si le jeu est affiché en couleur, 0 sinon
 * 
 * estQuadrille: 1 si on affiche le quadrillage, 0 sinon
 * 
 * delay_ms: Le délai en ms entre 2 affichages de la grille
 * 
 * largeur_cell: La largeur d'une cellule dans la fenetre (diminue quand on dezoom)
 * 
 */ 
typedef struct Jeu {
    Camera *cam;     // ? Stocker pointeurs ou struct direct ?
    Grille *grille;
    Stats *statistiques;
    SDL_Window *fenetre;
    SDL_Renderer *renderer;

    char estPause;
    char estCouleur;
    char estQuadrille;
    unsigned int delay_ms;
    unsigned int largeur_cell;
} Jeu;



Stats *init_stats();
Grille *init_grille(unsigned int taille);
cellule **init_matrice(unsigned int taille);
Camera *init_camera(unsigned int taille, unsigned int taille_max);
Jeu *init_jeu(unsigned int taille_choisie, unsigned int largeur_f, unsigned int hauteur_f);

Grille *copie_grille(Grille *grille);
void free_jeu(Jeu *jeu);
void affiche_stats(Stats *statistiques);

void free_grille(Grille *grille);
void free_matrice(cellule **matrice, unsigned int taille);


#endif
