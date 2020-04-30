#include "GrafoSt2020.h"
#include "estructuras_datos_extras.h"

#include <assert.h>

// Calcula el factor de densidad de un grafo no dirigido
// Densidad = 2*(|E(G)|) / (|V|*(|V|-1))
// Factor de Densidad = int[ |V|* Densidad ] = int[ 2*(|E(G)|) / (|V|-1) ]
u32 NuevoFactorDensidad(Grafo G) {
    assert(G != NULL);
    double fac = (double)(2 * NumeroDeLados(G)) / (double)(NumeroDeVertices(G) - 1); 
    return  (u32)(fac > 0 ? fac : 1);
}

/* Funciones para alocar el array de vecinos */
// Observar que son prácticamente iguales estas funciones con las de más abajo

// Crea una nuevo array de vecinos
Vertice* NuevaListaVecinos(u32 largo) {
    Vertice* vecinos = (Vertice*) calloc(largo, sizeof(Vertice));
    if (vecinos == NULL) return NULL;
    return vecinos;
}

// Este comparador se usa durante la construcción del grafo
int ComparadorNombresVertices(const void* a, const void* b) {
    Vertice temp_a = *(Vertice*)a;
    Vertice temp_b = *(Vertice*)b;
    if (temp_a->clave <  temp_b->clave) return -1;
    if (temp_a->clave == temp_b->clave) return 0;
    return 1;
}

/* Analogos a los vértices sólo que deseo separar más funcionalidad */

// Crea una nueva lista de nodos
// factor_densidad de entrada se calcula una única vez al principio de la carga del
// grafo
Nodo* NuevaListaNodos(u32 factor_densidad) {
    Nodo* vecinos = (Nodo*) calloc(factor_densidad, sizeof(Nodo));
    if (vecinos == NULL) return NULL;
    return vecinos;
}

// Insertar un vecino en una lista de vecinos
Nodo InsertarNodo(Nodo vertice, Nodo vecino) {
    assert(vertice != NULL);
    assert(vecino  != NULL);
    if (vertice->grado < vertice->longitud_vecinos) {
        vertice->vecinos[vertice->grado++] = vecino;
    } else {
        vertice->vecinos = (Nodo*) realloc(vertice->vecinos, 2 * vertice->longitud_vecinos * sizeof(Nodo));
        if (vertice->vecinos == NULL) return NULL;
        vertice->longitud_vecinos = 2 * vertice->longitud_vecinos;
        vertice->vecinos[vertice->grado++] = vecino;
    }
    return vecino;
}

/* LISTAS DE COLISIÓN TEMPORALES */

// Construir una nueva lista
Lista NuevaLista() {
    Lista lista = (Lista) calloc(1, sizeof(ListaSt));
    lista->largo = 0;
    lista->cabeza = NULL;
    lista->cola = NULL;
    return lista;
}

// Inserta los datos respecto a una clave en la lista L
// Al retornar chequeamos que lo que retorna no sea NULL

Nodo InsertarLista(Lista L, u32 clave, u32 grado, u32 color, u32 factor_densidad, u32 orden, Nodo siguiente) {
    assert(L != NULL);
    if (!L->largo) {
        // Insertamos un nodo por primera vez en L
        L->cabeza = (Nodo) calloc(1, sizeof(NodoSt));
        if (L->cabeza == NULL) return NULL;
        L->largo++;
        
        L->cabeza->clave = clave;
        L->cabeza->grado = grado;
        L->cabeza->color = color;
        L->cabeza->orden = orden;
        L->cabeza->longitud_vecinos = factor_densidad;
        // Creamos la lista de adyacencia
        L->cabeza->vecinos = NuevaListaNodos(factor_densidad);
        if (L->cabeza->vecinos == NULL) return NULL;

        L->cabeza->siguiente = siguiente;
        
        // Ahora la cabeza y la cola apuntan al mismo elemento
        L->cola = L->cabeza;
    } else {
        // Ya hay nodos en L
        Nodo ultimo = L->cola;
        
        ultimo->siguiente = (Nodo) calloc(1, sizeof(NodoSt));
        if (ultimo->siguiente == NULL) return NULL; 
        L->largo++;
        
        ultimo->siguiente->clave = clave;
        ultimo->siguiente->grado = grado;
        ultimo->siguiente->color = color;
        ultimo->siguiente->orden = orden;
        ultimo->siguiente->longitud_vecinos = factor_densidad;
        // Creamos la lista de adyacencia
        ultimo->siguiente->vecinos = NuevaListaNodos(factor_densidad);
        if (ultimo->siguiente->vecinos == NULL) return NULL;

        ultimo->siguiente->siguiente = siguiente;
        // el último deja de ser último
        L->cola = ultimo->siguiente;
    }
    #ifdef DEBUG_CONSTRUCCIONGRAFO
    printf("InsertarLista: L->largo == %u, clave == %u\n", L->largo, clave);
    #endif
    return L->cola;
}

// Buscar en la lista L el nodo asociado a clave
Nodo BuscarEnLista(Lista L, u32 clave) {
    Nodo correcto = L->cabeza;
    u32 i = 0;
    while (i < L->largo && correcto->clave != clave) {
        correcto = correcto->siguiente;
        i++;
    }
    if (i == L->largo) return NULL;
    return correcto;
}

// Dealocar la lista L
void DestruirLista(Lista L) {
    if (L != NULL && L->largo) {
        Nodo actual  = L->cabeza;
        Nodo proximo = actual->siguiente;
        free(actual->vecinos);
        free(actual);
        // Hasta que no quede un nodo, liberar
        while (proximo != NULL) {
            actual = proximo;
            proximo = actual->siguiente;
            free(actual->vecinos);
            free(actual);
        }
    }
    free(L);
}

// Función hash
u32 Hash(u32 clave, u32 modulo) {
    return clave%modulo;
}
