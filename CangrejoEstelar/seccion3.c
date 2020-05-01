#include "veinteveinte.h"
#include "extras.h"
#include "constantes.h"

// Intercambia los colores i y j en el grafo dado
// 0 se da como éxito y 1 como falla.
char SwitchColores(Grafo G, u32 i, u32 j) {
    char exito = 1;
    u32 colores = NumeroDeColores(G);
    
    if (i < colores && j < colores) {
        u32 numero_vertices = NumeroDeVertices(G);
        for (u32 k = 0; k < numero_vertices && exito; k++) {
            // FijarColor devuelve 0 si tiene éxito
            if (Color(k, G) == i) {
                exito = exito && !FijarColor(j, k, G);
            } else if (Color(k, G) == j) {
                exito = exito && !FijarColor(i, k, G);
            }
        }
        // Si el loop da 1, devolvemos 0
        // Si el loop da 0, devolvemos 1
        exito = !exito;
    }
    
    return exito;
}

// Devuelve 1 si G es bipartito y 0 en caso contrario
char Bipartito(Grafo G) {
    u32 numero_vertices = NumeroDeVertices(G);
    
    // Vértice que nos indica si hemos coloreado o no al vértice i en el orden
    // dado en G durante BFS, ie nos indica si lo hemos visitado
    u32* coloreado = (u32*) calloc(numero_vertices, sizeof(u32));
    if (coloreado == NULL) return 1;

    // Simulamos una cola para hacer BFS
    u32* cola_bfs = (u32*) calloc(numero_vertices, sizeof(u32));
    if (cola_bfs == NULL) {
        free(coloreado);
        // El grafo ya estaba con un coloreo propio
        return 1;
    }

    u32 inicio_cola, final_cola;

    // Orden del vértice siendo procesado en el BFS, grado y orden del vecino
    u32 orden_vertice_bfs;
    u32 grado_vertice_bfs;
    u32 orden_vecino;

    // Para revisar la condición de sí es o no bipartito
    char bipartito = 1;
    for (u32 i = 0; i < numero_vertices && bipartito; i++) {
        if (!coloreado[i]) {
            // Si todavía no encontramos una condición que falsee la propiedad
            // de bipartición, procedemos como si procesaramos un grafo bipartito 

            // Iniciamos el encolado
            // inicio_cola siempre apunta a la posición primer elemento
            // en la cola &&
            // final_cola siempre apunta a la siguiente posición al
            // último elemento
            inicio_cola = 0;
            final_cola  = 0;

            cola_bfs[final_cola] = i;
            final_cola++;
            final_cola %= numero_vertices;
            
            coloreado[i] = 1;
            
            FijarColor(0, i, G);

            // Mientras hay un elemento en la cola
            while (inicio_cola != final_cola) {
                // Front
                orden_vertice_bfs = cola_bfs[inicio_cola];
                // Nuevo primer elemento
                inicio_cola++;
                inicio_cola %= numero_vertices;
                
                grado_vertice_bfs = Grado(orden_vertice_bfs, G);

                for (u32 j = 0; j < grado_vertice_bfs; j++) {
                    orden_vecino = OrdenVecino(j, orden_vertice_bfs, G);
                    
                    if (!coloreado[orden_vecino]) {
                        // Si aún no lo visitamos en BFS, lo debemos visitar y asignar el color
                        coloreado[orden_vecino] = 1;
                        
                        // Nuevo color a partir del color del padre en el árbol BFS
                        FijarColor(1 - Color(orden_vertice_bfs, G), orden_vecino, G);
                        
                        // Este vértice nunca fue procesado, luego se agrega a la cola
                        // Enqueue y nuevo último elemento
                        cola_bfs[final_cola] = orden_vecino;
                        final_cola++;
                        final_cola %= numero_vertices;

                    } else {
                        // Si está coloreado (por BFS) y el color es el mismo que el del vértice
                        // que estamos procesando, nos vamos de este esquema
                        if (Color(orden_vertice_bfs, G) == ColorVecino(j, orden_vertice_bfs, G)) {
                            // Esta parte anula el BFS para el resto.
                            bipartito = 0;
                            // Salgo de este esquema de coloreo
                            break;
                        }
                    }
                }
                if (!bipartito) break;
            }
        }
    }
    
    free(coloreado);
    free(cola_bfs);

    if (!bipartito) {
        // Si falla, lo coloreo con Greedy
        Greedy(G);
    }
    
    return bipartito;
}

// Corre Greedy en G comenzando desde el color 0
u32 Greedy(Grafo G) {
    u32 color;
    u32 maximo_color = 0;
    u32 numero_vertices = NumeroDeVertices(G);

    // Marcar los colores usados por los vecinos, si ya fueron procesados
    u32* color_usado;

    // Grado del i-ésimo vértice en el orden interno
    u32 grado_i;

    FijarColor(0, 0, G);

    for (u32 i = 1; i < numero_vertices; i++) {
        color = 0;
        grado_i = Grado(i, G);
        if (grado_i > 0) {
            // Tiene vecinos

            // Marcar los colores usados  por los vecinos
            // que están en el rango 0, ... , maximo_color
            // o sea se marca en un array[maximo_color + 1]
            // Esta estrategía sirvió para varios casos y es bastante simple,
            // tiende a ser lenta para casos como WP, en realidad me lo tiró más
            // rápido que hacerlo con memset
            color_usado = (u32*) calloc(maximo_color + 1, sizeof(u32));
            if (color_usado == NULL) return MAX_U32;

            // Marcar los colores de los vecinos
            // Si el vecino j es de menor orden que i, entonces su color es <= maximo_color
            for (u32 j = 0; j < grado_i; j++) {
                if (OrdenVecino(j, i, G) < i) {
                    color_usado[ ColorVecino(j, i, G) ] = 1;
                }
            }
            
            // Esto va bien, suponiendo que esto llega al último, de no ser igual
            // se deja en el color siguiente
            while (color <= maximo_color && color_usado[color]) { color++; }
            
            free(color_usado);
        }

        FijarColor(color, i, G);
        if (color > maximo_color) maximo_color = color;
    }

    // Tengo que tomar en cuenta al color 0
    return maximo_color + 1;
}
