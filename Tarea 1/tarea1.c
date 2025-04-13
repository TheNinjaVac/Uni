#include "tdas/list.h"
#include "tdas/extra.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Ticket {
  int id; // ID del ticket
  char rut[20]; // RUT del cliente
  char nombre[100]; // Nombre del cliente
  int prioridad; // Prioridad del ticket : Alto, Medio, Bajo
  char observacion[200]; // Observación del ticket
  char estado[20]; // Estado del ticket: Antendido, Pendiente, Cerrado
} Ticket;


// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("Sistema de Gestión de Tickets");

  puts("1) Registrar Tickets");
  puts("2) Asignar prioridad a ticket");
  puts("3) Mostrar lista de tickets pendientes");
  puts("4) Procesar siguiente ticket ");
  puts("5) Buscar ticket por ID y mostrar detalles");
  puts("6) Salir");
}

void registrar_tickets(List *tickets) {
  Ticket *ticket = firstList(tickets);
  Ticket *nuevoTicket = (Ticket *)malloc(sizeof(Ticket));
  if (nuevoTicket == NULL) {
    puts("Error al asignar memoria para el nuevo ticket.");
    return;
  }
  printf("Ingrese el ID del ticket: ");
  scanf("%d", &nuevoTicket->id);
  while (ticket != NULL) {
    if (ticket->id == nuevoTicket->id) {
      puts("Ticket ya existe.");
      free(nuevoTicket);
      return;
    }
    ticket = nextList(tickets);
  }
  printf("Ingrese el RUT del cliente: ");
  scanf("%19s", nuevoTicket->rut); // Limitar a 19 caracteres
  printf("Ingrese el nombre del cliente: ");
  scanf("%99s", nuevoTicket->nombre); // Limitar a 99 caracteres
  printf("Ingrese la observación del ticket: ");
  scanf("%199s", nuevoTicket->observacion); // Limitar a 199 caracteres
  strcpy(nuevoTicket->estado, "Pendiente");
  nuevoTicket->prioridad = 4; // Inicializar prioridad a 4, un valor por defecto a la prioridad
  nuevoTicket->estado[0] = '\0'; // Inicializar estado como vacío
  strcpy(nuevoTicket->estado, "Pendiente"); // Inicializar estado como "Pendiente"
  // Agregar el ticket a la lista
  pushBack(tickets, nuevoTicket);
  printf("Ticket registrado con éxito: ID=%d, Nombre=%s\n", nuevoTicket->id, nuevoTicket->nombre);
}

int compararPrioridad(void *data1, void *data2) {
  Ticket *ticket1 = (Ticket *)data1;
  Ticket *ticket2 = (Ticket *)data2;
  // Retorna 1 si la prioridad de ticket1 es menor que la de ticket2
  return ticket1->prioridad < ticket2->prioridad;
}

void asignarPrioridad(List *tickets) {
  if (tickets == NULL || firstList(tickets) == NULL) {
    puts("No hay tickets registrados.");
    return;
  }

  Ticket *ticket = firstList(tickets);
  Ticket *ticketObjetivo = NULL;
  int idTicket;

  printf("Ingrese el ID del ticket para asignar prioridad: ");
  scanf("%d", &idTicket);

  // Buscar el ticket por ID
  while (ticket != NULL) {
    if (ticket->id == idTicket) {
      ticketObjetivo = ticket;
      break;
    }
    ticket = nextList(tickets);
  }

  if (ticketObjetivo == NULL) {
    printf("Error: No se encontró un ticket con ID=%d.\n", idTicket);
    return;
  }

  printf("Ingrese la nueva prioridad (1-Alto, 2-Media, 3-Baja): ");
  int nuevaPrioridad;
  scanf("%d", &nuevaPrioridad);

  if (nuevaPrioridad < 1 || nuevaPrioridad > 3) {
    puts("Prioridad inválida. Debe ser 1 (Alta), 2 (Media) o 3 (Baja).");
    return;
  }

  if (ticketObjetivo->prioridad == nuevaPrioridad) {
    puts("El ticket ya tiene asignada la misma prioridad.");
    return;
  }

  // Actualizar la prioridad
  ticketObjetivo->prioridad = nuevaPrioridad;

  // Eliminar el ticket de su posición actual
  popCurrent(tickets);

  // Insertar el ticket en la lista de manera ordenada
  list_sortedInsert(tickets, ticketObjetivo, compararPrioridad);

  puts("Prioridad asignada correctamente.");
}

