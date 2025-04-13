#ifndef LIST_H
#define LIST_H

typedef struct List List;

//Crer Lista
List * createList(void);

//Acceder al primer elemento
void * firstList(List * list);

//Acceder al siguiente elemento
void * nextList(List * list);

//Acceder al último elemento
void * lastList(List * list);

//Acceder al elemento anterior
void * prevList(List * list);

//insertar un elemento al inicio
void pushFront(List * list, void * data);

//insertar un elemento al final
void pushBack(List * list, void * data);

//insertar un elemento en la posición actual
void pushCurrent(List * list, void * data);

//Eliminar el primer elemento
void * popFront(List * list);

//Eliminar el último elemento
void * popBack(List * list);

//Eliminar el elemento actual
void * popCurrent(List * list);

//Limpiar la lista
void cleanList(List * list);

// Función para insertar ordenado de acuerdo a la función lower_than
void list_sortedInsert(List *L, void *data,
    int (*lower_than)(void *data1, void *data2));


#endif