#include "veinteveinte.h"

#include <assert.h>

u32 NumeroDeVertices(Grafo G) {
    return G->numero_vertices;
}

u32 NumeroDeLados(Grafo G) {
    return G->numero_lados;
}

u32 Delta(Grafo G) {
    return G->maximo_grado;
}
