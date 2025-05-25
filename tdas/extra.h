#ifndef EXTRA_H
#define EXTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

//Funcion que lee una línea del archivo CSV y la separa en campos usando el delimitador dado
char **leer_linea_csv(FILE *archivo, char separador);

// Función para comparar cadenas
List *split_string(const char *str, const char *delim);

// Función para limpiar la pantalla
void limpiarPantalla();

// Función que espera a que el usuario presione una tecla para continuar
void presioneTeclaParaContinuar();

#endif