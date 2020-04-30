#include "veinteveinte.h"
#include "constantes.h"
#include "extras.h"

// Cuenta el total de colores usados en el grafo G
u32 NumeroDeColores(Grafo G) {
    u32 maximo_color = 0;
    u32 numero_vertices = NumeroDeVertices(G);

    for (u32 i = 0; i < numero_vertices; i++) {
        if (Color(i, G) > maximo_color) maximo_color = Color(i, G);
    }
    
    // Debería ser >= 2, ya que siempre hay al menos una arista
    return maximo_color + 1;
}

// Compara el primer campo de un struct compuesto por campos de tipo u32
// para usar en caso de ordenes crecientes
int ComparadorPrimerCampo(const void * a, const void * b) {
    ParU32 temp_a = (ParU32) a;
    ParU32 temp_b = (ParU32) b;
    if (temp_a->coord1 <  temp_b->coord1) return -1;
    if (temp_a->coord1 == temp_b->coord1) return 0;
    return 1;
}

// Compara el primer campo de un struct compuesto por campos de tipo u32
// para usar en caso de ordenes decrecientes
int ComparadorPrimerCampoDec(const void * a, const void * b) {
    ParU32 temp_a = (ParU32) a;
    ParU32 temp_b = (ParU32) b;
    if (temp_a->coord1 <  temp_b->coord1) return 1;
    if (temp_a->coord1 == temp_b->coord1) return 0;
    return -1;
}

// Comparador de números u32, del tipo que se pueden usar con el stdlib de C
int ComparadorU32(const void * a, const void * b) {
    u32 temp_a = *(u32*)a;
    u32 temp_b = *(u32*)b; 
    if (temp_a <  temp_b) return -1;
    if (temp_a == temp_b) return 0;
    return 1;
}

// Intercambia los contenidos de dos punteros a enteros u32
void Swap(u32* a, u32* b) {
    u32 temp = *a;
    *a = *b;
    *b = temp;
}
