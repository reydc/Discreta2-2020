#ifndef _EXTRAS_H
#define _EXTRAS_H

#include "veinteveinte.h"

// Un struct para uso temporal, que permite ordenar campos cuyo tipo son u32
typedef struct ParU32St {
    u32 coord1; // coordenada 1
    u32 coord2; // coordenada 2
} ParU32St;

typedef ParU32St* ParU32;

// Devuelve el número de colores del grafo.
u32 NumeroDeColores(Grafo G);

// Compara el primer campo de un struct compuesto por campos de tipo u32
// para usar en caso de ordenes crecientes
int ComparadorPrimerCampo(const void * a, const void * b);

// Compara el primer campo de un struct compuesto por campos de tipo u32
// para usar en caso de ordenes decrecientes
int ComparadorPrimerCampoDec(const void * a, const void * b);

// Comparador de números u32, del tipo que se pueden usar con el stdlib de C
int ComparadorU32(const void * a, const void * b);

// Intercambia los contenidos de dos punteros a enteros u32
void Swap(u32* a, u32* b);

#endif