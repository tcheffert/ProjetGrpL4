#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// /!\ ATTENTION : vous devez garder ces deux include dans votre code.
// Ils remplacent <endian.h> et <semaphore.h> pour fournir une compatibilite
// avec macOS.
#include "include/portable_endian.h"
#include "include/portable_semaphore.h"

// Tant que vous n'avez pas vu la lecture des fichiers (module 5 INGInious),
// vous pouvez utiliser un fichier header pour definir vos graphes.
// Ce fichier a ete genere en utilisant le script fourni avec l'implementation
// Python.
#include "small_instance.h"

// Votre architecture de code doit etre propre et bien structuree.
// Nous vous demandons d'implementer un maximum des fonctionnalites independantes
// dans des fichiers differents.
// Par exemple, toutes les definitions de fonctions qui concernent
// la creation et manipulation de graphe devrait se retrouver dans include/graph.h
// et le code source lie dans src/graph.c.
// Vous devriez faire de meme pour tous les autres modules de votre code.
#include "include/graph.h" 

typedef struct {
    FILE *input_file;
    FILE *output_stream;
    uint8_t nb_threads;
    bool verbose;
} args_t;

void usage(char *prog_name) {
    fprintf(stderr, "UTILISATION:\n");
    fprintf(stderr, "    %s [OPTIONS] input_file\n", prog_name);
    fprintf(stderr,
            "    input_file: chemin vers le fichier d'instance representant le "
            "graphe a traiter.\n");
    fprintf(
        stderr,
        "    -f output_file: chemin vers le fichier qui contiendra le resultat "
        "de programme, au format specifie dans l'enonce. Defaut : stdout.\n");
    fprintf(stderr,
            "    -n n_threads: nombre de threads de calcul executant "
            "l'algorithme de plus court chemin. Defaut : 4.\n");
    fprintf(stderr,
            "    -v: autorise les messages de debug. Si ce n'est pas active, "
            "aucun message de ce type ne peut etre affiche, excepte les "
            "messages d'erreur en cas d'echec. Defaut : false.\n");
}

int parse_args(args_t *args, int argc, char *argv[]) {
    memset(args, 0, sizeof(args_t));

    // Valeurs pas defaut pour les arguments.
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    args->input_file = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "hn:vf:")) != -1) {
        switch (opt) {
            case 'n':
                args->nb_threads = atoi(optarg);
                if (args->nb_threads == 0) {
                    fprintf(stderr,
                            "Le nombre de threads de calcul doit etre superieur a 0 ! Recu : %s\n",
                            optarg);
                    return -1;
                }
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'f':
#ifdef LEPL1503_USE_HEADER_INSTANCE
                fprintf(stderr,
                        "Vous utilisez un fichier header comme instance de "
                        "graphe. Cela annule l'utilisation de l'argument donne "
                        "par -f. Veuillez retirer l'include au header si ce "
                        "n'est pas le comportement desire.\n");
                break;
#endif  // LEPL1503_USE_HEADER_INSTANCE
                args->output_stream = fopen(optarg, "w+");
                if (args->output_stream == NULL) {
                    fprintf(stderr,
                            "Impossible to open the output file %s: %s\n",
                            optarg, strerror(errno));
                    return -1;
                }
                break;
            case '?':
            case 'h':
                usage(argv[0]);
                return 1;
            default:
                usage(argv[0]);
                return -1;
        }
    }

#ifdef LEPL1503_USE_HEADER_INSTANCE
    fprintf(stderr,
            "Vous utilisez un fichier header comme instance de "
            "graphe. Cela annule l'utilisation du fichier d'instance en entree "
            "donne en argument. Veuillez retirer l'include au header si ce "
            "n'est pas le comportement desire.\n");
#else
    if (optind == argc) {
        fprintf(stderr,
                "Vous devez fournir un fichier d'instance en entree !\n");
        return -1;
    }

    if (NULL == (args->input_file = fopen(argv[optind], "r"))) {
        fprintf(stderr,
                "Impossible d'ouvrir le fichier qui contient l'instance de graphe %s: %s\n",
                argv[optind], strerror(errno));
        return -1;
    }
#endif  // LEPL1503_USE_HEADER_INSTANCE

    return 0;
}

int main(int argc, char *argv[]) {
    args_t args; 
    int err = parse_args(&args, argc, argv);
    if (err == -1) {
        exit(EXIT_FAILURE);
    } else if (err == 1) {
        exit(EXIT_SUCCESS);
    }

    // Vous remplacerez ce code par la lecture du fichier binaire
    // args.input_file apres le module 5 INGInious. En attendant, vous pouvez
    // acceder aux variables comme donne par l'exemple ci-dessous.
    if (args.verbose) {
        // Veillez a bien utiliser les macros PRI pour afficher vos valeurs.
        // Cela permet de gerer les differentes plateformes comme le raspberry.
        fprintf(stderr, "Nombre de noeuds: %" PRIu32 "\n", NB_NODES);
        fprintf(stderr, "Nombre de liens: %" PRIu32 "\n", NB_LINKS);
        for (int i = 0; i < NB_LINKS; ++i) {
            fprintf(stderr,
                    "Lien: %" PRIu32 " -> %" PRIu32 " (cout de %" PRIi32 ")\n",
                    (uint32_t)links[i][0], (uint32_t)links[i][1],
                    (int32_t)links[i][2]);
        }
    } 
    printf("Nombre de noeuds: %" PRIu32 "\n", NB_NODES);
    start(links, NB_LINKS, NB_NODES);

    if (args.input_file != NULL) {
        fclose(args.input_file);
    }

    if (args.output_stream != stdout) {
        fclose(args.output_stream);
    }
    return 0;
}