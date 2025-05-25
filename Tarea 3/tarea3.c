#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  char id[16]; // 
  char nombre[100]; // Nombre del mapa
  char descripcion[200]; // Descripción
  List* items; // Lista de ítems asociados
  int arriba; // ID del mapa al que se puede avanzar hacia arriba
  int abajo; // ID del mapa al que se puede avanzar hacia abajo
  int izquierda; // ID del mapa al que se puede avanzar hacia la izquierda
  int derecha; // ID del mapa al que se puede avanzar hacia la derecha
  bool is_final; // Indica si es un mapa final
} mapa;

typedef struct {
  char nombre[100];
  int valor;
  int peso;
} Item;

typedef struct {
  List* inventario;
  int peso_total;
  int puntaje;
  float tiempo;
  mapa* actual;
} Jugador;



void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("           Bienvenido a GraphQuest");
  puts("========================================");
  puts("1) Cargar laberinto");
  puts("2) Iniciar juego");
  puts("3) Salir del juego");
}


// Menú principal
void mostrarMenuJuego() {
  limpiarPantalla();
  puts("========================================");
  puts("           incia tu aventura");
  puts("========================================");

  puts("1) Recoger Ítem(s)");
  puts("2) Descartar Ítem(s)");
  puts("3) Avanzar en una Dirección");
  puts("4) Reiniciar Partida");
  puts("5) Estadísticas del Jugador");
  puts("6) Salir del juego");
}

void freeHashMap(HashMap* grafo) {
    for (Pair* par = firstMap(grafo); par != NULL; par = nextMap(grafo)) {
        mapa* m = (mapa*)par->value;
        for (Item* item = list_first(m->items); item != NULL; item = list_next(m->items)) {
            free(item);
        }
        list_clean(m->items);
        free(m->items);
        free(m);
        free(par->key);
    }
    // Aquí deberías liberar la estructura HashMap si tienes una función para ello
}

void leer_escenarios(HashMap *mapaInicial) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV


  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    mapa *nuevo_mapa = malloc(sizeof(mapa));
    if (nuevo_mapa == NULL) {
      perror("Error al asignar memoria para nuevo mapa");
      continue; // Si no se puede asignar memoria, salta a la siguiente iteración
    }
    int id_item = 1;
    printf("ID: %d\n", atoi(campos[0]));
    strcpy(nuevo_mapa->id, campos[0]);
    printf("Nombre: %s\n", campos[1]);
    strcpy(nuevo_mapa->nombre, campos[1]); // Asigna el nombre del mapa
    printf("Descripción: %s\n", campos[2]);
    strcpy(nuevo_mapa->descripcion, campos[2]); // Asigna la descripción del mapa

    List* items = split_string(campos[3], ";");
    nuevo_mapa->items = list_create();

    printf("Items: \n");
    for(char *item = list_first(items); item != NULL; item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
        Item* nuevo_item = malloc(sizeof(Item));
        if (nuevo_item == NULL) {
            perror("Error al asignar memoria para nuevo item");
            continue; // Si no se puede asignar memoria, salta a la siguiente iteración
        }
        strcpy(nuevo_item->nombre, item_name); // Asigna el nombre del ítem
        nuevo_item->valor = item_value; // Asigna el valor del ítem
        nuevo_item->peso = item_weight; // Asigna el peso del ítem
        // Agrega el ítem a la lista de ítems del mapa
        list_pushBack(nuevo_mapa->items, nuevo_item);
        list_clean(values);
        free(values);
    }
    
    int arriba = atoi(campos[4]);
    nuevo_mapa->arriba = arriba; // Asigna el ID del mapa al que se puede avanzar hacia arriba
    int abajo = atoi(campos[5]);
    nuevo_mapa->abajo = abajo; // Asigna el ID del mapa al que se puede avanzar hacia abajo
    int izquierda = atoi(campos[6]);
    nuevo_mapa->izquierda = izquierda; // Asigna el ID del mapa al que se puede avanzar hacia la izquierda
    int derecha = atoi(campos[7]);
    nuevo_mapa->derecha = derecha; // Asigna el ID del mapa al que se puede avanzar hacia la derecha

    
    if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);
    
    int is_final = atoi(campos[8]);
    nuevo_mapa->is_final = (is_final == 1); // Asigna si es un mapa final
    if (is_final) printf("Es final\n");
    
    insertMap(mapaInicial, strdup(nuevo_mapa->id), nuevo_mapa);
    printf("Mapa %s cargado correctamente.\n", nuevo_mapa->id);

    list_clean(items);
    free(items);
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
}

