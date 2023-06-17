/**
 * @file affichage.c
 * @author M3tex
 * @brief Fichier contenant toutes les fonctions utile à l'affichage du
 * jeu.
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "utilitaires.h"
#include "logique.h"
#include "affichage.h"
#include "types.h"




/**
 * @brief Permet de charger une configuration pré-enregistrée
 * dans un fichier.gol
 *
 * @param fichier Le chemin vers le fichier à ouvrir
 * @param jeu Un pointeur sur le Jeu
 * @param x L'abscisse où insérer le coin supérieur gauche du paterne
 * @param y L'ordonnée où insérer le coin supérieur gauche du paterne
 * @return char 1 si chargé avec succès, 0 sinon.
 */
char file2grid(const char *fichier, Jeu *jeu, int x, int y)
{
    // + lisible
    unsigned int taille = jeu -> grille -> taille;
    cellule **matrice = jeu -> grille -> matrice;

    printf("Le fichier choisit: %s\n", fichier);
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
    {
        print_redb("Impossible de charger le fichier. Vérifiez que le nom est correct.\n");
        return 0;
    }

    /* La première ligne contient la taille des lignes suivantes.
    On considère que la taille sera inférieure à 10^10 */
    char *nb_str = (char *) malloc(sizeof(char) * (10 + 1));
    if (nb_str == NULL) quitter("Impossible d'allouer de la mémoire pour lire le fichier", 2);

    char c = getc(f);
    unsigned char cpt = 0;
    while (c != '\n')
    {
        nb_str[cpt] = (unsigned char) c;
        c = getc(f);
        cpt++;
    }

    // On passe au suivant car le char actuel est '\n'
    c = getc(f);
    nb_str[cpt] = '\0';
    
    unsigned int nb = 0;
    if (!string2uint(nb_str, &nb)) return 0;
    free(nb_str);
    nb_str = NULL;

    // On vérifie que le paterne rentre bien aux coordonées données
    if (x + nb > taille || y + nb > taille) return 0;


    // On parcourt ensuite les autres lignes
    char *ligne = (char *) malloc(sizeof(char) * (nb + 1));     // +1 pour le \0
    if (ligne == NULL) quitter("Impossible d'allouer de la mémoire pour lire le fichier", 2);

    ligne[nb] = '\0';
    unsigned int i = 0;
    unsigned int nb_lignes = 0;
    while (c != EOF)
    {
        // Si on change de ligne, on charge la ligne dans la grille
        if (c == '\n' && i != 0)
        {
            for (unsigned int j = 0; j < nb; j++)
            {
                if (ligne[j] == '1')
                {
                    matrice[nb_lignes + y][j + x] = (1 << 7) + 1;  // 1 << 7 car cell originelle
                    jeu -> statistiques -> nb_cellules_depart++;
                }
                ligne[j] = 'X';     // marqueur pour + tard (pour savoir si il reste une ligne)
            }
            c = getc(f);
            i = 0;
            nb_lignes++;
            continue;
        }

        ligne[i++] = c;
        c = getc(f);
    }
    // On rajoute la dernière ligne si besoin
    if (ligne[0] != 'X')
    {
        for (unsigned int j = 0; j < nb; j++)
        {
            if (ligne[j] == '1')
            {
                matrice[i - 1 + y][j + x] = (1 << 7) + 1;
                jeu -> statistiques -> nb_cellules_depart++;
            }
        }
    }
    free(ligne);
    return 1;
}




/**
 * @brief Affiche la grille dans la fenetre SDL
 * 
 * @param jeu Un pointeur sur le Jeu
 */
