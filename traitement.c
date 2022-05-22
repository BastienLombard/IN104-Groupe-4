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


// Retourne 1 si une note est jouée sur l'intervalle considéré, et 0 sinon
int note_detection(double* tab, int size, double max) {
    int detec=0;
    double moy=0;
    int seuil=150;                                  // Seuil de détection d'une note en temporel

    for (int i=0; i<size; i++){
        moy=moy+fabs(tab[i])/size;
    }

    if (moy>max/seuil) {
        detec=1;
    }
    return detec;
}



// Prends en arument une fréquence et renvoie le numéro de la touche la plus proche sur un piano
int nearest_piano_key (double freq) {       
    double pas = PAS;
    double f=FONDA;

    if (freq<f || freq>f*pow(pas,87)) {
        return 0;
    }

    int key=1;
    while (f<freq && key<=88 && freq>=FONDA) {
        f=f*pas;
        key++;
    }

    if (fabs(freq-f)>fabs(freq-f/pas)) {
        key--;
    }
    return(key);
}



// Convertit un signal réel en un signal complexe (Re aux indices pairs, Im aux indices impairs)
double* conversion(double* T, long int size) {
    double* C=malloc(2*size*sizeof(double)) ;
    if (C==NULL) {            
        printf("erreur\n") ;
        exit(0);
    }

    for (int i=0; i<size; i++) {
        REAL(C,i)=T[i] ;
        IMAG(C,i)=0 ;
    }
    return C ;
}



// Trouve l'argmax du produit spectral de T
long int argmax_ps(double* T, long int size, long int i_min, long int i_max) {
    int H=3;                    //Nombre d'harmoniques considérées dans le produit spectral

    long int argmax=i_min;
    long int l=i_min;
    int n=1;
    double ps_max=T[l];

    while((n+1)*l<size && n<H) {
        n=n+1;
        l=n*i_min;
        ps_max=ps_max*T[l];
    }

    for (int i = i_min+1; i < i_max; i++) {
        int f=i;
        int k=1;
        double ps=T[f];

        while((k+1)*f<size && k<H) {
            k=k+1;
            f=k*i;
            ps=ps*T[l];
        }

        if (ps>ps_max) {
            argmax=i;
            ps_max=ps;
        }
    }
    return argmax;
}


// Renvoie le max d'un tableau entre l'indice i_min et i_max
double max_fft(double* lst, long int size, long int i_min, long int i_max) {
    double max=lst[i_min];

    for (long int i=i_min+1; i<i_max; i++) {
        if (lst[i]>max) {
            max=lst[i];
        }
    }
    return max;
}



// Renvoie le max global (en valeur absolue) dans un tableau
double max_lst(double* tab, int size) {
    double max=fabs(tab[0]);
    
    for (int i=1; i<size; i++) {
        if (fabs(tab[i])>max) {
            max=fabs(tab[i]);
        }
    }
    return max;
}


// Renvoie un tableau des différentes notes reconnues dans le signal en entrée
double* pitch_finder(struct soundfile sample) {
    long int size=sample.size ;


    double* list_freq=malloc(10*sizeof(double));
    if (list_freq==NULL) {            
        printf("erreur\n") ;
        exit(0);
    }

    double* data_complex=conversion(sample.data, size) ;

    for (int i = 0; i<size; i++) {
        REAL(data_complex,i)=REAL(data_complex,i)*(0.54+0.46*sin(2*M_PI*i/size)) ;        // Fenétrage du signal
    }

    gsl_fft_complex_radix2_forward(data_complex, 1, size) ;

    double* data_module=malloc(sizeof(double)*size/2);
    if (data_module==NULL) {            
        printf("erreur\n") ;
        exit(0);
    }

    for (int i = 0; i < size/2; i++) {
        data_module[i]=MODULE(data_complex,i)/size;
    }

    long int i_min=25*size/sample.fech;       // Les indices en dehors de [i_min;i_max] correspondent à des fréquences
    long int i_max=4500*size/sample.fech;     // inatteignables sur un piano
    
    long int argmax;
    double f_0;
    
    int pas_suppression = size/200 ;          // Correspond au nombre d'échantillons à supprimer autour des harmoniques

    double max=max_fft(data_module,size/2,i_min,i_max);

    int j=0;

    while (1) {
        argmax=argmax_ps(data_module, size/2, i_min, i_max);
        f_0=argmax*sample.fech*1.0/size;

        if (data_module[argmax]<max/3) {
            break;
        }

        else {
            list_freq[j]=f_0;
            j++;

            long int n=1; // Numéro de l'harmonique considérée

            while (n*argmax+pas_suppression/2<size/2 && n<30) {
                for (int i=-pas_suppression/2 ; i<=pas_suppression/2 ; i++) {
                    if (n*argmax + i>0) {
                        data_module[n*argmax + i]=0;
                    }
                }  
            n=n+1;
            }
        }
    }

    // On rempli le reste du tableau par la fréquence nulle 
    // Cela correspondra au fait qu'aucune autre note n'est jouée dans cet extrait
    for(int p=j; p<10; p++) {
        list_freq[p]=0.0;
    }

    free(data_module);

    return list_freq ;
}



struct transcription traitement () {
    struct soundfile sample = readfile() ;      // Ouverture du fichier audio

    struct transcription T;

    long int pas=4096*4;                        // Taille des sous-signaux

    T.Tech=pas*1.0/sample.fech;

    long int n=0;
    while ((n+1)*pas<sample.real_size) {
        n++;                                    // n est le nombre "d'instants" dans le signal transcrit (MIDI)
    }
    T.length=n;

    // MIDI est un tableau 10xn contenant les numéros des touches jouées (au plus 10) à chaque instant du signal transcrit
    int** MIDI=malloc(n*sizeof(int*));              
    for (int i=0; i<n; i++) {
        MIDI[i]=malloc(10*sizeof(int));
    }

    struct soundfile extract ;
    extract.fech=sample.fech ;
    extract.size=pas;
    extract.data=malloc(sizeof(double)*pas) ;

    double max=max_lst(sample.data, sample.real_size);
    
    for (int i=0; i<n; i++) {                             // On boucle sur les découpages réguliers succéssifs de sample
        for (int n=0; n<pas; n++) {             
            extract.data[n]=sample.data[n+i*pas];
        }

        if (note_detection(extract.data, pas, max)==1) {
            double* list_freq=pitch_finder(extract) ;

            for (int j=0; j<10; j++) {
                MIDI[i][j]=nearest_piano_key(list_freq[j]);
            }
            free(list_freq);
        }

        else {
            for (int j=0; j<10; j++) {
                MIDI[i][j]=0;                             // 0 signifie qu'aucune note n'est jouée à l'instant i
            }
        }
    }

    free(sample.data);

    T.signal=MIDI;
    return T;
}