/**
 * @file utilitaires.h
 * @author M3tex
 * @brief Header pour utilitaires.c
 * @version 0.1
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef UTILS_HEADER
#define UTILS_HEADER


void print_redb(const char *msg);
void quitter(const char *msg, int code_err);

char is_int(char *s);
char string2int(char *s, int *result);
char string2uint(char *s, unsigned int *result);
char mult_int(int x, int y, int *result);
char mult_uint(unsigned int x, unsigned int y, unsigned int *result);
char add_int(int x, int y, int *result);
char add_uint(unsigned int x, unsigned int y, unsigned int *result);

int get_int(const char *msg);
unsigned int get_uint(const char *msg);
void get_coord(int *x, int *y, unsigned int taille);

unsigned int min_uint(unsigned int a, unsigned int b);

#endif