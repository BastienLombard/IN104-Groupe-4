#ifndef TRAITEMENT_H
#define TRAITEMENT_H

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_fft_complex.h>
#include "readfile.h"

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])
#define MODULE(z,i) (sqrt(REAL(z,i)*REAL(z,i)+IMAG(z,i)*IMAG(z,i)))

#define FONDA 27.5
#define PAS pow(2.0,1.0/12)

struct transcription {
    int** signal;
    int length;
    double Tech;
} ;

int note_detection(double* tab, int size, double max) ;

int nearest_piano_key (double freq) ;

double* conversion(double* T, long int size) ;

long int argmax_ps(double* T, long int size, long int i_min, long int i_max) ;

double max_fft(double* lst, long int size, long int i_min, long int i_max) ;

double max_lst(double* tab, int size) ;

double* pitch_finder(struct soundfile sample) ;

struct transcription traitement () ;

#endif