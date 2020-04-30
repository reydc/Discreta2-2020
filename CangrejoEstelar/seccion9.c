#include "veinteveinte.h"
#include "constantes.h"

// Devuelve el color del vécino número j del vértice número i en el orden guardado
// en ese momento en G.
u32 ColorVecino(u32 j,u32 i,Grafo G) {
    if (i >= NumeroDeVertices(G)) return MAX_U32;
    if (j >= Grado(i,G))          return MAX_U32;
    return G->orden[i]->vecinos[j]->color;
}

// Devuelve el nombre del vécino número j del vértice número i en el orden guardado
// en ese momento G.
u32 NombreVecino(u32 j,u32 i,Grafo G) {
    return G->orden[i]->vecinos[j]->clave;
}

// El resultado es igual a k sii el vécino j-ésimo del i-ésimo vértice de G en el
// orden guardado en G es el k-ésimo vértice del orden guardado en G.
u32 OrdenVecino(u32 j,u32 i,Grafo G) {
    return G->orden[i]->vecinos[j]->orden;
}