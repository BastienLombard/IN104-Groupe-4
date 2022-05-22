#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define W_WIDTH 1820
#define W_HEIGH 215
#define half 20
#define full 35
#define size_piano 1820
#define octave 12
#define erreur -1
#define noire 0
#define droite 1
#define gauche 2
#define centre 5
#define ext_droite 3
#define ext_gauche 4


/*
T réfère au tableau de nb d'échantillons (lignes) et k touches (colonnes entre 0 et 9)
t réfère à l'instant donné (t-ième ligne de T)
k réfère au numéro de la touche (k-ième colonne de T)
*/

//fonctions d'erreur et pour liberer la mémoire en cas d'echec
void error(const char* message){
	SDL_Log("Erreur : %s, %s\n",message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void error_img(const char* message, SDL_Renderer* renderer, SDL_Window* window){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	error(message);
}


//renvoie un entier selon si la touche correspond a une touche "gauche", "droite", "noire", "extreme gauche", ou "extreme droite"
int type_touche(int num){
	if(num>88||num<0){
		return(erreur);
	}
	int pos;
	switch(num){
	//cas extreme gauche
		case 1:
		return(ext_gauche);
		break;
		case 2:
		return(noire);
		break;
		case 3:
		return(droite);
		break;
	//cas extreme droite
		case(88):
		return(ext_droite);
		break;
	//cas par défaut (une octave classique à 12 touches):
		default:
		pos = (num-3)%octave;
		switch(pos){
			case 1:
			return(gauche);
			break;
			case 2:
			return(noire);
			break;
			case 3:
			return(centre);
			case 4:
			return(noire);
			break;
			case 5:
			return(droite);
			break;
			case 6:
			return(gauche);
			break;
			case 7:
			return(noire);
			break;
			case 8:
			return(centre);
			break;
			case 9:
			return(noire);
			break;
			case 10:
			return(centre);
			break;
			case 11:
			return(noire);
			break;
			case 0:
			return(droite);
			break;
			default:
			return(erreur);
			break;

		}
	}
	return(erreur);
}

int trouver_position(int num){
	int oct;
	int pos;

	switch(num){
	//cas extreme gauche
		case 1:
		return(0);
		break;
		case 2:
		return(half);
		break;
		case 3:
		return(full);
		break;
	//cas extreme droite
		case(88):
		return(size_piano-full);
		break;
	//cas par défaut (une octave classique a 12 touches):
		default:
		pos = (num-3)%octave;
		oct=(num-3)/octave;
		int inter=2*full+full*oct*7;
		switch(pos){
			case 1:
			return(inter);
			break;
			case 2:
			return(inter+half);
			break;
			case 3:
			return(inter+full);
			case 4:
			return(inter+full+half);
			break;
			case 5:
			return(inter+2*full);
			break;
			case 6:
			return(inter+3*full);
			break;
			case 7:
			return(inter+3*full+half);
			break;
			case 8:
			return(inter+4*full);
			break;
			case 9:
			return(inter+4*full+half);
			break;
			case 10:
			return(inter+5*full);
			break;
			case 11:
			return(inter+5*full+half);
			break;
			case 0:
			return(inter-full);
			break;
			default:
			break;

		}
	}
	return(erreur);
}


void afficher_touche(int** T, int t, int k, SDL_Window* window, SDL_Renderer* renderer){
	int type_t=type_touche(T[t][k]);
	
	//cas pas de touches :
	if(type_t!=erreur){
		
	//init 
	SDL_Surface *image_t=NULL;
	SDL_Texture *touche_t=NULL;

	switch(type_t){//charger, puis afficher au bon endroit la touche à un instant donné pour la touche k
		case noire:

		image_t=SDL_LoadBMP("textures/touche_noire.bmp");

		if(image_t==NULL){error_img("chargement image touche noire", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche noire", renderer, window);}
		

		//affichage texture
		SDL_Rect rectangle_t;
	
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche noire", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);
		
		rectangle_t.y=0;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche noire", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		case droite:

		image_t=SDL_LoadBMP("textures/droite.bmp");

		if(image_t==NULL){error_img("chargement image touche droite", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche droite", renderer, window);}
		

		//affichage texture

	
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche droite", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);

		rectangle_t.y=(W_HEIGH-rectangle_t.h)/2;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche droite", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		case gauche:

		image_t=SDL_LoadBMP("textures/gauche.bmp");

		if(image_t==NULL){error_img("chargement image touche gauche", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche gauche", renderer, window);}
		

		//affichage texture

	
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche gauche", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);

		rectangle_t.y=(W_HEIGH-rectangle_t.h)/2;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche gauche", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		case ext_droite:

		image_t=SDL_LoadBMP("textures/extreme_droite.bmp");

		if(image_t==NULL){error_img("chargement image touche extreme_droite", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche extreme_droite", renderer, window);}
		

		//affichage texture
	
	
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche extreme_droite", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);

		rectangle_t.y=(W_HEIGH-rectangle_t.h)/2;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche extreme_droite", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		case ext_gauche:

		image_t=SDL_LoadBMP("textures/extreme_gauche.bmp");

		if(image_t==NULL){error_img("chargement image touche extreme_gauche", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche extreme_gauche", renderer, window);}
		

		//affichage texture
	
	
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche extreme_gauche", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);

		rectangle_t.y=(W_HEIGH-rectangle_t.h)/2;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche extreme_gauche", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		case centre:

		image_t=SDL_LoadBMP("textures/centre.bmp");

		if(image_t==NULL){error_img("chargement image touche centre", renderer, window);}
		touche_t=SDL_CreateTextureFromSurface(renderer, image_t);
		SDL_FreeSurface(image_t);
		if(touche_t==NULL){error_img("chargement texture touche centre", renderer, window);}
		

		//affichage texture
		
		if(SDL_QueryTexture(touche_t, NULL,NULL, &rectangle_t.w, &rectangle_t.h)!=0){error_img("Query touche centre", renderer, window);}

		if(trouver_position(T[t][k])==-1){error_img("erreur positionnement",renderer, window);}
		
		rectangle_t.x=trouver_position(T[t][k]);

		rectangle_t.y=(W_HEIGH-rectangle_t.h)/2;
		
		if(SDL_RenderCopy(renderer, touche_t, NULL, &rectangle_t)!=0){error_img("affichage texture touche centre", renderer, window);}

		break;
//------------------------------------------------------------------------------//
		default :
		break;

		}
	}
}