#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "map.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}


long hash(char *key, long capacity) {
    unsigned long hash = 0;
    while (*key)
        hash = (hash * 31 + *key++) % capacity;
    return hash;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long index = hash(key, map->capacity); // a) Aplicar la función hash
    while (map->buckets[index] != NULL && map->buckets[index]->key != NULL) { // b) Resolver colisiones
        if (is_equal(map->buckets[index]->key, key)) {
            free(map->buckets[index]->value); // Liberar el valor anterior si es dinámico
            map->buckets[index]->value = value;
            map->current = index;
        return;
}
        index = (index + 1) % map->capacity; // Avanzar al siguiente índice (resolución lineal)
    }

    // c) Insertar el nuevo par en la casilla encontrada
    Pair * newPair = createPair(key, value);
    map->buckets[index] = newPair;
    map->size++;
    map->current = index; // Actualizar el índice current

    // Verificar si se necesita redimensionar el mapa
    if (map->size > map->capacity * 0.75) { // Umbral del factor de carga
        enlarge(map);
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; // No borrar (testing purposes)

    // a) Crear una variable auxiliar para mantener el arreglo actual
    Pair **old_buckets = map->buckets;
    long old_capacity = map->capacity;

    // b) Duplicar el valor de la capacidad
    map->capacity *= 2;

    // c) Asignar un nuevo arreglo con la nueva capacidad
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
    
    // d) Inicializar el tamaño a 0
    map->size = 0;

    // e) Insertar los elementos del arreglo antiguo en el nuevo mapa
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
        }
    }

    // Liberar la memoria del arreglo antiguo
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        perror("Error al asignar memoria para el mapa");
        return NULL;
    }
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    map->size = 0;
    map->capacity = capacity;
    map->current = -1; // Inicialmente no hay elementos accedidos
    return map;
}

void eraseMap(HashMap * map, char * key) {
    // Buscar el par asociado a la clave
    Pair * pair = searchMap(map, key);

    if (pair == NULL) {
        // Si no se encuentra el par, no hacer nada
        return;
    }

    // Invalidar el par asignando NULL a la clave
    pair->key = NULL;

    // Actualizar el tamaño del mapa
    map->size--;
    free(pair->value); // Liberar el valor si es dinámico
    free(pair);        // Liberar el par
}

Pair * searchMap(HashMap * map, char * key) {
    long index = hash(key, map->capacity); // a) Aplicar la función hash

    while (map->buckets[index] != NULL) { // b) Resolver colisiones
        if (map->buckets[index]->key != NULL && is_equal(map->buckets[index]->key, key)) {
            map->current = index; // Actualizar el índice current
            return map->buckets[index]; // Retornar el par encontrado
        }
        index = (index + 1) % map->capacity; // Avanzar al siguiente índice (arreglo circular)
    }

    // c) Si se llega a una casilla nula, retornar NULL
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) { // Recorrer desde el inicio
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) { // Encontrar el primer par válido
            map->current = i; // Actualizar el índice current
            return map->buckets[i]; // Retornar el primer par válido
        }
    }
    return NULL; // Si no hay pares válidos, retornar NULL
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) { // Continuar desde el índice actual
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) { // Encontrar el siguiente par válido
            map->current = i; // Actualizar el índice current
            return map->buckets[i]; // Retornar el siguiente par válido
        }
    }
    return NULL; // Si no hay más pares válidos, retornar NULL
}
