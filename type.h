#ifndef TYPE_H
#define TYPE_H

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define W_WIDTH 1820
#define W_HEIGH 215
#define half 10
#define full 20
#define size_piano 1820
#define octave 12
#define erreur -1
#define noire 0
#define droite 1
#define gauche 2
#define centre 5
#define ext_droite 3
#define ext_gauche 4

void error(const char* message);

void error_img(const char* message, SDL_Renderer* renderer, SDL_Window* window);

int type_touche(int** T, int t, int k);

int trouver_position(int num);

void afficher_touche(int** T, int t, int k, SDL_Window* window, SDL_Renderer* renderer);

#endif