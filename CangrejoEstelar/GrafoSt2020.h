#ifndef _GRAFOST_H
#define _GRAFOST_H

/* VERSION DE TESTEO */

typedef unsigned int u32;

/*  VerticeSt
    Esta estructura guarda info de un vértice 
*/

typedef struct VerticeSt {
    u32 clave;
    u32 grado;
    u32 color;
    u32 orden;
    // Array de vecinos
    struct VerticeSt ** vecinos;
} VerticeSt;

typedef VerticeSt* Vertice;

/*  GrafoSt
    Estructura que sirve para guardar el grafo propiamente dicho.
    Información guardada:
    Número de vértices
    Número de lados
    Máximo grado
    Vertices
    Orden
*/
typedef struct GrafoSt {
    // Número de vertices y lados
    u32 numero_vertices;
    u32 numero_lados;
    // Delta grande
    u32 maximo_grado;
    // Array de vértices
    Vertice* vertice;
    // Orden de procesamiento para los algoritmos
    Vertice* orden;
} GrafoSt;

#endif