void affiche_grille(Jeu *jeu)
{
    // + lisible (évite les jeu -> XXX -> XXX)
    SDL_Renderer *renderer = jeu -> renderer;
    unsigned int largeur_cell = jeu -> largeur_cell;
    unsigned int taille_cam = jeu -> cam -> width;
    cellule **matrice = jeu -> grille -> matrice;
    Camera *cam = jeu -> cam;


    // On met la couleur du fond en noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Rect tmp_rect;
    tmp_rect.h = largeur_cell;
    tmp_rect.w = largeur_cell;

    // On affiche les cellules en blanc par défaut
    unsigned char r = 255, g = 255, b = 255;
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    for (unsigned int i = 0; i < taille_cam; i++)
    {
        for (unsigned int j = 0; j < taille_cam; j++)
        {
            /* Permet de s'assurer que on aura pas de segfault et que la caméra
            reste bien dans la grille */
            update_camera(cam);

            // On affiche que les cellules vivantes
            if (matrice[cam -> origin_y + i][cam -> origin_x + j])
            {
                tmp_rect.x = j * largeur_cell;
                tmp_rect.y = i * largeur_cell;

                if (jeu -> estCouleur) get_color(matrice[cam -> origin_y + i][cam -> origin_x + j], &r, &g, &b);

                /* On vérifie que la couleur ne soit pas noir, si c'est le cas
                on la met en blanc (pour éviter d'avoir du noir sur du noir) */
                if (r == 0 && g == 0 && b == 0) r = 255, g = 255, b = 255;

                SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &tmp_rect);
            }
        }

        // Affichage du quadrillage si choisit par l'utilisateur
        if (jeu -> estQuadrille)
        {
            // On affiche les lignes en blanc
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            
            // Lignes verticales
            SDL_RenderDrawLine(renderer, i * largeur_cell, 0, i * largeur_cell, cam -> max_width);

            // Lignes horizontales
            SDL_RenderDrawLine(renderer, 0, i * largeur_cell, cam -> max_width, i * largeur_cell);
        }
    }

    // On affiche tout d'un coup
    SDL_RenderPresent(renderer);
}



/**
 * @brief Permet de demander une configuration initiale à l'utilisateur.
 * La configuration sera chargée depuis un fichier spécifié par l'utilisateur.
 * 
 * @param grille Un pointeur sur le jeu
 */
void init_fichier(Jeu *jeu)
{
    // + lisible
    Camera *cam = jeu -> cam;

    char file[50];      // On considère qu'un nom de fichier ne dépassera pas 50 caractères.
    system(CLEAR);
    printf("Choisissez un fichier (au format templates/[nom fichier.gol]) avec nom fichier parmi:\n");
    system(LS);
    print_redb("\nPour spacefiller: très joli en couleur mais:\n");
    print_redb("attention aux performances si grille > 500x500 (zoomer pour aller + vite)\n");
    scanf("%s", file);
    getchar();

    // On demande où insérer le template
    int x, y;
    get_coord(&x, &y, cam -> width);

    /* Les coordonnées demandées au dessus sont par rapport à la caméra actuelle.
    On les ramène aux 'vraies' coordonnées dans la grille complète */
    x = cam -> origin_x + x;
    y = cam -> origin_y + y;
    file2grid(file, jeu, x, y);
    init_GUI(jeu);
}



/**
 * @brief Permet de demander une configuration initiale à l'utilisateur.
 * La configuration sera demandée depuis le terminal.
 * 
 * @param jeu Un pointeur sur le jeu
 */
void init_terminal(Jeu *jeu)
{
    // + lisible
    cellule **matrice = jeu -> grille -> matrice;
    Camera *cam = jeu -> cam;

    unsigned int nb_cell_debut = get_uint("Combien de cellules de départ ?");
    int x, y;
    for (unsigned int i = 0; i < nb_cell_debut; i++)
    {
        get_coord(&x, &y, cam -> width);

        // idem on ramène aux vraies coordonnés dans la grille complète
        x = (cam -> origin_x) + x;
        y = (cam -> origin_y) + y;
        matrice[y][x] = (1 << 7) + 1;       // 1 << 7 car cellule originelle
    }
    jeu -> statistiques -> nb_cellules_depart = nb_cell_debut;
    init_GUI(jeu);
}



