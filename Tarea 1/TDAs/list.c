#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node Node;

struct Node {
  void * data;
  Node * next;
  Node * prev;
};

struct List {
  Node * head;
  Node * tail;
  Node * current;
};

typedef List List;

Node *createNode(void *data) {
  Node *new = (Node *)malloc(sizeof(Node));
  if (new == NULL) {
    fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
    exit(EXIT_FAILURE);
  }
  new->data = data;
  new->prev = NULL;
  new->next = NULL;
  return new;
}
List * createList() {
  List*lista = (List *) malloc(sizeof(List));
  lista->head = NULL;
  lista->tail = NULL;
  lista->current = NULL;
  return lista;
}

void * firstList(List * list) {
  if (list->head == NULL) return NULL;
    list->current = list->head;
    return list->current->data;
}

void * nextList(List * list) {
  if (list->current == NULL) return NULL;
  list->current = list->current->next;
  return (list->current) ? list->current->data : NULL;
}

void * lastList(List * list) {
  if (list->tail == NULL ) return NULL;
  list->current = list->tail;
  return list->current->data;
}

void * prevList(List * list) {
  if(list->current == NULL) return NULL;
  list->current = list->current->prev;
  return (list->current) ? list->current->data : NULL;;
}

void pushFront(List *list, void *data) {
  if (list == NULL) return; // Validar lista
  Node *nuevoNodo = (Node *)malloc(sizeof(Node));
  assert(nuevoNodo != NULL);
  nuevoNodo->prev = NULL;
  nuevoNodo->data = data;
  nuevoNodo->next = list->head;
  if (list->head) list->head->prev = nuevoNodo;
  if (list->tail == NULL) list->tail = nuevoNodo;
  list->head = nuevoNodo;
}
void pushBack(List * list, void * data) {
  if (list->tail == NULL) { // Si la lista está vacía
    Node *nuevoNodo = createNode(data);
    list->head = nuevoNodo;
    list->tail = nuevoNodo;
    list->current = nuevoNodo;
    return;
  }
  list->current = list->tail;
  pushCurrent(list,data);
}

void pushCurrent(List *list, void *data) {
  if (list == NULL) return; // Validar lista
  if (list->current == NULL) { // Si la lista está vacía
    pushBack(list, data); // Insertar como el único nodo
    return;
  }

  Node *nuevoNodo = createNode(data);

  if (list->current == list->tail) { // Si current está en el último nodo
    list->current->next = nuevoNodo;
    nuevoNodo->prev = list->current;
    list->tail = nuevoNodo;
  } else { // Insertar en el medio
    nuevoNodo->next = list->current->next;
    nuevoNodo->prev = list->current;
    list->current->next->prev = nuevoNodo;
    list->current->next = nuevoNodo;
  }
}

void * popFront(List * list) {
  list->current = list->head;
  return popCurrent(list);
}

void * popBack(List * list) {
  list->current = list->tail;
  return popCurrent(list);
}

void *popCurrent(List *list) {
  if (list == NULL || list->current == NULL) return NULL;

  Node *nodoAEliminar = list->current;
  void *dato = nodoAEliminar->data;

  // Caso 1: La lista tiene un solo nodo
  if (list->head == list->tail) {
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
  }
  // Caso 2: El nodo a eliminar es el primero
  else if (list->current == list->head) {
    list->head = list->current->next;
    list->head->prev = NULL;
    list->current = list->head;
  }
  // Caso 3: El nodo a eliminar es el último
  else if (list->current == list->tail) {
    list->tail = list->current->prev;
    list->tail->next = NULL;
    list->current = list->tail; // Actualizar current al nuevo último nodo
  }
  // Caso 4: El nodo a eliminar está en el medio
  else {
    list->current->prev->next = list->current->next;
    list->current->next->prev = list->current->prev;
    list->current = list->current->next; // Mover current al siguiente nodo
  }

  free(nodoAEliminar); // Liberar el nodo eliminado
  return dato; // Devolver el dato almacenado en el nodo
}

void list_sortedInsert(List *L, void *data,
  int (*lower_than)(void *data1, void *data2)) {
if (L == NULL) {
return; // Lista no inicializada
}

// Caso especial: inserción al principio o en lista vacía
if (L->head == NULL || lower_than(data, L->head->data)) {
pushFront(L, data);
return;
}

// Caso general: encontrar la posición correcta para insertar
Node *current = L->head;
while (current->next != NULL && !lower_than(data, current->next->data)) {
current = current->next;
}

// Preparar para usar list_pushCurrent
L->current = current;

// Insertar el nodo en la posición actual
pushCurrent(L, data);
}

void cleanList(List *list) {
  if (list == NULL) return;
  while (list->head != NULL) {
    popFront(list);
  }
  free(list); // Liberar la estructura de la lista
}