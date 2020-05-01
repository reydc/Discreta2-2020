#include "veinteveinte.h"
#include "extras.h"
#include "constantes.h"

// Todas estas funciones reportan las fallas con 1 y los éxitos con 0

// Orden Welsh Powell: por grados del mayor al menor
char WelshPowell(Grafo G) {
    u32 numero_vertices = NumeroDeVertices(G);

    // Para ordenar los grados y conservar los nombres
    ParU32 grado_nombre = (ParU32) calloc(numero_vertices, sizeof(ParU32St));
    if (grado_nombre == NULL) return 1;

    // Para ordenar los nombres reales y poder buscar su orden natural
    u32* nombres_reales = (u32*) calloc(numero_vertices, sizeof(u32));
    if (nombres_reales == NULL) {
        free(grado_nombre);
        return 1;
    }

    for (u32 i = 0; i < numero_vertices; i++) {
        grado_nombre[i].coord1 = Grado(i, G);
        grado_nombre[i].coord2 = nombres_reales[i] = Nombre(i, G);
    }

    // Ordenamos por grado de forma decreciente
    qsort(grado_nombre, numero_vertices, sizeof(ParU32St), ComparadorPrimerCampoDec);
    
    // Damos el orden natural a los nombres reales, ya que no tengo forma de
    // saber la posición en donde caen
    qsort(nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);

    // Acomodar segun el orden obtenido
    u32 clave;
    u32* posicion_natural = NULL;
    for (u32 i = 0; i < numero_vertices; i++) {
        clave = grado_nombre[i].coord2;
        posicion_natural = (u32*) bsearch(&clave, nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);
        FijarOrden(i, G, (u32)(posicion_natural - nombres_reales));
    }
    
    free(grado_nombre);
    free(nombres_reales);

    return 0;
}

// Revierte los bloques de colores
char RevierteBC(Grafo G) {
    u32 numero_vertices = NumeroDeVertices(G);

    // Para ordenar los colores
    ParU32 color_nombre = (ParU32) calloc(numero_vertices, sizeof(ParU32St));
    if (color_nombre == NULL) return 1;

    // Para ordenar los nombres reales y poder buscar su orden natural
    u32* nombres_reales = (u32*) calloc(numero_vertices, sizeof(u32));
    if (nombres_reales == NULL) {
        free(color_nombre);
        return 1;
    }

    for (u32 i = 0; i < numero_vertices; i++) {
        color_nombre[i].coord1 = Color(i, G);
        color_nombre[i].coord2 = nombres_reales[i] = Nombre(i, G);
    }

    // Ordenamos por color de forma decreciente
    qsort(color_nombre, numero_vertices, sizeof(ParU32St), ComparadorPrimerCampoDec);
    
    // Damos el orden natural a los nombres reales, ya que no tengo forma de
    // saber la posición en donde caen
    qsort(nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);

    // Acomodar segun el orden obtenido
    u32 clave;
    u32* posicion_natural = NULL;
    for (u32 i = 0; i < numero_vertices; i++) {
        clave = color_nombre[i].coord2;
        posicion_natural = (u32*) bsearch(&clave, nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);
        FijarOrden(i, G, (u32)(posicion_natural - nombres_reales));
    }
    
    free(color_nombre);
    free(nombres_reales);

    return 0;
}

