#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char id[10];          // ID único de la canción
    List *artists;    // Nombre del artista o artistas
    char album[100];      // Nombre del álbum
    char track_name[100]; // Nombre de la canción
    float tempo;          // Tempo de la canción
    char genre[50];       // Género de la canción
} cancion;

typedef struct {
    char id[10];          // ID único de la canción
    List *artists;    // Nombre del artista o artistas 
    char track_name[100];        // Lista de canciones del artista
} tempoCancion;

typedef struct {
    char *artista;   // nombre del artista
    List *songs;      // Lista de canciones del artista
} autor;

typedef struct {
    List *lento;          // Lista de canciones lentas
    List *medio;          // Lista de canciones de tempo medio
    List *rapido;          // Lista de canciones rápidas
} tempo;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("           MENÚ PRINCIPAL CANCIONES");
  puts("========================================");

  puts("1) Cargar Canciones");
  puts("2) Buscar por id");
  puts("3) Buscar por autor");
  puts("4) Buscar por tempo");
  puts("5) Salir");
}

int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}


int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}
/**
 * Carga canciones desde un archivo CSV
 */
void leer_canciones(HashMap *mapa_canciones, HashMap *mapa_artistas, tempo *lista_tempo) {
    limpiarPantalla();
    FILE *archivo = fopen("data/song_dataset_.csv", "r");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    leer_linea_csv(archivo, ','); // Saltar encabezado

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        if (!campos[0] || !campos[2] || !campos[4] || !campos[18] || !campos[20]) continue;

        cancion *nueva_cancion = malloc(sizeof(cancion));
        if (!nueva_cancion) continue;

        strcpy(nueva_cancion->id, campos[0]);
        strcpy(nueva_cancion->track_name, campos[4]);
        strcpy(nueva_cancion->album, campos[3]);
        nueva_cancion->tempo = atof(campos[18]);
        strcpy(nueva_cancion->genre, campos[20]);

        // Dividir artistas (copiando string temporal)
        char *artistas_raw = strdup(campos[2]);
        nueva_cancion->artists = split_string(artistas_raw, ";");
        free(artistas_raw);
        if (!nueva_cancion->artists) {
            free(nueva_cancion);
            continue;
        }

        insertMap(mapa_canciones, strdup(nueva_cancion->id), nueva_cancion);

        for (char *artista = list_first(nueva_cancion->artists); artista != NULL; artista = list_next(nueva_cancion->artists)) {
            Pair *par = searchMap(mapa_artistas, artista);
            if (par) {
                autor *artista_existente = par->value;
                list_pushBack(artista_existente->songs, nueva_cancion);
            } else {
                autor *nuevo_autor = malloc(sizeof(autor));
                if (!nuevo_autor) continue;

                nuevo_autor->artista = strdup(artista);
                nuevo_autor->songs = list_create();
                if (!nuevo_autor->artista || !nuevo_autor->songs) {
                    free(nuevo_autor->artista);
                    free(nuevo_autor);
                    continue;
                }
                list_pushBack(nuevo_autor->songs, nueva_cancion);
                insertMap(mapa_artistas, nuevo_autor->artista, nuevo_autor);
            }
        }

        // Clasificación por tempo
        if (nueva_cancion->tempo < 80)
            list_pushBack(lista_tempo->lento, nueva_cancion);
        else if (nueva_cancion->tempo <= 120)
            list_pushBack(lista_tempo->medio, nueva_cancion);
        else
            list_pushBack(lista_tempo->rapido, nueva_cancion);
    }

    fclose(archivo);
    puts("Canciones cargadas correctamente.");
}


void buscarPorId(HashMap *mapa_canciones) {
    limpiarPantalla();
    char id[10];
    printf("Ingrese el ID de la canción: ");
    scanf("%s", id);

    Pair *resultado = searchMap(mapa_canciones, id);
    if (resultado != NULL) {
        cancion *cancion_encontrada = (cancion *)resultado->value;
        printf("Cancion encontrada:\n");
        printf("ID: %s\n", cancion_encontrada->id);
        printf("Título canción: %s\n", cancion_encontrada->track_name);
        printf("Artistas:\n");
        for (char *artista = list_first(cancion_encontrada->artists); artista != NULL; 
             artista = list_next(cancion_encontrada->artists)) {
            printf("  %s\n", artista);
        }
        printf("Album: %s\n", cancion_encontrada->album);
        printf("Género: %s\n", cancion_encontrada->genre);
        printf("Tempo: %.2f\n", cancion_encontrada->tempo);
    } else {
        printf("Canción no encontrada.\n");
    }

}

