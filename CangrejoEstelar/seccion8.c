#include "veinteveinte.h"
#include "constantes.h"

#include <assert.h>

// Funciones de infoextract de vertices 
// Valor de retorno (2 a la 32)-1 para reportar errores, salvo para las de "Nombre" que no tienen
// forma de reportar errores, las otras no hay problema pues es imposible que (2 a la 32)-1 sea un
// color o un grado.


// Nombre real del i-ésimo vértice en el orden de G
u32 Nombre(u32 i, Grafo G) {
    return G->orden[i]->clave;
}

// Color del i-ésimo vértice en el orden de G
u32 Color(u32 i, Grafo G) {
    if (i >= NumeroDeVertices(G)) {
        return MAX_U32;
    }
    return G->orden[i]->color;
}

// Grado del i-ésimo vértice en el orden de G
u32 Grado(u32 i, Grafo G) {
    if (i >= NumeroDeVertices(G)) {
        return MAX_U32;
    }
    return G->orden[i]->grado;
}