// Ordena por cardinalidad de los bloques de colores, del mas chico al mas grande
char ChicoGrandeBC(Grafo G) {
    u32 numero_vertices = NumeroDeVertices(G);
    u32 delta = Delta(G);

    // Para ordenar los colores, aunque en esta función no ordenamos esto
    // sino que lo preservamos para aplicar los cambios al orden interno
    // que se va a modificar
    ParU32 color_nombre = (ParU32) calloc(numero_vertices, sizeof(ParU32St));
    if (color_nombre == NULL) return 1;

    // Para ordenar los nombres reales y poder buscar su orden natural
    u32* nombres_reales = (u32*) calloc(numero_vertices, sizeof(u32));
    if (nombres_reales == NULL) {
        free(color_nombre);
        return 1;
    }
    // Contemos los colores: calloc llama con delta + 1 por los colores que
    // se pueden usar (es decir, se podría usar {0,1,...,Delta}) para cubrir
    // los casos posibles, recordando que durante la construcción del grafo
    // se usó Greedy
    // La primera coordenada tiene el contador del color y la segunda el color 
    ParU32 contador_color = (ParU32) calloc(delta + 1, sizeof(ParU32St));
    if (contador_color == NULL) {
        free(color_nombre);
        free(nombres_reales);
        return 1;
    }

    // Para ordenar los bloques eficientemente necesitamos saber el
    // mayor nombre de color usado, ya que va a haber casos en el que
    // el coloreo que se tiene actualmente puede ser muchísimo menor que
    // que el delta 
    u32 maximo_color_usado = 0;

    u32 color_vertice = 0;
    for (u32 i = 0; i < numero_vertices; i++) {
        // Para ordenar juntando los colores
        color_nombre[i].coord1 = color_vertice = Color(i, G);
        color_nombre[i].coord2 = nombres_reales[i] = Nombre(i, G);
        
        if (contador_color[color_vertice].coord1 == 0) {
            // Esto va a servir para no perder el color cuando se haya ordenado por
            // cardinalidad
            contador_color[color_vertice].coord1 = 1;
            contador_color[color_vertice].coord2 = color_vertice;
        } else {
            // Si resulta que ya está señalizado el color, incrementamos el contador
            contador_color[color_vertice].coord1++;
        }
        // Vemos si es el mayor nombre de color usado
        if (color_vertice > maximo_color_usado) maximo_color_usado = color_vertice;
    }

    // Para calcular la posicion en el orden interno, necesito la posicion 
    // base para los vértices de un color dado
    // posicion_base_color[i] == posicion donde comienza el bloque de color i
    // en el orden interno del grafo
    u32* posicion_base_color = (u32*) calloc(maximo_color_usado + 1, sizeof(u32));
    if (posicion_base_color == NULL) {
        free(color_nombre);
        free(nombres_reales);
        free(contador_color);
        return 1;
    }

    // Este array dinámico cumple: contador_en_orden_por_color[c] == k
    // sii hay k vértices de color c que se han fijado en las posiciones
    // correspondientes según la especificación de ChicoGrandeBC
    u32* contador_en_orden_por_color = (u32*) calloc(maximo_color_usado + 1, sizeof(u32));
    if (contador_en_orden_por_color == NULL) {
        free(color_nombre);
        free(nombres_reales);
        free(contador_color);
        free(posicion_base_color);
        return 1;
    }

    // Si es muy chico en comparación con Delta+1
    contador_color = (ParU32) realloc(contador_color, (maximo_color_usado + 1) * sizeof(ParU32St));
    if (contador_color == NULL) {
        free(color_nombre);
        free(nombres_reales);
        free(contador_color);
        free(posicion_base_color);
        free(contador_en_orden_por_color);
        return 1;
    }
    

    // Ordenamos los colores por cardinalidad (tenemos una permutación de
    // los bloques con los pares cumpliendo
    // (|Vj0|,j0) <= (|Vj1|,j1) <= ... <= (|Vj(maximo_color_usado)|,j(maximo_color_usado))
    qsort(contador_color, maximo_color_usado + 1, sizeof(ParU32), ComparadorPrimerCampo);
    
    // Buscamos las posiciones base en el orden interno, por color
    posicion_base_color[contador_color[0].coord2] = 0;
    // Siempre va a haber al menos dos colores
    for (u32 i = 1; i <= maximo_color_usado; i++) {
        posicion_base_color[contador_color[i].coord2] = 
                    posicion_base_color[contador_color[i-1].coord2] + 
                    contador_color[i-1].coord1;
    }

    // Damos el orden natural a los nombres reales, ya que no tengo forma de
    // saber la posición en donde caen
    qsort(nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);


    // Ahora tenemos que "transferir" los cambios al orden interno
    // Tengo que pasar las posiciones divididas por color y asegurar algo como
    // la siguiente división de índices del orden
    // [0            , ... , |Vj0| - 1                ]   -> vértices con color j0, posición base == 0
    // [|Vj0|        , ... , |Vj0| + |Vj1| - 1        ]   -> vértices con color j1, posición base == 0 + |Vj0|
    // [|Vj0| + |Vj1|, ... , |Vj0| + |Vj1| + |Vj2| - 1]   -> vértices con color j2, posición base == 0 + |Vj0| + |Vj1| 
    // ...
    // ...
    // [0 + ... + |Vj(maximo_color_usado-1)|, ... , |Vj(maximo_color_usado-1)| + |Vj(maximo_color_usado)| - 1]
    // -> vértices con color j(maximo_color_usado), posición base == 0 + |Vj0| + ... + |Vj(maximo_color_usado-1)|
    
    u32 clave, color, posicion_en_orden_interno;
    u32* posicion_natural = NULL;
    for (u32 i = 0; i < numero_vertices; i++) {
        clave = color_nombre[i].coord2;
        color = color_nombre[i].coord1;
        posicion_natural = (u32*) bsearch(&clave, nombres_reales, numero_vertices, sizeof(u32), ComparadorU32);
        posicion_en_orden_interno = posicion_base_color[color] + contador_en_orden_por_color[color];
        contador_en_orden_por_color[color]++;
        FijarOrden(posicion_en_orden_interno, G, (u32)(posicion_natural - nombres_reales));
    }

    free(color_nombre);
    free(nombres_reales);
    free(contador_color);
    free(posicion_base_color);
    free(contador_en_orden_por_color);

    return 0;
}

// Pseudo-aleatoriza vertices usando semilla R.
char AleatorizarVertices(Grafo G, u32 R) {
    u32 numero_vertices = NumeroDeVertices(G);

    // Inicializar el generador pseudo-aleatorio
    srand(R);
    
    // Array dinámico que tiene los indices naturales de los vértices,
    // serán reordenados por medio del algoritmo de Fisher-Yates
    u32* indices_naturales = (u32*) calloc(numero_vertices, sizeof(u32));
    if (indices_naturales == NULL) return 1;

    // La especificación dice 
    // "IMPORTANTE: debe dar el mismo resultado independientemente del coloreo o
    // del orden que tenga en ese momento G. Es decir, ..."
    // También puede pasar por encima, tranquilamente, sobre el orden actual
    // por que no importa. Entonces hago el reordenamiento para los indices de los
    // vértices en orden natural
    for (u32 i = 0; i < numero_vertices; i++) {
        indices_naturales[i] = i;
    }

    // Reordenamiento "aleatorio" por Fisher-Yates
    u32 j;
    for (u32 i = numero_vertices - 1; i > 0; i--) {
        j = ((u32) rand()) % (i + 1);
        Swap(indices_naturales + i, indices_naturales + j);
    }
    
    for (u32 i = 0; i < numero_vertices; i++) {
        FijarOrden(i, G, indices_naturales[i]);
    }

    free(indices_naturales);

    return 0;
}

