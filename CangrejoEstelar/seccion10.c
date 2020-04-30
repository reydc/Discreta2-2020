#include "veinteveinte.h"

//Funciones para modificar datos de los v'ertices, char es para retornar error
//si se le pide algo fuera de rango o un alloc error.

// Asigna color x al vértice i del orden interno.
// Esta función es peligrosa y puede provocar que queden colores no asignados a ningún vértice.
// 0 se da como éxito y 1 como falla.
char FijarColor(u32 x,u32 i,Grafo G) {
    if (i < NumeroDeVertices(G)) {
        G->orden[i]->color = x;
        return 0;
    }
    return 1;
}

// Asigna en el lugar i del orden el vértice N-esimo del orden natural.
// Esta función es peligrosa y puede provocar que quede un orden interno que no sea un orden
// de todos los vértices o que haya vértices repetidos en el orden.
// 0 se da como éxito y 1 como falla.
char FijarOrden(u32 i,Grafo G,u32 N) {
    if (i < NumeroDeVertices(G) && N < NumeroDeVertices(G)) {
        G->orden[i] = G->vertice[N];
        G->orden[i]->orden = i;
        return 0;
    }
    return 1;
}