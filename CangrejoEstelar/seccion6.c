#include "constantes.h"
#include "estructuras_datos_extras.h"
#include "veinteveinte.h"

#include <assert.h>
#include <ctype.h>

// SECCION DE TESTEO

// Tamaño máximo de un buffer para leer la línea
// Según el formato de entrada, tenemos que lo mínimo que necesitamos procesar (obviando los
// comentarios)son dos vértices y un lado, con lo que MAX_BUFFER debe cumplir >= 5. Ponemos
// 8 para poder ver si leemos aunque sea "p edge ", junto a '\0' que es agregado automáticamente
// por fgets, y nos indique que estamos cargando datos.

#define MAX_BUFFER 8

// Máximo número de dígitos para u32, si eso no ocurre, resulta que el vértice no es válido
#define MAX_DIGITOS 11

// Construccion/Destruccion
// Lee los datos desde el standard input en el formato indicado en el pdf y carga los
// datos en la estructura, incluyendo algun orden, y con esto se tiene un grafo. Luego
// se corre Greedy sobre el grafo, para colorearlo y devuelve el grafo coloreado.
// En caso de que haya un error (cualquiera) devuelve NULL.
// Según el formato descripto lo que me interesa es:
// Las líneas pueden tener un cantidad arbitraria de caracteres.
// Ignorar los comentarios: "c ..."
// Encontrar una línea que, filtrada por la condición de los espacios, sea equivalente a: "p edge n m"
// donde n indica el número de vértices y m el número de lados, exactos.
// Siguen m líneas de la forma, filtrada por la condición de los espacios: "e v w"
// donde e indica que tenemos el lado vw y se garantiza que tal lado aparece una sola vez.
// Siempre m > 0 y (por lo tanto) n >= 2.

