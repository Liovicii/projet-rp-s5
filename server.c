#include "dht.h"


/**
 * \fn void usage (char * arg)
 * \brief Affiche l'usage de la fonction sur la sortie d'erreur
 *
 * \param arg Nom du programme
 */
void usage(char * arg){
    fprintf(stderr, "usage: %s IP PORT\n", arg);
    exit(EXIT_FAILURE);
}



/**
 * \fn int main (int argc, int * argv[])
 * \brief Entrée du programme.
 *
 * \param argc Nombre de paramètres (+ nom du programme)
 * \param argv[] Tableau contenant les paramètres (+ nom du programme)
 * \return EXIT_SUCCESS arrêt normal, EXIT_FAILURE en cas d'erreur
 */
int main(int argc, int * argv[]){

    // initialisations des variables


    // vérification des arguments
    if(argc != 3){
        usage(argv[0]);
    }

    

    return EXIT_SUCCESS;

}
