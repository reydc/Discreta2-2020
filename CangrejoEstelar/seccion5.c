#include "veinteveinte.h"
#include "constantes.h"

// Devuelve el número de componentes conexas de G.
u32 NumCCs(Grafo G) {
    // El valor MAX_U32 es imposible y lo tiro en caso de que falle
    // calloc por que no habia nada especificado al respecto
    
    u32 componentes_conectadas = 0;
    u32 numero_vertices = NumeroDeVertices(G);

    // Marcamos los visitados, según el orden interno
    u32* visitados = (u32*) calloc(numero_vertices, sizeof(u32));
    if (visitados == NULL) return MAX_U32;

    // Simulamos un stack para hacer DFS
    u32* stack_dfs = (u32*) calloc(numero_vertices, sizeof(u32));
    if (stack_dfs == NULL) {
        free(visitados);
        return MAX_U32;
    }
    // Siguiente índice libre en el stack
    // top == 0 sii el stack está vacío
    u32 top;

    // orden del vértice siendo procesado por dfs, grado y el orden del vecino
    u32 orden_vertice_dfs;
    u32 grado_vertice_dfs;
    u32 orden_vecino;

    // Visitar según el orden interno guardado en G
    for (u32 i = 0; i < numero_vertices; i++) {
        if (!visitados[i]) {
            componentes_conectadas++;
            
            top = 0;
            
            visitados[i] = 1;
            
            stack_dfs[top] = i;
            top++;

            while (top) {
                // Top del stack
                orden_vertice_dfs = stack_dfs[top-1];                
                // Pop
                top--;

                grado_vertice_dfs = Grado(orden_vertice_dfs, G);
                for (u32 j = 0; j < grado_vertice_dfs; j++) {
                    orden_vecino = OrdenVecino(j, orden_vertice_dfs, G);
                    if (!visitados[orden_vecino]) {
                        // Visitado en el orden
                        visitados[orden_vecino] = 1;

                        // Push del vecino
                        stack_dfs[top] = orden_vecino;
                        top++;
                    }
                }
            }
        }
    }

    free(visitados);
    free(stack_dfs);

    return componentes_conectadas;
}