Grafo ConstruccionDelGrafo() {
    Grafo grafo = NULL;

    char buffer[MAX_BUFFER];

    // "Booleano" que indica si estamos pasando por un comentario
    char es_comentario = 0;
    // "Booleano" que indica si estamos cargando datos
    char es_dato = 0;
    // "Booleano" que indica que estamos cargando los numeros de vertices y lados
    char datos_limites = 0;
    
    // "Booleano" que indica que estamos cargando una arista
    char datos_lado = 0;

    // "Booleano" para indicar que alocamos un grafo
    char grafo_inicializado = 0;

    // Esto es para contar los espacios cuando leemos para buscar el número de vertices y lados
    // Después se usa para tomar los lados
    // siempre cuenta si hay espacios entre dos numeros y si no se debe dar un error
    u32 espacios_limites = 0;

    // Damos 12 lugares, 11 para los dígitos y uno para el caracter '\0' 
    // String del primer número, para la conversión
    u32 digitos_primer_numero = 0;
    char string_primer_numero[12];
    // String del segundo número, para la conversión
    u32 digitos_segundo_numero = 0;
    char string_segundo_numero[12];

    // n_vertices = n y m_lados = m para "p edge <n> <m>"
    u32 n_vertices = 0;
    u32 m_lados = 0;

    // Nombre de vértice más chico que aparece
    u32 minimo_vertice = MAX_U32;
    // Nombre de vértice más grande que aparece
    u32 maximo_vertice = 0;

    // v = <v> y w = <w> para "e <v> <w>" 
    u32 v = 0;
    u32 w = 0;

    Nodo nodo_v;
    Nodo nodo_w;

    u32 hash_v = 0;
    u32 hash_w = 0;

    // Número en el orden natural, a medida que aparecen los vértices
    u32 orden = 0;

    // Indices para explorar los caracteres que consigue fgets
    u32 indice_buffer = 0;

    u32 vertices_leidos = 0;
    u32 lados_leidos    = 0;

    // Copia temporal de los vértices del grafo, esto nos va a servir para
    // asentar los vértices sin perder espacio y en el orden natural
    Lista* vertice_temporal = NULL;
    Nodo* orden_temporal = NULL;

    u32 factor_densidad = 1;

    while (fgets(buffer, MAX_BUFFER, stdin)) {
        // fgets no da NULL

        // Veo si hay comentarios
        if (!es_dato && !es_comentario) {
            if (buffer[0] == 'c') {
                es_comentario = 1;
            } else if (buffer[0] == 'p' && buffer[1] == ' ' &&
                        buffer[2] == 'e' && buffer[3] == 'd' &&
                        buffer[4] == 'g' && buffer[5] == 'e' && buffer[6] == ' ') {
                es_dato = 1;
                datos_limites = 1;
            } else {
                // En este caso tenemos un error, no hay ni indicación de comentario ni 
                // de que cargamos datos
                return NULL;
            }
        }
        if (!es_dato && es_comentario) {
            indice_buffer = 0;
            while (indice_buffer < MAX_BUFFER) {
                if (buffer[indice_buffer++] == '\n') {
                    // Deja de ser un comentario
                    es_comentario = 0;
                    // El siguiente, si indice_buffer < MAX_BUFFER, va a ser '\0'
                    break;
                }
            }
        }

        // Tenemos datos, en principio
        if (es_dato) {
            if (datos_limites) {
                // Obtenemos el número de vértices y de lados
                indice_buffer = 0;
                while (indice_buffer < MAX_BUFFER) {
                    if (buffer[indice_buffer] == '\n' && 
                        digitos_primer_numero && digitos_segundo_numero &&
                        espacios_limites <= 3) {
                        // Si llegamos a un salto de línea, además de que hay como mucho 3 espacios
                        // y tenemos ya un número no nulo de vértices y no nulo de aristas, entonces
                        // ya terminamos y debemos pasar a procesar las aristas.
                        string_primer_numero[digitos_primer_numero] = '\0';
                        string_segundo_numero[digitos_segundo_numero] = '\0';
                        
                        n_vertices = (u32) strtol(string_primer_numero, NULL, 10);
                        m_lados    = (u32) strtol(string_segundo_numero, NULL, 10);
                        
                        datos_limites = 0;
                        // El siguiente, si indice_buffer < MAX_BUFFER, va a ser '\0'
                        break;
                    } else if (isdigit(buffer[indice_buffer])) {
                        // Tenemos un dígito
                        if (!digitos_segundo_numero && digitos_primer_numero < MAX_DIGITOS && espacios_limites == 0) {
                            string_primer_numero[digitos_primer_numero++] = buffer[indice_buffer];
                        } else if (digitos_segundo_numero < MAX_DIGITOS && espacios_limites == 1) {
                            string_segundo_numero[digitos_segundo_numero++] = buffer[indice_buffer];
                        } else {
                            // Errores de varios tipos:
                            // Si no se cumple la primera condición, llegamos a la cantidad máxima
                            // de dígitos.
                            // Si no se cumple la segunda condición, estamos leyendo los dígitos del
                            // segundo número pero, o ocupamos todos los dígitos posibles o hay más espacios
                            // de los que debería.
                            
                            return NULL;
                        }
                    } else if (digitos_primer_numero && !espacios_limites && isspace(buffer[indice_buffer])) {
                        // Tenemos espacio en blanco, después de obtener la cantidad de vértices del grafo
                        espacios_limites++;
                    } else if (digitos_segundo_numero && espacios_limites == 1 && isspace(buffer[indice_buffer])) {
                        // Tenemos espacio en blanco extras después del primer espacio y la cantidad de vértices
                        espacios_limites++;
                    } else if (digitos_segundo_numero && espacios_limites == 2 && isspace(buffer[indice_buffer])) {
                        // Con esto si hay un espacio en blanco después del número de lados, lo contamos
                        espacios_limites++;
                    } else if (digitos_segundo_numero && espacios_limites == 3 && !isspace(buffer[indice_buffer])) {
                        // Error cuando se procesaban los dígitos en general, como caracteres extras
                        // despues del posible espacio extra.
                        
                        return NULL;
                    }
                    indice_buffer++;
                }
            } else {
                if (!grafo_inicializado) {

                    grafo = (Grafo) calloc(1, sizeof(GrafoSt));
                    if (grafo == NULL) return NULL;

                    grafo_inicializado = 1;
                    
                    // Inicialización de la estructura del grafo
                    grafo->numero_vertices = n_vertices;
                    grafo->numero_lados    = m_lados;
                    grafo->maximo_grado    = 0;
                    
                    // Para crear las listas de adyacencia temporales
                    factor_densidad = NuevoFactorDensidad(grafo);
                    
                    // Tomo memoria para el array de vértices del grafo
                    grafo->vertice = (Vertice*) calloc(n_vertices, sizeof(Vertice));
                    if (grafo->vertice == NULL) {
                        free(grafo);
                        return NULL;
                    }
                    // Tomo memoria para el array de orden interno
                    grafo->orden   = (Vertice*) calloc(n_vertices, sizeof(Vertice));
                    if (grafo->orden == NULL) {
                        free(grafo->vertice);
                        free(grafo);
                        return NULL;
                    }
                    // Tomo memoria para la lista de vértices temporal
                    // Doy una lista de vecinos de tamaño 2*n para que no haya tantas colisiones
                    vertice_temporal = (Lista*) calloc(2 * n_vertices, sizeof(Lista));
                    if (vertice_temporal == NULL) {
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        return NULL;
                    }
                    // Tomo memoria par un orden temporal
                    // Me permite guardar las referencias a los datos dados en vértice temporal
                    // para evitar hacer dos loops sobre vertice_temporal
                    orden_temporal = (Nodo*) calloc(n_vertices, sizeof(Nodo));
                    if (orden_temporal == NULL) {
                        free(vertice_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        return NULL;
                    }
                }
                // Obtenemos las aristas
                indice_buffer = 0;
                while (indice_buffer < MAX_BUFFER && lados_leidos <= m_lados) {
                    if (!datos_lado) {
                        if (buffer[0] == 'e' && buffer[1] == ' ') {
                            // Comenzamos a procesar una arista

                            // Reiniciamos los arrays que deben contener dígitos
                            for (u32 i = 0; i <= digitos_primer_numero; i++) {
                                string_primer_numero[i] = '\0';
                            }
                            for (u32 i = 0; i <= digitos_segundo_numero; i++) {
                                string_segundo_numero[i] = '\0';
                            }
                            digitos_primer_numero  = 0;
                            digitos_segundo_numero = 0;

                            // Reiniciamos el conteo de espacios
                            espacios_limites = 0;

                            datos_lado = 1;
                        } else {
                            // Error: deberíamos estar cargando un lado
                            
                            return NULL;
                        }
                    } else {
                        // Seguimos procesando la línea, consideramos que ocurre si tenemos EOF y termina de forma abrupta
                        if ((buffer[indice_buffer] == '\n' || feof(stdin)) && 
                            digitos_primer_numero && digitos_segundo_numero &&
                            espacios_limites <= 3) {
                            // Hay como mucho 3 espacios y tenemos ya dos números que tienen sentido 

                            string_primer_numero[digitos_primer_numero]   = '\0';
                            string_segundo_numero[digitos_segundo_numero] = '\0';
                            
                            v = (u32) strtol(string_primer_numero, NULL, 10);
                            w = (u32) strtol(string_segundo_numero, NULL, 10);

                            if (v < minimo_vertice) minimo_vertice = v;
                            if (w < minimo_vertice) minimo_vertice = w;

                            if (v > maximo_vertice) maximo_vertice = v;
                            if (w > maximo_vertice) maximo_vertice = w;

                            hash_v = Hash(v, 2*n_vertices);
                            hash_w = Hash(w, 2*n_vertices);

                            if (vertice_temporal[hash_v] == NULL) vertice_temporal[hash_v] = NuevaLista();
                            nodo_v = BuscarEnLista(vertice_temporal[hash_v], v);

                            if (vertice_temporal[hash_w] == NULL) vertice_temporal[hash_w] = NuevaLista();
                            nodo_w = BuscarEnLista(vertice_temporal[hash_w], w);

                            if (nodo_v == NULL) {
                                orden_temporal[orden] = InsertarLista(vertice_temporal[hash_v], v, 0, orden, factor_densidad, orden, NULL);
                                nodo_v = orden_temporal[orden];
                                if (nodo_v != NULL) {
                                    vertices_leidos++;
                                } else {
                                    for (u32 k = 0; k < 2*n_vertices; k++) {
                                        DestruirLista(vertice_temporal[k]);
                                    }
                                    free(vertice_temporal);
                                    free(orden_temporal);
                                    free(grafo->vertice);
                                    free(grafo->orden);
                                    free(grafo);
                                    grafo = NULL;
                                    return NULL;
                                }
                                orden++;
                            }
                            
                            if (nodo_w == NULL) {
                                orden_temporal[orden] = InsertarLista(vertice_temporal[hash_w], w, 0, orden, factor_densidad, orden, NULL);
                                nodo_w = orden_temporal[orden];
                                if (nodo_w != NULL) {
                                    vertices_leidos++;
                                } else {
                                    for (u32 k = 0; k < 2*n_vertices; k++) {
                                        DestruirLista(vertice_temporal[k]);
                                    }
                                    free(vertice_temporal);
                                    free(orden_temporal);
                                    free(grafo->vertice);
                                    free(grafo->orden);
                                    free(grafo);
                                    grafo = NULL;
                                    return NULL;
                                }
                                orden++;
                            }

                            // Insertar en las listas de adyacencias
                            if (InsertarNodo(nodo_v, nodo_w) && InsertarNodo(nodo_w, nodo_v)) {
                                if (nodo_v->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_v->grado;
                                if (nodo_w->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_w->grado;
                                lados_leidos++;
                            } else {
                                for (u32 k = 0; k < 2*n_vertices; k++) {
                                    DestruirLista(vertice_temporal[k]);
                                }
                                free(vertice_temporal);
                                free(orden_temporal);
                                free(grafo->vertice);
                                free(grafo->orden);
                                free(grafo);
                                grafo = NULL;
                                return NULL;
                            }
                            datos_lado = 0;
                            // El siguiente, si indice_buffer < MAX_BUFFER, va a ser '\0'
                            break;
                        } else if (isdigit(buffer[indice_buffer])) {
                            if (!digitos_segundo_numero && digitos_primer_numero < MAX_DIGITOS && espacios_limites == 0) {
                                string_primer_numero[digitos_primer_numero++] = buffer[indice_buffer];
                            } else if (digitos_segundo_numero < MAX_DIGITOS && espacios_limites == 1) {
                                string_segundo_numero[digitos_segundo_numero++] = buffer[indice_buffer];
                            } else {
                                // Errores de varios tipos:
                                // Si no se cumple la primera condición, llegamos a la cantidad máxima
                                // de dígitos.
                                // Si no se cumple la segunda condición, estamos leyendo los dígitos del
                                // segundo número pero, o ocupamos todos los dígitos posibles o hay más espacios
                                // de los que debería.
                                
                                return NULL;
                            }
                        } else if (digitos_primer_numero && !espacios_limites && isspace(buffer[indice_buffer])) {
                            // Tenemos espacio en blanco, después de obtener la cantidad de vértices del grafo
                            espacios_limites++;
                        } else if (digitos_segundo_numero && espacios_limites == 1 && isspace(buffer[indice_buffer])) {
                            // Tenemos espacio en blanco extras después del primer espacio y la cantidad de vértices
                            espacios_limites++;
                        } else if (digitos_segundo_numero && espacios_limites == 2 && isspace(buffer[indice_buffer])) {
                            // Con esto si hay un espacio en blanco después del número de lados, lo contamos
                            espacios_limites++;
                        } else if (digitos_segundo_numero && espacios_limites == 3 && !isspace(buffer[indice_buffer])) {
                            // Error cuando se procesaban los dígitos en general, como caracteres extras
                            // despues del posible espacio extra.

                            return NULL;
                        }
                    }
                    indice_buffer++;
                }
                // Ya tengo lo que me hace falta, ignoro las siguientes líneas (si las hay)
                if (lados_leidos == m_lados) break;
            }
        }
    }
    if (feof(stdin) && lados_leidos != m_lados) {
        // Se indicó EOF pero todavía falta al menos un lado

        // Caso 0: No hemos comenzado a leer la línea, ie llegamos a EOF con menos de 8 caracteres
        // Caso 1: No hemos leído dígitos del primer número, pero si hemos comenzado a leer la línea
        //         Este caso no es posible, porque el buffer siempre toma de a 8 caracteres
        // Caso 2: Hemos leído parcialmente los dígitos del primer número
        // Caso 3: Leímos todos los dígitos del primer número pero ninguno del segundo
        // Caso 4: Hemos leído los dígitos del primer número y parcialmente los del segundo
        // Caso 5: Leímos ambos números y no queda nada, excepto espacios en blanco
        

        if (!datos_lado && buffer[0] == 'e' && buffer[1] == ' ') {
            // Caso 0: No hemos comenzado a leer la línea, ie llegamos a EOF con menos de 8 caracteres
            // Reiniciamos los arrays que deben contener dígitos
            for (u32 i = 0; i <= digitos_primer_numero; i++) {
                string_primer_numero[i] = '\0';
            }
            for (u32 i = 0; i <= digitos_segundo_numero; i++) {
                string_segundo_numero[i] = '\0';
            }
            digitos_primer_numero  = 0;
            digitos_segundo_numero = 0;

            // Reiniciamos el conteo de espacios
            espacios_limites = 0;

            indice_buffer = 2;
            while (indice_buffer < MAX_BUFFER) {
                if (digitos_primer_numero && digitos_segundo_numero &&
                    espacios_limites <= 3) {
                    // Hay como mucho 3 espacios y tenemos ya dos números que tienen sentido 

                    string_primer_numero[digitos_primer_numero]   = '\0';
                    string_segundo_numero[digitos_segundo_numero] = '\0';
                        
                    v = (u32) strtol(string_primer_numero, NULL, 10);
                    w = (u32) strtol(string_segundo_numero, NULL, 10);
                    
                    if (v < minimo_vertice) minimo_vertice = v;
                    if (w < minimo_vertice) minimo_vertice = w;

                    if (v > maximo_vertice) maximo_vertice = v;
                    if (w > maximo_vertice) maximo_vertice = w;

                    hash_v = Hash(v, 2*n_vertices);
                    hash_w = Hash(w, 2*n_vertices);
                    
                    if (vertice_temporal[hash_v] == NULL) vertice_temporal[hash_v] = NuevaLista();
                    nodo_v = BuscarEnLista(vertice_temporal[hash_v], v);

                    if (vertice_temporal[hash_w] == NULL) vertice_temporal[hash_w] = NuevaLista();
                    nodo_w = BuscarEnLista(vertice_temporal[hash_w], w);

                    if (nodo_v == NULL) {
                        orden_temporal[orden] = InsertarLista(vertice_temporal[hash_v], v, 0, orden, factor_densidad, orden, NULL);
                        nodo_v = orden_temporal[orden];
                        if (nodo_v != NULL) {
                            vertices_leidos++;
                        } else {
                            for (u32 k = 0; k < 2*n_vertices; k++) {
                                DestruirLista(vertice_temporal[k]);
                            }
                            free(vertice_temporal);
                            free(orden_temporal);
                            free(grafo->vertice);
                            free(grafo->orden);
                            free(grafo);
                            grafo = NULL;
                            return NULL;
                        }
                        orden++;
                    }
                    
                    if (nodo_w == NULL) {
                        orden_temporal[orden] = InsertarLista(vertice_temporal[hash_w], w, 0, orden, factor_densidad, orden, NULL);
                        nodo_w = orden_temporal[orden];
                        if (nodo_w != NULL) {
                            vertices_leidos++;
                        } else {
                            for (u32 k = 0; k < 2*n_vertices; k++) {
                                DestruirLista(vertice_temporal[k]);
                            }
                            free(vertice_temporal);
                            free(orden_temporal);
                            free(grafo->vertice);
                            free(grafo->orden);
                            free(grafo);
                            grafo = NULL;
                            return NULL;
                        }
                        orden++;
                    }
                    // Insertar en las listas de adyacencias
                    if (InsertarNodo(nodo_v, nodo_w) && InsertarNodo(nodo_w, nodo_v)) {
                        if (nodo_v->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_v->grado;
                        if (nodo_w->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_w->grado;
                        lados_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    // El siguiente, si indice_buffer < MAX_BUFFER, va a ser '\0'
                    break;
                } else if (isdigit(buffer[indice_buffer])) {
                    if (!digitos_segundo_numero && digitos_primer_numero < MAX_DIGITOS && espacios_limites == 0) {
                        string_primer_numero[digitos_primer_numero++] = buffer[indice_buffer];
                    } else if (digitos_segundo_numero < MAX_DIGITOS && espacios_limites == 1) {
                        string_segundo_numero[digitos_segundo_numero++] = buffer[indice_buffer];
                    } else {
                        // Errores de varios tipos:
                        // Si no se cumple la primera condición, llegamos a la cantidad máxima
                        // de dígitos.
                        // Si no se cumple la segunda condición, estamos leyendo los dígitos del
                        // segundo número pero, o ocupamos todos los dígitos posibles o hay más espacios
                        // de los que debería.
                        
                        return NULL;
                    }
                } else if (digitos_primer_numero && !espacios_limites && isspace(buffer[indice_buffer])) {
                    // Tenemos espacio en blanco, después de obtener la cantidad de vértices del grafo
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 1 && isspace(buffer[indice_buffer])) {
                    // Tenemos espacio en blanco extras después del primer espacio y la cantidad de vértices
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 2 && isspace(buffer[indice_buffer])) {
                    // Con esto si hay un espacio en blanco después del número de lados, lo contamos
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 3 && !isspace(buffer[indice_buffer])) {
                    // Error cuando se procesaban los dígitos en general, como caracteres extras
                    // despues del posible espacio extra.

                    return NULL;
                }
                indice_buffer++;
            }
        } else if (datos_lado) {
            // Caso 2: Hemos leído parcialmente los dígitos del primer número
            // Caso 3: Leímos todos los dígitos del primer número pero ninguno del segundo
            // Caso 4: Hemos leído los dígitos del primer número y parcialmente los del segundo
            indice_buffer = 0;
            while (indice_buffer < MAX_BUFFER) {
                if (digitos_primer_numero && digitos_segundo_numero &&
                    espacios_limites <= 3) {
                    // Hay como mucho 3 espacios y tenemos ya dos números que tienen sentido 

                    string_primer_numero[digitos_primer_numero]   = '\0';
                    string_segundo_numero[digitos_segundo_numero] = '\0';
                        
                    v = (u32) strtol(string_primer_numero, NULL, 10);
                    w = (u32) strtol(string_segundo_numero, NULL, 10);

                    if (v < minimo_vertice) minimo_vertice = v;
                    if (w < minimo_vertice) minimo_vertice = w;
                    
                    if (v > maximo_vertice) maximo_vertice = v;
                    if (w > maximo_vertice) maximo_vertice = w;
                    
                    hash_v = Hash(v, 2*n_vertices);
                    hash_w = Hash(w, 2*n_vertices);
                    
                    if (vertice_temporal[hash_v] == NULL) vertice_temporal[hash_v] = NuevaLista();
                    nodo_v = BuscarEnLista(vertice_temporal[hash_v], v);

                    if (vertice_temporal[hash_w] == NULL) vertice_temporal[hash_w] = NuevaLista();
                    nodo_w = BuscarEnLista(vertice_temporal[hash_w], w);

                    if (nodo_v == NULL) {
                        orden_temporal[orden] = InsertarLista(vertice_temporal[hash_v], v, 0, orden, factor_densidad, orden, NULL);
                        nodo_v = orden_temporal[orden];
                        if (nodo_v != NULL) {
                            vertices_leidos++;
                        } else {
                            for (u32 k = 0; k < 2*n_vertices; k++) {
                                DestruirLista(vertice_temporal[k]);
                            }
                            free(vertice_temporal);
                            free(orden_temporal);
                            free(grafo->vertice);
                            free(grafo->orden);
                            free(grafo);
                            grafo = NULL;
                            return NULL;
                        }
                        orden++;
                    }
                            
                    if (nodo_w == NULL) {
                        orden_temporal[orden] = InsertarLista(vertice_temporal[hash_w], w, 0, orden, factor_densidad, orden, NULL);
                        nodo_w = orden_temporal[orden];
                        if (nodo_w != NULL) {
                            vertices_leidos++;
                        } else {
                            for (u32 k = 0; k < 2*n_vertices; k++) {
                                DestruirLista(vertice_temporal[k]);
                            }
                            free(vertice_temporal);
                            free(orden_temporal);
                            free(grafo->vertice);
                            free(grafo->orden);
                            free(grafo);
                            grafo = NULL;
                            return NULL;
                        }
                        orden++;
                    }

                    // Insertar en las listas de adyacencias
                    if (InsertarNodo(nodo_v, nodo_w) && InsertarNodo(nodo_w, nodo_v)) {
                        if (nodo_v->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_v->grado;
                        if (nodo_w->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_w->grado;

                        lados_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    // El siguiente, si indice_buffer < MAX_BUFFER, va a ser '\0'
                    break;
                } else if (isdigit(buffer[indice_buffer])) {
                    if (!digitos_segundo_numero && digitos_primer_numero < MAX_DIGITOS && espacios_limites == 0) {
                        string_primer_numero[digitos_primer_numero++] = buffer[indice_buffer];
                    } else if (digitos_segundo_numero < MAX_DIGITOS && espacios_limites == 1) {
                        string_segundo_numero[digitos_segundo_numero++] = buffer[indice_buffer];
                    } else {
                        // Errores de varios tipos:
                        // Si no se cumple la primera condición, llegamos a la cantidad máxima
                        // de dígitos.
                        // Si no se cumple la segunda condición, estamos leyendo los dígitos del
                        // segundo número pero, o ocupamos todos los dígitos posibles o hay más espacios
                        // de los que debería.
                        return NULL;
                    }
                } else if (digitos_primer_numero && !espacios_limites && isspace(buffer[indice_buffer])) {
                    // Tenemos espacio en blanco, después de obtener la cantidad de vértices del grafo
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 1 && isspace(buffer[indice_buffer])) {
                    // Tenemos espacio en blanco extras después del primer espacio y la cantidad de vértices
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 2 && isspace(buffer[indice_buffer])) {
                    // Con esto si hay un espacio en blanco después del número de lados, lo contamos
                    espacios_limites++;
                } else if (digitos_segundo_numero && espacios_limites == 3 && !isspace(buffer[indice_buffer])) {
                    // Error cuando se procesaban los dígitos en general, como caracteres extras
                    // despues del posible espacio extra.
                    return NULL;
                }
                indice_buffer++;
            }
        } else if (datos_lado && digitos_primer_numero && digitos_segundo_numero && espacios_limites <= 3) {
            // Caso 5: Leímos ambos números y no queda nada, excepto espacios en blanco
            indice_buffer = 0;
            espacios_limites = 0;
            while (indice_buffer < MAX_BUFFER && buffer[indice_buffer] != '\0') {
                if (isspace(buffer[indice_buffer])) espacios_limites++;
                indice_buffer++;
            }
            if (((indice_buffer < MAX_BUFFER && buffer[indice_buffer] == '\0') ||
                 (indice_buffer == MAX_BUFFER)) &&
                  indice_buffer - 1 == espacios_limites) {
                // Hay como mucho 3 espacios y tenemos ya dos números que tienen sentido 
                    
                string_primer_numero[digitos_primer_numero]   = '\0';
                string_segundo_numero[digitos_segundo_numero] = '\0';
                    
                v = (u32) strtol(string_primer_numero, NULL, 10);
                w = (u32) strtol(string_segundo_numero, NULL, 10);

                if (v < minimo_vertice) minimo_vertice = v;
                if (w < minimo_vertice) minimo_vertice = w;

                if (v > maximo_vertice) maximo_vertice = v;
                if (w > maximo_vertice) maximo_vertice = w;

                hash_v = Hash(v, 2*n_vertices);
                hash_w = Hash(w, 2*n_vertices);
                
                if (vertice_temporal[hash_v] == NULL) vertice_temporal[hash_v] = NuevaLista();
                nodo_v = BuscarEnLista(vertice_temporal[hash_v], v);

                if (vertice_temporal[hash_w] == NULL) vertice_temporal[hash_w] = NuevaLista();
                nodo_w = BuscarEnLista(vertice_temporal[hash_w], w);

                if (nodo_v == NULL) {
                    orden_temporal[orden] = InsertarLista(vertice_temporal[hash_v], v, 0, orden, factor_densidad, orden, NULL);
                    nodo_v = orden_temporal[orden];
                    if (nodo_v != NULL) {
                        vertices_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    orden++;
                }
                            
                if (nodo_w == NULL) {
                    orden_temporal[orden] = InsertarLista(vertice_temporal[hash_w], w, 0, orden, factor_densidad, orden, NULL);
                    nodo_w = orden_temporal[orden];
                    if (nodo_w != NULL) {
                        vertices_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    orden++;
                }

                // Insertar en las listas de adyacencias
                if (InsertarNodo(nodo_v, nodo_w) && InsertarNodo(nodo_w, nodo_v)) {
                    if (nodo_v->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_v->grado;
                    if (nodo_w->grado > grafo->maximo_grado) grafo->maximo_grado = nodo_w->grado;

                    lados_leidos++;
                } else {
                    for (u32 k = 0; k < 2*n_vertices; k++) {
                        DestruirLista(vertice_temporal[k]);
                    }
                    free(vertice_temporal);
                    free(orden_temporal);
                    free(grafo->vertice);
                    free(grafo->orden);
                    free(grafo);
                    grafo = NULL;
                    return NULL;
                }
            } else {
                // Error cuando se procesaban los dígitos en general, como caracteres extras
                // despues del posible espacio extra.
                return NULL;
            }
        }
    }

    // Ya se deberían haber leído todas la líneas
    if (lados_leidos != m_lados && grafo_inicializado) {
        // No coinciden los lados que se han leído con los del grafo a devolver
        for (u32 k = 0; k < 2*n_vertices; k++) {
            DestruirLista(vertice_temporal[k]);
        }
        free(vertice_temporal);
        free(orden_temporal);
        free(grafo->vertice);
        free(grafo->orden);
        free(grafo);
        grafo = NULL;
        return NULL;
    }
    
    // Hay menos vértices leídos, en este caso, optamos por dar nombres arbitrarios a los que faltan
    // OBSERVAR: Esto no está del todo bien, estamos suponiendo demasiado y me faltaría chequear condiciones
    //           por los posibles huecos
    if (vertices_leidos < n_vertices) {
        u32 color_aislados = orden;
        u32 i, hash_i;
        Nodo nodo_i;
        // Los vértices que nos dieron no alcanzan
        // Analizamos los casos posibles para dar el orden y completar el grafo
        if (maximo_vertice < MAX_U32 && (maximo_vertice + 1) < MAX_U32) {
            i = maximo_vertice + 1;
            while (i < MAX_U32 && vertices_leidos < n_vertices) {
                hash_i = Hash(i, 2*n_vertices);
                if (vertice_temporal[hash_i] == NULL) vertice_temporal[hash_i] = NuevaLista();
                // Vemos que estén en vertices y si no le asignamos un nodo (lo registramos).
                nodo_i = BuscarEnLista(vertice_temporal[hash_i], i);
                
                // Se supone que esto nos lo deberíamos pasar, pero por las dudas
                if (nodo_i == NULL) {
                    // Damos su orden, en el orden natural
                    // Damos un color correspondiente al orden natural al nuevo vértice
                    orden_temporal[orden] = InsertarLista(vertice_temporal[hash_i], i, 0, color_aislados, factor_densidad, orden, NULL);
                    nodo_i = orden_temporal[orden];
                    if (nodo_i != NULL) {
                        vertices_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    orden++;
                    i++;
                } else {
                    // Dejamos a i como está y tratamos de alocar de nuevo
                    continue;
                }
            }
        }
        if (minimo_vertice > 0 && vertices_leidos < n_vertices) {
            i = 0;
            while (i < MAX_U32 && vertices_leidos < n_vertices) {
                hash_i = Hash(i, 2*n_vertices);
                if (vertice_temporal[hash_i] == NULL) vertice_temporal[hash_i] = NuevaLista();
                // Vemos que estén en vertices y si no le asignamos un nodo (lo registramos).
                nodo_i = BuscarEnLista(vertice_temporal[hash_i], i);
                
                // Se supone que esto nos lo deberíamos pasar, pero por las dudas
                if (nodo_i == NULL) {
                    // Damos su orden, en el orden natural
                    // Damos un color correspondiente al orden natural al nuevo vértice
                    orden_temporal[orden] = InsertarLista(vertice_temporal[hash_i], i, 0, color_aislados, factor_densidad, orden, NULL);
                    nodo_i = orden_temporal[orden];
                    if (nodo_i != NULL) {
                        vertices_leidos++;
                    } else {
                        for (u32 k = 0; k < 2*n_vertices; k++) {
                            DestruirLista(vertice_temporal[k]);
                        }
                        free(vertice_temporal);
                        free(orden_temporal);
                        free(grafo->vertice);
                        free(grafo->orden);
                        free(grafo);
                        grafo = NULL;
                        return NULL;
                    }
                    orden++;
                    i++;
                } else {
                    // Dejamos a i como está y tratamos de alocar de nuevo
                    continue;
                }
            }
        }
        if (vertices_leidos < n_vertices) {
            // Por  alguna razón no tenemos los vértices suficientes, obviamente esto es anómalo
            // y devolvemos el error.
            for (u32 k = 0; k < 2*n_vertices; k++) {
                DestruirLista(vertice_temporal[k]);
            }
            free(vertice_temporal);
            free(orden_temporal);
            free(grafo->vertice);
            free(grafo->orden);
            free(grafo);
            grafo = NULL;
            return NULL;
        }
    }

    // Algún error ocurrió mientras leía stdin
    if (ferror(stdin) && grafo_inicializado) {
        for (u32 k = 0; k < 2*n_vertices; k++) {
            DestruirLista(vertice_temporal[k]);
        }
        free(vertice_temporal);
        free(orden_temporal);
        free(grafo->vertice);
        free(grafo->orden);
        free(grafo);
        grafo = NULL;
        return NULL;
    }
    
    // Transferir los datos de los nodos a los vertices del grafo
    u32 vertices_copiados = 0;
    Vertice vertice_de_copia;
    for (u32 i = 0; i < n_vertices; i++) {
        if (orden_temporal[i] != NULL) {
            vertice_de_copia = (Vertice) calloc(1, sizeof(VerticeSt));
            if (vertice_de_copia == NULL) {
                for (u32 k = 0; k < 2*n_vertices; k++) {
                    DestruirLista(vertice_temporal[k]);
                    if (k < vertices_copiados) {
                        free(grafo->orden[k]->vecinos);
                        free(grafo->orden[k]);
                    }
                }
                free(vertice_temporal);
                free(orden_temporal);
                free(grafo->vertice);
                free(grafo->orden);
                free(grafo);
                grafo = NULL;
                return NULL;
            }
            // Uso orden_temporal para copiar los vertices
            vertice_de_copia->clave = orden_temporal[i]->clave;
            vertice_de_copia->color = orden_temporal[i]->color;
            vertice_de_copia->grado = orden_temporal[i]->grado;
            vertice_de_copia->orden = orden_temporal[i]->orden;
            vertice_de_copia->vecinos = NuevaListaVecinos(vertice_de_copia->grado);
            if (vertice_de_copia->vecinos == NULL) {
                for (u32 k = 0; k < 2*n_vertices; k++) {
                    DestruirLista(vertice_temporal[k]);
                    if (k < vertices_copiados) {
                        free(grafo->orden[k]->vecinos);
                        free(grafo->orden[k]);
                    }
                }
                free(vertice_temporal);
                free(orden_temporal);
                free(grafo->vertice);
                free(grafo->orden);
                free(grafo);
                grafo = NULL;
                return NULL;
            }
            // Nos faltan los vecinos
            // Lo paso a un lugar en la tabla de vertices
            grafo->vertice[i] = vertice_de_copia;
            // Lo paso a su lugar en la tabla de orden interno, el orden de entrada
            grafo->orden[vertice_de_copia->orden]   = vertice_de_copia;
            vertices_copiados++;
        } else {
            for (u32 k = 0; k < 2*n_vertices; k++) {
                DestruirLista(vertice_temporal[k]);
                if (k < vertices_copiados) {
                    free(grafo->orden[k]->vecinos);
                    free(grafo->orden[k]);
                }
            }
            free(vertice_temporal);
            free(orden_temporal);
            free(grafo->vertice);
            free(grafo->orden);
            free(grafo);
            grafo = NULL;
            return NULL;
        }
    }
    // Listo los vértices, pero ahora necesito llenar las listas de adyacencia
    u32 longitud_lista;
    for (u32 i = 0; i < n_vertices; i++) {
        longitud_lista = grafo->vertice[i]->grado;
        for (u32 j = 0; j < longitud_lista; j++) {
            // Uso el orden copiado en el vertice (el mismo i para facilidad, como hicimos 
            // arriba) para recuperar en orden_temporal el orden los vecinos de grafo->vertice[i]
            // A su vez uso el orden del vecino en el nodo temporal para recuperar
            // en la tabla de orden interno del grafo
            grafo->vertice[i]->vecinos[j] = grafo->orden[orden_temporal[i]->vecinos[j]->orden]; 
        }
    }

    // Ordenamos el arreglo de vertices del grafo en el orden natural
    // El orden es el mismo, ya que grafo->orden guarda punteros a los vertices
    // no las posiciones en grafo->vertices, por supuesto orden natural, no es el que
    // queda en el orden interno sino en la tabla de vértices
    qsort(grafo->vertice, n_vertices, sizeof(Vertice), ComparadorNombresVertices);
    
    // Recién ahora puedo destruir las estructuras auxiliares,
    // ya que se referencian unos a otros
    for (u32 k = 0; k < 2*n_vertices; k++) {
        DestruirLista(vertice_temporal[k]);
    }
    free(vertice_temporal);
    free(orden_temporal);

    // Ahora si, usar el orden natural
    for (u32 i = 0; i < n_vertices; i++) {
        // Indicar que orden[i] == vertice i-ésimo en el orden natural
        FijarOrden(i, grafo, i);
    }
    
    // Corremos Greedy, doy un coloreo acorde al trabajo
    Greedy(grafo);

    return grafo;
}

// Destruye G y libera la memoria alocada
void DestruccionDelGrafo(Grafo G) {
    if (G) {
        u32 numero_vertices = NumeroDeVertices(G);
        for (u32 i = 0; i < numero_vertices; i++) {
            free(G->vertice[i]->vecinos);
            free(G->vertice[i]);
        }
        free(G->vertice);
        free(G->orden);
        free(G);
        G = NULL;
    }
    return;
}

// Copiado
// Aloca la memoria suficiente para alojar una copia de G, hace la copia de G en esa memoria y
// devuelve un puntero a ella. Si ocurre algún error devuelve NULL.
Grafo CopiarGrafo(Grafo G) {
    Grafo grafo = (Grafo) calloc(1, sizeof(GrafoSt));
    if (grafo != NULL) {
        u32 numero_vertices = NumeroDeVertices(G);
        grafo->numero_vertices = numero_vertices;
        grafo->numero_lados    = NumeroDeLados(G);
        grafo->maximo_grado    = Delta(G);
        grafo->vertice = (Vertice*) calloc(numero_vertices, sizeof(Vertice));
        if (grafo->vertice == NULL) {
            free(grafo);
            return NULL;
        }
        grafo->orden   = (Vertice*) calloc(numero_vertices, sizeof(Vertice));
        if (grafo->orden == NULL) {
            free(grafo->vertice);
            free(grafo);
            return NULL;
        }
        // Para copiar los vértices
        u32 vertices_copiados = 0;
        Vertice vertice_de_copia;
        for (u32 i = 0; i < numero_vertices; i++) {
            vertice_de_copia = (Vertice) calloc(1, sizeof(VerticeSt));
            if (vertice_de_copia == NULL) {
                for (u32 k = 0; k < vertices_copiados; k++) {
                    free(grafo->orden[k]->vecinos);
                    free(grafo->orden[k]);
                }
                free(grafo->vertice);
                free(grafo->orden);
                free(grafo);
                return NULL;
            }
            // Uso la tabla de vertices de G para copiar en el nuevo grafo
            // los mismo datos en vertice
            vertice_de_copia->clave = G->vertice[i]->clave;
            vertice_de_copia->color = G->vertice[i]->color;
            vertice_de_copia->grado = G->vertice[i]->grado;
            vertice_de_copia->orden = G->vertice[i]->orden;
            vertice_de_copia->vecinos = NuevaListaVecinos(vertice_de_copia->grado);
            if (vertice_de_copia->vecinos == NULL) {
                for (u32 k = 0; k < vertices_copiados; k++) {
                    free(grafo->vertice[k]->vecinos);
                    free(grafo->vertice[k]);
                }
                free(grafo->vertice);
                free(grafo->orden);
                free(grafo);
                return NULL;
            }
            // Nos faltan los vecinos
            // Paso a su lugar en vertice a la copia
            grafo->vertice[i] = vertice_de_copia;
            // Paso a su lugar en orden a la copia
            grafo->orden[vertice_de_copia->orden] = vertice_de_copia;
            vertices_copiados++;
        }
        // Listo los vértices, pero ahora necesito llenar las listas de adyacencia
        // Para copiar los vecinos
        u32 longitud_lista;
        for (u32 i = 0; i < numero_vertices; i++) {
            longitud_lista = grafo->vertice[i]->grado;
            for (u32 j = 0; j < longitud_lista; j++) {
                // Copie a los lugares de los vértices desde la tabla de vértices de
                // G, entonces uso su tabla para hallar el orden en la tabla de orden
                // de la copia
                grafo->vertice[i]->vecinos[j] = grafo->orden[G->vertice[i]->vecinos[j]->orden]; 
            }
        }
    }
    return grafo;
}
