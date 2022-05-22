#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_fft_complex.h>

#include "type.h"
#include "readfile.h"
#include "traitement.h"

#define W_WIDTH 1820
#define W_HEIGH 215


int main(int argc, char* argv[]) {
	struct transcription T=traitement();

	//declaration des types de la SDL
	SDL_Window *window=NULL;
	SDL_Renderer *renderer=NULL;
	SDL_Surface *image=NULL;
	SDL_Texture *texture=NULL;

	//initialisation SDL :
	if(SDL_Init(SDL_INIT_VIDEO)!=0){ error("init SDL"); }

	//creation fenetre :
	
	window=SDL_CreateWindow("lecteur de notes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGH,0);

	if(window==NULL){error("init fenetre");}

	//création rendu :

	renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);
	if (renderer==NULL){error("init rendu");}


	image=SDL_LoadBMP("textures/piano_88_touches.bmp");
	if(image==NULL){error_img("chargement image", renderer, window);}

	texture=SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);
	if(texture==NULL){error_img("chargement texture", renderer, window);}

	//affichage texture piano :

	SDL_Rect rectangle;
	
	if(SDL_QueryTexture(texture, NULL,NULL, &rectangle.w, &rectangle.h)!=0){error_img("Query", renderer, window);}

	rectangle.x=(W_WIDTH-rectangle.w)/2;
	rectangle.y=(W_HEIGH-rectangle.h)/2;

	if(SDL_RenderCopy(renderer, texture, NULL, &rectangle)!=0){error_img("affichage texture", renderer, window);}
	SDL_RenderPresent(renderer);

	

//--------------------------------------------------------------------------------------//
	//TRAITEMENT DU SIGNAL :

	double DT = T.Tech*1000;		// Conversion de secondes à millisecondes
	int len=T.length;
	int max_key=10;

	//on se donne T.signal la matrice len x 10 => dt intervalle de temps et k de touche 
	//DT ecart entre 2 notes
	for (int dt=0; dt<len; ++dt){
		for (int k=0; k<max_key; ++k){
			afficher_touche(T.signal, dt, k, window, renderer);
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(DT);
		if(SDL_QueryTexture(texture, NULL,NULL, &rectangle.w, &rectangle.h)!=0){error_img("Query", renderer, window);}
		if(SDL_RenderCopy(renderer, texture, NULL, &rectangle)!=0){error_img("affichage texture", renderer, window);}
		SDL_RenderPresent(renderer);

	}

	//destruction/liberation 

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return EXIT_SUCCESS;
}