// Función para ver los items disponibles en el mapa actual y recogerlos

mapa* copiarMapa(const mapa* original) {
    mapa* copia = malloc(sizeof(mapa));
    if (!copia) return NULL;
    memcpy(copia, original, sizeof(mapa));
    // Copiar la lista de items
    copia->items = list_create();
    for (Item* item = list_first(original->items); item != NULL; item = list_next(original->items)) {
        Item* nuevo_item = malloc(sizeof(Item));
        memcpy(nuevo_item, item, sizeof(Item));
        list_pushBack(copia->items, nuevo_item);
    }
    return copia;
}

HashMap* copiarGrafo(HashMap* original) {
    HashMap* copia = createMap(50);
    for (Pair* par = firstMap(original); par != NULL; par = nextMap(original)) {
        mapa* mapa_copia = copiarMapa((mapa*)par->value);
        insertMap(copia, strdup(par->key), mapa_copia);
    }
    return copia;
}

//Funcion para ver el inventario del jugador
void verInventario(Jugador* jugador) {
  if (jugador == NULL || jugador->inventario == NULL) {
    puts("Error: Jugador o inventario no inicializados.");
    return;
  }

  if (list_first(jugador->inventario) == NULL) {
    puts("Tu inventario está vacío.");
    return;
  }

  puts("Inventario:");
  for (Item* item = list_first(jugador->inventario); item != NULL; item = list_next(jugador->inventario)) {
    printf("- %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
  }
  printf("Peso total: %d kg\n", jugador->peso_total);
  printf("Puntaje total: %d pts\n", jugador->puntaje);
}

void estadisticasJugador(Jugador* jugador) {
  if (jugador == NULL) {
    puts("Error: Jugador no inicializado.");
    return;
  }
  printf("Estadísticas del Jugador:\n");
  printf("Peso total en inventario: %d kg\n", jugador->peso_total);
  printf("Puntaje total: %d pts\n", jugador->puntaje);
  printf("Tiempo restante: %.2f segundos\n", jugador->tiempo);
  if (jugador->actual != NULL) {
    printf("Mapa actual: %s\n", jugador->actual->nombre);
  } else {
    puts("No hay mapa actual.");
  }
  verInventario(jugador);
}


void recogerItem(Jugador* jugador, Item *item) {
  if (jugador == NULL || item == NULL) {
    puts("Error: Jugador o ítem no inicializados.");
    return;
  }

  // Agregar el ítem al inventario del jugador
  list_pushBack(jugador->inventario, item);
  jugador->peso_total += item->peso;
  jugador->puntaje += item->valor;
  printf("Has recogido el ítem: %s\n", item->nombre);
}

void itemsDisponibles(mapa* m, Jugador* jugador) {
  do {
    limpiarPantalla();
    if (list_first(m->items) == NULL) {
    puts("No hay más ítems disponibles en este mapa.");
    return;
   }
    puts("Ítems disponibles:");
    for (Item* item = list_first(m->items); item != NULL; item = list_next(m->items)) {
    printf("- %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
    }
    char nombre_item[100];
    puts("Selecciona el ítem a recoger (ingresa el nombre o 'salir' para terminar):");
    printf("Nombre: ");
    scanf(" %[^\n]", nombre_item);
    if (strcmp(nombre_item, "salir") == 0) break;
    Item* item_recogido = NULL;
    for (Item* item = list_first(m->items); item != NULL; item = list_next(m->items)) {
      if (strcmp(item->nombre, nombre_item) == 0) {
        item_recogido = item;
        break;
      }
    }
    if (item_recogido != NULL) {
      recogerItem(jugador, item_recogido);
      list_popCurrent(m->items); // Elimina el ítem actual
      puts("Ítem recogido.");
    } else {
      puts("Ítem no encontrado.");
    }
    presioneTeclaParaContinuar();
  } while (list_first(m->items) != NULL);
  jugador->tiempo -= 1;
  printf("Se ha descontado 1 s, Tiempo restante: %.2f segundos\n", jugador->tiempo);
}

void reiniciarPartida(Jugador* jugador) {
  if (jugador == NULL) {
    puts("Error: Jugador no inicializado.");
    return;
  }
  // Reiniciar el inventario del jugador
  list_clean(jugador->inventario);
  jugador->peso_total = 0;
  jugador->puntaje = 0;
  jugador->tiempo = 15.00; // Reiniciar el tiempo
  jugador->actual = NULL; // Reiniciar el mapa actual
  puts("Partida reiniciada.");
}



void avanzarDireccion(HashMap *grafo,mapa* m, Jugador* jugador) {
  if (m == NULL || jugador == NULL) {
    puts("Error: Mapa o jugador no inicializados.");
    return;
  }
  puts("Selecciona una dirección (w: arriba, s: abajo, a: izquierda, d: derecha):");
  char direccion;
  int nuevo_id = -1;
  if (m->arriba != -1) printf("Arriba: %d\n", m->arriba );
  if (m->abajo != -1) printf("Abajo: %d\n", m->abajo);
    
  if (m->izquierda != -1) printf("Izquierda: %d\n", m->izquierda);
  if (m->derecha != -1) printf("Derecha: %d\n", m->derecha);
  scanf(" %c", &direccion); // Leer la dirección ingresada por el usuario
  //Mostrar las opciones de movimiento
  switch (direccion) {
    case 'w': // Arriba
      nuevo_id = m->arriba;
      break;
    case 's': // Abajo
      nuevo_id = m->abajo;
      break;
    case 'a': // Izquierda
      nuevo_id = m->izquierda;
      break;
    case 'd': // Derecha
      nuevo_id = m->derecha;
      break;
    default:
      puts("Dirección no válida. Usa 'w', 's', 'a' o 'd'.");
      return;
  }

  if (nuevo_id == -1) {
    
    return;
  }

  char clave[10]; sprintf(clave, "%d", nuevo_id);
  Pair* siguienteMapa = searchMap(grafo, clave);
  if (siguienteMapa != NULL) {
    jugador->actual = (mapa*)siguienteMapa->value;
    printf("Te has movido al mapa: %s\n", jugador->actual->nombre);
    float tiempo_restante = (jugador->peso_total+1)/10.0; // Calcula el tiempo restante basado en el peso total
    jugador->tiempo -= tiempo_restante; // Reduce el tiempo restante
  } else {
    puts("No se pudo encontrar el mapa en esa dirección.");
  }
  if (strcmp(jugador->actual->id, "16") == 0) {
    puts("¡Has llegado a un mapa final! ¡Felicidades!");
    estadisticasJugador(jugador);
    presioneTeclaParaContinuar();
    return; // Termina el juego
    jugador->tiempo = 0; // Termina el juego
    }
}

void descartarObjeto(mapa* m, Item* item) {
  if (m == NULL || item == NULL) {
    puts("Error: Mapa o ítem no inicializados.");
    return;
  }
  // Agregar el ítem al mapa
  list_pushBack(m->items, item);
  printf("Has descartado el ítem: %s al piso del mapa.\n", item->nombre);
}

void descartarItem(Jugador* jugador, mapa* m) {
  do {
    limpiarPantalla();
    if (list_first(jugador->inventario) == NULL) {
      puts("No hay más ítems en tu inventario.");
      return;
    }
    puts("Ítems en tu inventario:");
    for (Item* item = list_first(jugador->inventario); item != NULL; item = list_next(jugador->inventario)) {
      printf("- %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
    }
    char nombre_item[100];
    puts("Selecciona el ítem a descartar (ingresa el nombre o 'salir' para terminar):");
    printf("Nombre: ");
    scanf(" %[^\n]", nombre_item);
    if (strcmp(nombre_item, "salir") == 0) break;
    Item* item_recogido = NULL;
    for (Item* item = list_first(jugador->inventario); item != NULL; item = list_next(jugador->inventario)) {
      if (strcmp(item->nombre, nombre_item) == 0) {
        item_recogido = item;
        break;
      }
    }
    if (item_recogido != NULL) {
      descartarObjeto(jugador->actual, item_recogido);
      jugador->peso_total -= item_recogido->peso;
      jugador->puntaje -= item_recogido->valor;
      list_popCurrent(jugador->inventario); // Elimina el ítem actual
      puts("Ítem descartado.");
    } else {
      puts("Ítem no encontrado.");
    }
    presioneTeclaParaContinuar();
  } while (list_first(jugador->inventario) != NULL);
  jugador->tiempo -= 1;
  printf("Se ha descontado 1 s, Tiempo restante: %.2f segundos\n", jugador->tiempo);
}


void deseaReiniciarPartida(Jugador* jugador) {
  if (jugador == NULL) {
    puts("Error: Jugador no inicializado.");
    return;
  }
  char respuesta;
  puts("¿Deseas reiniciar la partida? (s/n)");
  scanf(" %c", &respuesta);
  if (respuesta == 's' || respuesta == 'S') {
    reiniciarPartida(jugador);
  } else {
    puts("Partida no reiniciada.");
  }
}

void iniciarJuego(HashMap* grafo, HashMap* grafo_original,Jugador* jugador) {
  if (grafo == NULL || jugador == NULL) {
    puts("Error: Grafo o jugador no inicializados.");
    return;
  }

  // Inicializar el jugador
  jugador->inventario = list_create();
  jugador->peso_total = 0;
  jugador->puntaje = 0;
  jugador->tiempo = 15.00;

  // Asignar el mapa inicial al jugador
  char clave_inicio[] = "1";
  Pair* primerMapa = searchMap(grafo, clave_inicio);
  if (primerMapa != NULL) {
    jugador->actual = (mapa*)primerMapa->value;
    printf("Has iniciado en el mapa: %s\n", jugador->actual->nombre);
  } else {
    puts("No se encontró el mapa con ID 1.");
    return;
  }
  char opcion;
  do {
    if (jugador->tiempo <= 0) {
      limpiarPantalla();
      puts("Se acabó el tiempo. ¡Juego terminado!");
      estadisticasJugador(jugador);
      deseaReiniciarPartida(jugador);
      if (jugador->tiempo <= 0) return; 
    }
    mostrarMenuJuego();
    scanf(" %c", &opcion);
    switch (opcion) {
      case '1':
        // Recoger ítem(s)
        itemsDisponibles(jugador->actual, jugador);
        break;
      case '2':
        // Descartar ítem(s)
        descartarItem(jugador, jugador->actual);
        break;
      case '3':
        // Avanzar en una dirección
        avanzarDireccion(grafo,jugador->actual, jugador);
        break;
      case '4':
        // Reiniciar partida
        freeHashMap(grafo);
        grafo = copiarGrafo(grafo_original);
        reiniciarPartida(jugador);
        Pair* primerMapa = searchMap(grafo, "1");
        if (primerMapa != NULL) {
          jugador->actual = (mapa*)primerMapa->value;
        printf("Has reiniciado en el mapa: %s\n", jugador->actual->nombre);
        }
        break;
      case '5':
        estadisticasJugador(jugador);
        break;
    }
    presioneTeclaParaContinuar();
  } while (opcion != '6');
  // Liberar recursos al salir del juego
  freeHashMap(grafo);
  puts("Saliendo del juego...");
  return;
}

int main() {
  char opcion;
  HashMap* grafo_original = createMap(50);
  HashMap* grafo = NULL;
  Jugador* jugador = malloc(sizeof(Jugador));
  if (jugador == NULL) {
    puts("Error al crear el jugador.");
    return 1;
  }
  bool juego_iniciado = false;
  do {
    mostrarMenuPrincipal();
    scanf(" %c", &opcion);
    switch (opcion) {
      case '1': 
        if (juego_iniciado) {
          puts("Ya se ha cargado el laberinto.");
          break;
        }
        leer_escenarios(grafo_original);
        juego_iniciado = true;
        break;
      case '2': 
        if(!juego_iniciado) {
          puts("Por favor, primero carga el laberinto.");
        } else {
          grafo = copiarGrafo(grafo_original);
          iniciarJuego(grafo, grafo_original, jugador);
        }
        break;
    }
    presioneTeclaParaContinuar();
  } while (opcion != '3');
  free(jugador);
  freeHashMap(grafo_original);
  puts("Gracias por jugar GraphQuest. ¡Hasta luego!");
  return 0;
}