/**
 * @brief Permet d'obtenir une configuration de départ aléatoire, à l'image
 * des 'soup search' utilisés pour trouver de nouvelles structures
 * (voir https://conwaylife.com/wiki/Soup)
 * 
 * @param jeu Un pointeur sur le jeu concerné
 */
void init_rdm(Jeu *jeu)
{
    // + lisible
    Camera *cam = jeu -> cam;
    unsigned int taille = cam -> width;
    cellule **matrice = jeu -> grille -> matrice;

    // On parcourt la sous-grille de taille sélectionnée par l'utilisateur
    for (unsigned int i = 0; i < taille; i++)
    {
        for (unsigned int j = 0; j < taille; j++)
        {
            if (random() & 1)
            {
                matrice[cam -> origin_y + i][cam -> origin_x + j] = (1 << 7) + 1;
                jeu -> statistiques -> nb_cellules_depart++;
            }
        }
    }

    init_GUI(jeu);
}



/**
 * @brief Permet de demander une configuration initiale à l'utilisateur.
 * La configuration sera demandée depuis le GUI.
 * 
 * @param jeu Un pointeur sur le jeu concerné
 */
void init_GUI(Jeu *jeu)
{
    jeu -> fenetre = SDL_CreateWindow("Configuration initiale", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, largeur_f, hauteur_f, 0);
    if (jeu -> fenetre == NULL)
    {
        printf("Erreur SDL: %s\n", SDL_GetError());
        quitter("Impossible de continuer suite à l'erreur.", 3);
    }

    jeu -> renderer = SDL_CreateRenderer(jeu -> fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (jeu -> renderer == NULL)
    {
        printf("Erreur SDL: %s\n", SDL_GetError());
        quitter("Impossible de continuer suite à l'erreur.", 3);
    }

    // On affiche les commandes spécifiques à la configuration initiale et on lance la boucle d'affichage
    affiche_commandes(jeu, 1);
    char affiche = 1;
    while (affiche)
    {
        // On regarde les évènements (clics souris etc)
        SDL_Event event;
        watch_events(&event, jeu, &affiche, 1);

        // On affiche l'état de la grille
        affiche_grille(jeu);
    }
}



/**
 * @brief Permet de gérér les évènements SDL:
 * Par exemple permet de quitter la boucle de jeu
 * quand l'utilisateur essaye de fermer la fenêtre
 * ou de bouger la caméra.
 * 
 * @param event Un pointeur sur la structure contenant les évènement.
 * @param jeu Un pointeur sur le Jeu concerné
 * @param gameloop Un pointeur sur la variable controlant la boucle d'affichage
 * @param estConfig 1 si on gère les évènements dans le menu de config, 0 sinon.
 */
void watch_events(SDL_Event *event, Jeu *jeu, char *gameloop, char estConfig)
{
    // + lisible
    unsigned int largeur_cell = jeu -> largeur_cell;
    Camera *cam = jeu -> cam;
    cellule **matrice = jeu -> grille -> matrice;
    
    // ? casser en plusieurs sous-fonction car trop grosse ?

    //SDL_MouseMotionEvent moved;
    SDL_MouseButtonEvent clicked;
    SDL_MouseWheelEvent scrolled;

    unsigned int click_x, click_y;
    unsigned int old_width;
    while (SDL_PollEvent(event))
    {
        switch (event -> type)
        {
        // On quitte la boucle si l'utilisateur ferme la fenetre
        case SDL_QUIT:
            *gameloop = 0;
            break;
        case SDL_WINDOWEVENT_CLOSE:
            *gameloop = 0;
            break;

        // Si l'utilisateur fait un click on récupère les coordonées du click
        case SDL_MOUSEBUTTONDOWN:
            clicked = event -> button;
            click_x = cam -> origin_x + (clicked.x / largeur_cell), click_y = cam -> origin_y + (clicked.y / largeur_cell);

            // On regarde quel bouton est pressé et on agit en conséquence
            switch (clicked.button)
            {
                case SDL_BUTTON_LEFT:
                    // Si appui sur bouton gauche on ajoute une cellule (menu config seulement)
                    if (estConfig && !matrice[click_y][click_x])
                    {
                        matrice[click_y][click_x] = (1 << 7) + 1;     // (1 << 7) + 1 pour stats sur cellules originelles.
                        jeu -> statistiques -> nb_cellules_depart++;
                    }
                    break;
                case SDL_BUTTON_RIGHT:
                    // Si appui sur bouton droit on supprime la cellule (menu config seulement)
                    if (estConfig && matrice[click_y][click_x])
                    {
                        matrice[click_y][click_x] = 0;
                        jeu -> statistiques -> nb_cellules_depart--;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SDL_MOUSEMOTION:
            /* On enregistre le dernier mouvement de souris avant le scroll.
            todo: Zoomer sur là où le curseur pointe ? */
            //moved = event -> motion;
            break;

        case SDL_MOUSEWHEEL:
            /* Permet de détecter un scroll.
            On met à jour la camera en fonction des scroll. */
            scrolled = event -> wheel;
            old_width = cam -> width;

            /* On vérifie qu'on peut réduire la largeur des cellules.
            scrolled.y vaut 1 ou -1 en fonction du sens de rotation de la molette */
            if (largeur_cell + scrolled.y >= 1 && largeur_cell + scrolled.y < cam -> max_width)
            {
                largeur_cell += scrolled.y;
                cam -> width = cam -> max_width / largeur_cell;

                /* Évite des mouvements de caméra brusques (honnêtement je ne sais pas pourquoi il 
                y a des mouvements brusques si je mets jeu -> largeur_cell += scrolled.y directement)*/
                jeu -> largeur_cell = largeur_cell;

                // On met à jour la caméra seulement si la largeur change (inutile sinon)
                if (cam -> width != old_width && cam -> centre_x >= (cam -> width / 2) && cam -> centre_y >= (cam -> width / 2))
                {
                    cam -> origin_x = cam -> centre_x - (cam -> width / 2);
                    cam -> origin_y = cam -> centre_y - (cam -> width / 2);
                    update_camera(cam);
                }
            }
            break;
        
        case SDL_KEYDOWN:
            // Détecte les touches pressées
            switch ((event -> key).keysym.sym)
            {
            case SDLK_UP:
                // On translate notre origine vers le haut
                if (cam -> origin_y - 1 <= cam -> max_width && cam -> origin_y - 1 + cam -> width < cam -> max_width)    // 1ère condition car 0 - 1 = UINT_MAX
                {
                    cam -> origin_y--;
                    update_camera(jeu -> cam);
                }
                break;
            case SDLK_DOWN:
                // On translate notre origine vers le bas
                if (cam -> origin_y + 1 <= cam -> max_width && cam -> origin_y + 1 + cam -> width < cam -> max_width)
                {
                    cam -> origin_y++;
                    update_camera(cam);
                }
                break;
            case SDLK_LEFT:
                // On translate notre origine vers la gauche
                if (cam -> origin_x - 1 <= cam -> max_width && cam -> origin_x - 1 + cam -> width < cam -> max_width)
                {
                    cam -> origin_x--;
                    update_camera(cam);
                }
                break;
            case SDLK_RIGHT:
                // On translate notre origine vers la droite
                if (cam -> origin_x + 1 <= cam -> max_width && cam -> origin_x + 1 + cam -> width < cam -> max_width)
                {
                    cam -> origin_x++;
                    update_camera(cam);
                }
                break;
            
            // On reset la grille si menu de config et touche R pressée
            case SDLK_r:
                if (estConfig)
                {
                    free_matrice(jeu -> grille -> matrice, jeu -> grille -> taille);
                    jeu -> grille -> matrice = init_matrice(jeu -> grille -> taille);
                }
                break;
            
            // On lance la simulation si menu de config et touche espace pressée
            case SDLK_SPACE:
                if (estConfig) *gameloop = 0;
                break;
            
            // On met en pause si la touche P est pressée
            case SDLK_p:
                jeu -> estPause = !(jeu -> estPause);
                break;
            
            // On affiche la grille si la touche G est pressée
            case SDLK_g:
                jeu -> estQuadrille = !(jeu -> estQuadrille);
                break;
            
            // On augmente / diminue le délais si la touche i / k est pressée.
            case SDLK_i:
                jeu -> delay_ms++;
                break;
            case SDLK_k:
                if (jeu -> delay_ms - 1 < jeu -> delay_ms) jeu -> delay_ms--;
                break;
            
            // On met le jeu en couleur si la touche c est pressée
            case SDLK_c:
                jeu -> estCouleur = !(jeu -> estCouleur);
            default:
                break;
            }
        default:
            break;
        }

    }
}



/**
 * @brief Appelée si le programme est mal lancé.
 * Affiche une aide sur les options disponibles.
 * 
 */
void affiche_aide()
{
    printf("Utilisation:\n");
    printf("'./gol -f' -> Charge une configuration de départ depuis un fichier.gol\n");
    printf("'./gol -t' -> Demande une configuration de départ dans le terminal\n");
    printf("'./gol -r' -> Configuration de départ aléatoire\n");
    printf("'./gol -g' -> Demande une configuration de départ depuis le GUI\n\n");
    quitter("Commande incorrecte\n", 1);
}



/**
 * @brief Recalcule le point d'origine en fonction du centre et
 * évite de sortir de la grille.
 * 
 */
void update_camera(Camera *cam)
{
    // On vérifie qu'on ne sort pas de la grille
    if (cam -> origin_x + cam -> width >= cam -> max_width) cam -> origin_x = cam -> max_width - cam -> width;
    if (cam -> origin_y + cam -> width >= cam -> max_width) cam -> origin_y = cam -> max_width - cam -> width;
    if (cam -> origin_x > cam -> max_width) cam -> origin_x = 0;
    if (cam -> origin_y > cam -> max_width) cam -> origin_y = 0;

    // On met à jour le centre de la caméra à partir de l'origine
    cam -> centre_x = cam -> origin_x + (cam -> width / 2);
    cam -> centre_y = cam -> origin_y + (cam -> width / 2);
}



/**
 * @brief Affiche les commandes disponibles en fonction du contexte:
 * - commandes de caméra
 * - commandes dans le menu de config
 * - commandes dans le jeu
 * 
 * @param estConfig 1 si commandes du menu de config, 0 sinon
 */
void affiche_commandes(Jeu *jeu, char estConfig)
{
    // Commandes générales
    system(CLEAR);
    printf("Utilisez la molette pour zoomer / dézoomer\n");
    printf("Utilisez les touches directionnelles pour déplacer la caméra dans la grille\n");
    printf("Appuyez sur 'c' pour afficher le jeu en couleur\n");
    printf("Appuyez sur 'g' pour afficher la grille ");
    print_redb("attention si dezoom au maximum avec la grille activée, tout devient blanc !\n\n");

    // Commandes spécifiques au menu de configuration
    if (estConfig)
    {
        printf("Appuyez sur clic gauche pour ajouter une cellule, clic droit pour en supprimer une\n");
        printf("Appuyez sur 'r' pour réinitialiser la grille\n");
        printf("Appuyez sur espace pour lancer le jeu\n");

        /* On affiche un popup pour notifier l'utilisateur que les commandes sont affichées dans
        le terminal (on pourrait les mettre direct dans le popup avec un SDL_ShowMessageBox aussi) */
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Commandes", "Les commandes sont "
                                 "affichées dans le terminal.", jeu -> fenetre);
        return;
    }

    // Commandes spécifiques au jeu
    printf("Appuyez sur 'p' pour mettre le jeu en pause\n");
    printf("Appuyez sur 'i' pour augmenter le délai entre 2 mises à jours de la grille\n");
    printf("Appuyez sur 'k' pour diminuer le délai entre 2 mises à jour de la grille\n");
}



/**
 * @brief À partir de l'âge d'une cellule au format:
 * O A A A A A A A, avec A les 7 bits d'âge de la cellule,
 * permet de déterminer la couleur de cette cellule.
 * 
 * @param cell La cellule concernée
 * @param r Un pointeur sur la variable stockant le rouge
 * @param g Un pointeur sur la variable stockant le vert
 * @param b Un pointeur sur la variable stockant le bleu
 */
void get_color(cellule cell, unsigned char *r, unsigned char *g, unsigned char *b)
{
    /* On utilise le spectre du visible pour déterminer la couleur en fonction de l'âge:
    Une cellule d'âge 0 aura une longueur d'onde de 420nm, une cellule d'âge 1 aura une 
    longueur d'onde de 422, ..., une cellule d'âge maximal aura une longueur d'onde de 676nm.
    
    Attention: j'ai récupéré la fonction permettant de déterminer une couleur rgb256 à partir d'une
    longueur d'onde sur internet (voir dans README où j'explique + en détail). */
    double rd, gd, bd;
    double long_onde = (cell & 127) * 2 + 420;
    spectral_color(&rd, &gd, &bd, long_onde);   // fonction récupérée sur StackOverflow

    // On convertit en rgb256
    *r = 256 * rd;
    *g = 256 * gd;
    *b = 256 * bd;
}




// RGB <0,1> <- lambda l <400,700> [nm]
/**
 * @brief Permet de déterminer une couleur en rgb à partir
 * d'une longueur d'onde en nm.
 * 
 * Fonction récupérée ici:
 * https://stackoverflow.com/questions/3407942/rgb-values-of-visible-spectrum/22681410#22681410
 * 
 * @param r Un pointeur sur la variable stockant le rouge
 * @param g Un pointeur sur la variable stockant le vert
 * @param b Un pointeur sur la variable stockant le bleu
 * @param l La longueur d'onde en nm
 */
void spectral_color(double *r,double *g,double *b,double l) 
{
double t;  
*r=0.0; *g=0.0; *b=0.0;
        if ((l>=400.0)&&(l<410.0)) {t=(l-400.0)/(410.0-400.0); *r=    +(0.33*t)-(0.20*t*t); }
else if ((l>=410.0)&&(l<475.0)) { t=(l-410.0)/(475.0-410.0); *r=0.14         -(0.13*t*t); }
else if ((l>=545.0)&&(l<595.0)) { t=(l-545.0)/(595.0-545.0); *r=    +(1.98*t)-(     t*t); }
else if ((l>=595.0)&&(l<650.0)) { t=(l-595.0)/(650.0-595.0); *r=0.98+(0.06*t)-(0.40*t*t); }
else if ((l>=650.0)&&(l<700.0)) { t=(l-650.0)/(700.0-650.0); *r=0.65-(0.84*t)+(0.20*t*t); }
        if ((l>=415.0)&&(l<475.0)) { t=(l-415.0)/(475.0-415.0); *g=             +(0.80*t*t); }
else if ((l>=475.0)&&(l<590.0)) { t=(l-475.0)/(590.0-475.0); *g=0.8 +(0.76*t)-(0.80*t*t); }
else if ((l>=585.0)&&(l<639.0)) { t=(l-585.0)/(639.0-585.0); *g=0.84-(0.84*t)           ; }
        if ((l>=400.0)&&(l<475.0)) { t=(l-400.0)/(475.0-400.0); *b=    +(2.20*t)-(1.50*t*t); }
else if ((l>=475.0)&&(l<560.0)) { t=(l-475.0)/(560.0-475.0); *b=0.7 -(     t)+(0.30*t*t); }
}