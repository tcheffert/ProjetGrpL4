#include <stdlib.h>
#include <stdio.h>
#include "../include/graph.h"

// TODO: implementez les fonctions liees a graph_t
// et declarees dans include/graph.h.

typedef struct BFResult {
    int64_t* dist;// dist[i] =distance from source to i
    int32_t* predecessor;//path
} BFResult_t;
typedef struct MaxResult {
    int64_t max_cost;
    int32_t max_node;
} MaxResult_t;
typedef struct Path {
    int size;
    int32_t * path;
} Path_t;

int64_t* mallocAndInitInt64Array(int size, int64_t initvalue) {
    int64_t* d = malloc(size*sizeof(int64_t));
    for(int i = 0; i<size; i++) {
        d[i]=initvalue;
    }
    return d;
}
void initInt64Array(int64_t* d, int size, int64_t initvalue) {
    for(int i = 0; i<size; i++) {
        d[i]=initvalue;
    }
}
void printInt64Array(char* name, int64_t* d, int size) {
    printf("%s [", name);     
    for (int i = 0; i < size; i++) {     
        printf("%ld, ", d[i]);     
    }  
    printf("]\n");     
}
int32_t* mallocAndInitInt32Array(int size, int32_t initvalue) {
    int32_t* d = malloc(size*sizeof(int32_t));
    for(int i = 0; i<size; i++) {
        d[i]=initvalue;
    }
    return d;
}
void initInt32Array(int32_t* d, int size, int32_t initvalue) {
    for(int i = 0; i<size; i++) {
        d[i]=initvalue;
    }
}
void printInt32Array(char* name, int32_t* d, int size) {
    printf("%s [", name);     
    for (int i = 0; i < size; i++) {     
        printf("%d ", d[i]);     
    }  
    printf("]\n");     
}
void computeMax(int64_t* dist, int32_t source, int32_t nb_nodes, MaxResult_t* maxResult) {
    //Retourne l'indice du noeud dont
    //      -il existe un chemin de `s` vers ce noeud et
    //      -le cout de ce chemin est le plus élevé parmis tous les noeuds ayant un chemin depuis `s`.
    int64_t max_cost = INT64_MIN;
    int32_t max_node = source;
    for (int node_idx = 0; node_idx < nb_nodes; node_idx++) {
        if (node_idx != source && dist[node_idx] != INT64_MAX && dist[node_idx] >= max_cost) {
            //# Permet de departager les noeuds destinations avec le meme plus court chemin etant le plus eleve.
            if (max_node == source || node_idx < max_node) {
                max_cost = dist[node_idx];
                max_node = node_idx;
                printf("   -->   max_node=%d    max_cost=%ld\n",max_node , max_cost);
            }
        }
    }
    if (max_cost == INT64_MIN && dist[source] != INT32_MAX && dist[source] >= max_cost) {
        max_cost = dist[source];
    }
    maxResult->max_cost=max_cost;
    maxResult->max_node=max_node;
}
void computePath(int32_t* predecessor, int32_t source, int32_t max_node, int32_t nb_nodes, Path_t* pathResult) {
//    Retourne une liste contenant
//      le chemin de `source` vers `dest`/max_node
//      en utilisant le tableau de précédence `path`.
    printInt32Array( "predecessor",predecessor, nb_nodes);

    int i = max_node;//dest;
    int size = 1;
    while (i != source) {
        size++;
        i = predecessor[i];
        printf("computePath %d\n",i);
    }
    int32_t* path = malloc(size* sizeof(int32_t));
    i = max_node;//dest;
    for(int j = size-1;j>=0; j--) {
        printf( "%d -> %d\n",i, predecessor[i]);
        path[j] = i;
        i = predecessor[i];
    }
    pathResult->path=path;
    pathResult->size=size;
} 
void bellman_ford( int64_t (* links)[3], int32_t nb_links, int32_t source, int32_t nb_nodes, BFResult_t* result) {
    int64_t* dist = mallocAndInitInt64Array(nb_nodes, INT64_MAX);
    dist[source]=0;//Le noeud source est à distance 0 de lui-meme
    int32_t* predecessor = mallocAndInitInt32Array(nb_nodes, -1);
    for (int i = 0; i < nb_nodes - 1; i++) {
        for (int j = 0; j < nb_links; j++) {
            int32_t node_from = links[j][0]; 
            int32_t node_to = links[j][1]; 
            int64_t cost = links[j][2]; 
            if (dist[node_from] != INT64_MAX && dist[node_to] > dist[node_from] + cost) {
                dist[node_to] = dist[node_from] + cost;
                predecessor[node_to] = node_from;
            }
        }
    }
    //Detection de cycle negatif.
    for (int j = 0; j < nb_links; j++) {
        int32_t node_from = links[j][0]; 
        int32_t node_to = links[j][1]; 
        int64_t cost = links[j][2]; 
        if (dist[node_from] != INT64_MAX && dist[node_to] > dist[node_from] + cost) {
            printf("Cycle négatif détecté\n");
            initInt64Array(dist, nb_nodes, INT64_MAX);
            dist[node_from] = 0;  //# Le noeud source est à distance 0 de lui-meme.
            initInt32Array(predecessor, nb_nodes, -1);
        }
    }
    result->dist=dist;
    result->predecessor=predecessor;
}
void freeResult(BFResult_t* bfResult, Path_t* pathResult) {
    free(bfResult->dist);
    free(bfResult->predecessor);
    free(pathResult->path);

}
void start(int64_t (* links)[3], int32_t nb_links, int32_t nb_nodes) {
    printf("----------------------START-----------------------\n");
    for (int source = 0; source < nb_nodes; source++) {
        printf("----------------------%d-----------------------\n", source);
        BFResult_t bfResult; 
        Path_t pathResult;
        bellman_ford(links, nb_links, source, nb_nodes, &bfResult);
        printInt64Array("Distances",bfResult.dist, nb_nodes);
        MaxResult_t maxResult;
        computeMax(bfResult.dist, source, nb_nodes, &maxResult);
        printf("\tdestination : %d\n", maxResult.max_node);
        printf("\tcout : %ld\n", maxResult.max_cost);
        computePath(bfResult.predecessor, source, maxResult.max_node, nb_nodes, &pathResult);
        printf("\tnombre de noeuds : %d\n", pathResult.size);
        printInt32Array("\tpath",pathResult.path, pathResult.size);
        freeResult(&bfResult, &pathResult);
    }
}