void buscarPorArtista(HashMap *mapa_artistas) {
    limpiarPantalla();
    char artista[100]; // Declarar un arreglo con tamaño suficiente
    printf("Ingrese el nombre del artista: ");
    scanf(" %99[^\n]", artista); // Leer la línea completa, incluyendo espacios
    // Normalizar la clave (opcional, por ejemplo, eliminar espacios)

    Pair *resultado = searchMap(mapa_artistas, artista);

    if (resultado != NULL) {
        autor *artista_encontrado = (autor *)resultado->value;
        printf("Artista encontrado:\n");
        printf("Nombre: %s\n", artista_encontrado->artista);
        printf("Canciones:\n");
        for (cancion *cancion = list_first(artista_encontrado->songs); cancion != NULL; 
             cancion = list_next(artista_encontrado->songs)) {
            printf("  %s\n", cancion->track_name);
        }
    } else {
        printf("Artista no encontrado.\n");
    }

    
}


tempo *crearTempoList() {
    tempo *tempo_list = malloc(sizeof(tempo));
    if (tempo_list == NULL) {
        perror("Error al asignar memoria para TempoList");
        return NULL;
    }

    tempo_list->lento = list_create();
    tempo_list->medio = list_create();
    tempo_list->rapido = list_create();

    if (tempo_list->lento == NULL || tempo_list->medio == NULL || tempo_list->rapido == NULL) {
        perror("Error al crear las listas de tempo");
        free(tempo_list);
        return NULL;
    }

    return tempo_list;
}

void buscarPorTempo(tempo *tempo_list, const char *categoria) {
    List *lista_tempo;

    // Determinar la lista correspondiente según la categoría
    if (strcmp(categoria, "lento") == 0) {
        lista_tempo = tempo_list->lento;
    } else if (strcmp(categoria, "medio") == 0) {
        lista_tempo = tempo_list->medio;
    } else if (strcmp(categoria, "rapido") == 0) {
        lista_tempo = tempo_list->rapido;
    } else {
        printf("Categoría de tempo no válida. Use 'lento', 'medio' o 'rapido'.\n");
        return;
    }

    // Recorrer las canciones en la lista
    printf("Canciones en la categoría '%s':\n", categoria);
    for (cancion *c = list_first(lista_tempo); c != NULL; c = list_next(lista_tempo)) {
        printf(" %s - %s (Tempo: %.2f)\n",c->id, c->track_name, c->tempo);
    }
}

void liberarTodo(HashMap *mapa_canciones, HashMap *mapa_artistas, tempo *lista_tempo) {
    // Liberar canciones en mapa_canciones
    if (mapa_canciones) {
        Pair *par = firstMap(mapa_canciones);
        while (par != NULL) {
            cancion *c = (cancion *)par->value;

            if (c) {
                if (c->artists) {
                    for (char *artista = list_first(c->artists); artista != NULL; artista = list_next(c->artists)) {
                        if (artista) free(artista);  // solo si fue strdup
                    }
                    list_clean(c->artists);
                    free(c->artists);
                }
                free(c);
            }

            par = nextMap(mapa_canciones);
        }
    }

    // Liberar autores en mapa_artistas
    if (mapa_artistas) {
        Pair *par = firstMap(mapa_artistas);
        while (par != NULL) {
            autor *a = (autor *)par->value;

            if (a) {
                if (a->artista) free(a->artista);
                if (a->songs) {
                    list_clean(a->songs);
                    free(a->songs);
                }
                free(a);
            }

            par = nextMap(mapa_artistas);
        }
    }

    // Liberar listas de tempo
    if (lista_tempo) {
        if (lista_tempo->lento) {
            list_clean(lista_tempo->lento);
            free(lista_tempo->lento);
        }

        if (lista_tempo->medio) {
            list_clean(lista_tempo->medio);
            free(lista_tempo->medio);
        }

        if (lista_tempo->rapido) {
            list_clean(lista_tempo->rapido);
            free(lista_tempo->rapido);
        }

        free(lista_tempo);
    }
}
int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario

  // Inicializa un mapa para almacenar canciones por ID
  HashMap *mapa_canciones = createMap(120000);
  // Inicializa un mapa para almacenar canciones por artista
  // Iniciado con 120,000 analizando el peor caso, que cada cancion tenga un artista distinto
  HashMap *mapa_artistas = createMap(120000);
  // Inicializa un lista para almacenar canciones por tempo
  tempo *lista_tempo = crearTempoList();
  bool cargado = false;

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      if (cargado) {
        printf("Ya se han cargado las canciones.\n");
        break;
      }
      leer_canciones(mapa_canciones,mapa_artistas, lista_tempo);
      cargado = true;
      break;
    case '2':
      buscarPorId(mapa_canciones);
      break;
    case '3':
      buscarPorArtista(mapa_artistas);
      break;
    case '4':{
      char categoria[10];
      printf("Ingrese la categoría de tempo (lento, medio, rapido): ");
      scanf("%s", categoria);
      buscarPorTempo(lista_tempo, categoria);
    break;
    }
    }
    presioneTeclaParaContinuar();

  } while (opcion != '5');
  liberarTodo(mapa_canciones, mapa_artistas, lista_tempo);
  return 0;
}