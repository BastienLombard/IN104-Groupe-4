#include <stdio.h>
#include <stdlib.h>

struct wavfile {
    char id[4]; // doit contenir "RIFF"
    int totallength; // taille totale du fichier moins 8 octets
    char wavefmt[8]; // doit etre "WAVEfmt "
    int format; // 16 pour le format PCM
    short pcm; // 1 for PCM format
    short channels; // nombre de channels
    int frequency; // frequence d echantillonage
    int bytes_per_second; // nombre de octets par secondes
    short bytes_by_capture; // nombre de bytes par echantillon
    short bits_per_sample; // nombre de bit par echantillon
    char data[4]; // doit contenir "data"
    int bytes_in_data; // nombre de bytes de la partie data
} ;

struct soundfile{
    int fech;
    int size;                       // Nombre d'échantillons avec zero-padding
    int real_size;                  // Nombre d'échantillons sans le zero-padding
    double* data;
};

struct soundfile readfile() {
    int taille=1;                 
    int nbech=0;                    // Nombre d'echantillons extraits du fichier audio
    char fichieraudio[100];
    struct soundfile sample;

    printf ("entrer le nom du fichier audio a analyser :\n");
    scanf("%s", fichieraudio);

    FILE *wav = fopen(fichieraudio,"rb");       //ouverture du fichier wave

    struct wavfile header;                      //creation du header

    if ( wav == NULL ) {
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        exit(1);
    }

    //initialise le header par l'entete du fichier wave
    //verifie que le fichier possède un entête compatible

    if ( fread(&header,sizeof(header),1,wav) < 1 ) {
        printf("\nne peut pas lire le header\n");
        exit(1);
    }

    sample.fech=header.frequency ;

    if ( header.id[0] != 'R' || header.id[1] != 'I' || header.id[2] != 'F' || header.id[3] != 'F' ) {
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        exit(1);
    }

    if (header.channels!=1) {
        printf("\nerreur : le fichier n'est pas mono\n");
        exit(1);
    }

    nbech=(header.bytes_in_data/header.bytes_by_capture);
    sample.real_size=nbech;

    while (nbech>taille) {
        taille=taille*2;
    }

    sample.data=malloc(sizeof(double)*taille); // Correspond à du zero-padding car taille est la puissance de 2 la plus proche de nbech
    if (sample.data == NULL) {
        printf("erreur\n") ;
        exit(0);
    }

    sample.size=taille ;

    int i=0;
    short value=0;

    while( fread(&value,(header.bits_per_sample)/8,1,wav) ) {   //lecture des echantillons et enregistrement dans le tableau
        sample.data[i]=value;
        i++;
    }

    fclose(wav);

    return sample;
}
