#ifndef _EDE_H
#define _EDE_H

#include "veinteveinte.h"

// TODO ESTO SIRVE PARA USAR AL PRINCIPIO, DESPUÉS SE IGNORA

// Funciones para alocar el array de vecinos

// Crea una nuevo array de vecinos
Vertice* NuevaListaVecinos(u32 largo);

// Este comparador se usa durante la construcción del grafo
int ComparadorNombresVertices(const void* a, const void* b);

// LISTAS DE COLISIÓN
// Estructura que se usará para la carga, después se descarta

// Esta es una estructura que se parece a los vértices pero que se usa temporalmente
typedef struct NodoSt {
    u32 clave;
    u32 grado;
    u32 color;
    u32 orden;
    u32 longitud_vecinos;
    // Array de vecinos (Un array de nodos)
    struct NodoSt ** vecinos;
    struct NodoSt *siguiente;
} NodoSt;

typedef NodoSt* Nodo;

// Hay más funcionalidad en los nodos, pero es por que es una estructura temporal

// Crea una nuevo array de vecinos
Nodo* NuevaListaNodos(u32 factor_densidad);

// Insertar un vecino en el array de vecinos
Nodo InsertarNodo(Nodo vertice, Nodo vecino);

/* ListaSt
   Esta estructura representaría una lista para evitar colisiones por medio
   de la función hash (módulo sobre la cantidad de vértices por una constante).
*/

typedef struct ListaSt {
    u32  largo;
    Nodo cabeza;
    Nodo cola;
} ListaSt;

typedef ListaSt* Lista;

Lista NuevaLista();

Nodo InsertarLista(Lista L, u32 clave, u32 grado, u32 color, u32 factor_densidad, u32 orden, Nodo siguiente);

Nodo BuscarEnLista(Lista L, u32 clave);

void DestruirLista(Lista L);

u32 Hash(u32 clave, u32 modulo);

/* AUXILIARES */

// Calcula el factor de densidad de un grafo no dirigido
// Densidad = 2*(|E(G)|) / (|V|*(|V|-1))
// Factor de Densidad = int[ |V|* Densidad ] = int[ 2*(|E(G)|) / (|V|-1) ]
u32 NuevoFactorDensidad(Grafo G);

#endif