void buscarTicket(List *ticket){
  if (ticket == NULL || firstList(ticket) == NULL) { // Validar si la lista está vacía
    puts("No hay tickets registrados.");
    return;
  }
  Ticket *ticketBuscado = (Ticket *)firstList(ticket); // Inicializar el iterador al primer elemento
  int idTicket;
  printf("Ingrese el ID del ticket a buscar: ");
  scanf("%d", &idTicket);

  while (ticketBuscado != NULL) {
    if (ticketBuscado->id == idTicket) {
      printf("Detalles del ticket:\n");
      printf("ID: %d, RUT: %s, Nombre: %s, Prioridad: %d, Observación: %s, Estado: %s\n",
             ticketBuscado->id, ticketBuscado->rut, ticketBuscado->nombre,
             ticketBuscado->prioridad, ticketBuscado->observacion,
             ticketBuscado->estado);
      return;
    }
    ticketBuscado = (Ticket *)nextList(ticket);
  }
  puts("Ticket no encontrado.");
}
void mostrarTicketsPendientes(List *tickets, List *ticketsAtendidos) {
  if ((tickets == NULL || firstList(tickets) == NULL) &&
      (ticketsAtendidos == NULL || firstList(ticketsAtendidos) == NULL)) {
    puts("No hay tickets registrados.");
    return;
  }
  if (tickets != NULL && firstList(tickets) != NULL) {
    Ticket *ticketPendiente = (Ticket *)firstList(tickets);
    printf("Tickets pendientes:\n");
    while (ticketPendiente != NULL) {
      if (ticketPendiente->estado != NULL && strcmp(ticketPendiente->estado, "Pendiente") == 0) {
        printf("ID: %d, RUT: %s, Nombre: %s, Prioridad: %d, Observación: %s\n",
               ticketPendiente->id, ticketPendiente->rut, ticketPendiente->nombre,
               ticketPendiente->prioridad, ticketPendiente->observacion);
      }
      ticketPendiente = (Ticket *)nextList(tickets);
    }
  } else {
    puts("No hay tickets pendientes.");
  }
  if (ticketsAtendidos != NULL && firstList(ticketsAtendidos) != NULL) {
    Ticket *ticketAtendido = (Ticket *)firstList(ticketsAtendidos);
    printf("Tickets atendidos:\n");
    while (ticketAtendido != NULL) {
      if (ticketAtendido->estado != NULL && strcmp(ticketAtendido->estado, "Atendido") == 0) {
        printf("ID: %d, RUT: %s, Nombre: %s, Prioridad: %d, Observación: %s\n",
               ticketAtendido->id, ticketAtendido->rut, ticketAtendido->nombre,
               ticketAtendido->prioridad, ticketAtendido->observacion);
      }
      ticketAtendido = (Ticket *)nextList(ticketsAtendidos);
    }
  } else {
    puts("No hay tickets atendidos.");
  }
  return;
}

void atenderTickets(List *tickets, List *ticketsAtendidos) {
  if (tickets == NULL || firstList(tickets) == NULL) { // Validar si la lista está vacía
    puts("No hay tickets pendientes.");
    return;
  }
  Ticket *ticketObjetivo = NULL;
  Ticket *ticket = (Ticket *)firstList(tickets); // Inicializar el iterador al primer elemento
  while (ticket != NULL) {
    if (strcmp(ticket->estado, "Pendiente") == 0) {
      printf("Atendiendo ticket ID: %d\n", ticket->id);
      strcpy(ticket->estado, "Atendido");
      ticketObjetivo = ticket; // Guardar el ticket atendido
      // Mover el ticket atendido a la lista de tickets atendidos
      pushBack(ticketsAtendidos, ticketObjetivo);
      popCurrent(tickets); // Eliminar el ticket de la lista de pendientes
      break; // Salir después de atender el primer ticket pendiente
    }
    ticket = (Ticket *)nextList(tickets);
  }
}
int main() {
  int opcion;
  List *ticketsPendientes = createList();
  List *ticketsAtendidos = createList();
  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf("%d", &opcion);

    switch (opcion) {
    case 1:
      registrar_tickets(ticketsPendientes);
      break;
    case 2:
      asignarPrioridad(ticketsPendientes);
      break;
    case 3:
      mostrarTicketsPendientes(ticketsPendientes,ticketsAtendidos);
      break;
    case 4:
      atenderTickets(ticketsPendientes, ticketsAtendidos);
      break;
    case 5:
      buscarTicket(ticketsPendientes); // Llamar a la función buscarTicket
      break;
    case 6:
      puts("Ejecucion Terminada");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }
    presioneTeclaParaContinuar();
  } while (opcion != 6);
  cleanList(ticketsPendientes);
  cleanList(ticketsAtendidos);
  free(ticketsPendientes);
  free(ticketsAtendidos);

  return 0;
}