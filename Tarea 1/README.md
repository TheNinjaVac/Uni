
## Código de tarea1
Para ejecutar tarea1 primero debemos compilar (en la carpeta raíz)
````
gcc tdas/*.c tarea1.c -Wno-unused-result -o tarea1
````

Y luego ejecutar:
````
./tarea1
````

## Sinopsis
Este programa es un Sistema de Gestión de Tickets en C, que permite registrar, clasificar, visualizar, procesar y buscar tickets de atención al cliente, utilizando una estructura de lista dinámica (TDA List).

El programa implementa un sistema interactivo de consola para la gestión de tickets de atención al cliente, organizado por prioridad. A través de un menú, el usuario puede:

1. Registrar Tickets: Se ingresan los datos del cliente (ID, RUT, nombre, observación), inicializando el ticket como pendiente y sin prioridad asignada.

2. Asignar Prioridad a un Ticket: Permite modificar la prioridad de un ticket existente (1 = Alta, 2 = Media, 3 = Baja), reorganizándolo dentro de la lista según esa prioridad.

3. Mostrar Tickets Pendientes y Atendidos: Lista los tickets en estado pendiente y atendido, diferenciando entre ambas listas.

4. Procesar el Siguiente Ticket: Atiende el primer ticket pendiente en la lista (prioridad más alta disponible), cambiando su estado a atendido y moviéndolo a otra lista.

5. Buscar Ticket por ID: Permite encontrar y mostrar los detalles de un ticket específico ingresando su ID.

6. Salir del Programa: Finaliza la ejecución y libera la memoria utilizada.

## TDAs
En la carpeta `tdas` se encuentran implementados TDAs ha utilizar (lista y extra). 
lista cuenta con las funciones:

````````
lista
````````
`Crer Lista`
## List * createList(void);

`Acceder al primer elemento`
## void * firstList(List * list);

`Acceder al siguiente elemento`
## void * nextList(List * list);

`Acceder al último elemento`
## void * lastList(List * list);

`Acceder al elemento anterior`
## void * prevList(List * list);

`insertar un elemento al inicio`
## void pushFront(List * list, void * data);

`insertar un elemento al final`
## void pushBack(List * list, void * data);

`insertar un elemento en la posición actual`
## void pushCurrent(List * list, void * data);

`Eliminar el primer elemento`
## void * popFront(List * list);

`Eliminar el último elemento`
## void * popBack(List * list);

`Eliminar el elemento actual`
## void * popCurrent(List * list);

`Limpiar la lista`
## void cleanList(List * list);

`Insertar el ticket en la lista de manera ordenada, Funcion de tips`
## list_sortedInsert
````````
extra
````````

`Función para limpiar la pantalla`
## void limpiarPantalla();

`Función para pausar la ejecución y esperar una tecla`
## void presioneTeclaParaContinuar();

## Función
1. La función registrar_tickets se encarga de crear un nuevo ticket de atención al cliente y agregarlo a la lista de tickets pendientes.

Primero, revisa si ya existe un ticket con el mismo ID que el usuario quiere ingresar. Para eso, recorre toda la lista de tickets pendientes y compara. Si encuentra un ticket con ese mismo ID, le informa al usuario que el ticket ya existe y cancela el registro.

Si no hay duplicados, la función le pide al usuario que ingrese los datos necesarios para el nuevo ticket: el RUT del cliente, su nombre y una observación. Todos estos datos se ingresan desde consola y se almacenan en una estructura nueva de tipo Ticket.

Luego, se establece que el estado del ticket es “Pendiente”, lo que indica que aún no ha sido atendido. También se le asigna una prioridad inicial por defecto, que en este caso es cero, representando que todavía no tiene prioridad asignada.

Una vez completo, el ticket se agrega al final de la lista de pendientes(porque su prioridad todavía no ha sido definida).

Finalmente, el sistema informa al usuario que el ticket fue registrado correctamente, mostrando el ID y el nombre del cliente como confirmación.

2. La función asignarPrioridad permite al usuario asignar o modificar la prioridad de un ticket ya registrado en el sistema.

Primero, verifica que la lista de tickets no esté vacía. Si no hay tickets registrados, informa al usuario y termina la función.

Luego, le pide al usuario el ID del ticket al cual quiere asignarle una prioridad. Recorre la lista buscando un ticket con ese ID. Si no lo encuentra, le informa al usuario que no existe tal ticket.

Si el ticket sí existe, le solicita al usuario una nueva prioridad. Esta puede ser: (Alta, Media, Baja )

Verifica que la prioridad ingresada sea válida y distinta de la que ya tiene el ticket. Si es inválida o si es igual a la actual, le avisa al usuario y no realiza ningún cambio.

En caso de que todo esté correcto, el ticket se elimina temporalmente de la lista y se reubica según su nueva prioridad. Esto se hace para mantener el orden en la lista: los tickets con prioridad más alta (número menor) deben estar antes que los de menor prioridad.

Para reinsertarlo, recorre la lista desde el principio, buscando el lugar adecuado. Si encuentra un ticket con menor prioridad (número más grande), lo inserta justo antes. Si no encuentra ninguno, lo agrega al final.

Finalmente, confirma al usuario que la prioridad fue asignada correctamente.

3. La función atenderTickets se encarga de procesar el siguiente ticket pendiente.

Primero, revisa si hay tickets pendientes en la lista. Si no hay ninguno, le informa al usuario y termina la función sin hacer nada.

Si sí hay tickets, recorre la lista desde el inicio, buscando el primer ticket que tenga el estado “Pendiente”. Apenas encuentra uno, lo marca como “Atendido”, cambiando su estado.

Luego, ese ticket se mueve a otra lista que guarda los tickets ya atendidos. Este paso es importante porque permite llevar un registro separado de los tickets que ya fueron procesados.

Después de moverlo, lo elimina de la lista de pendientes, porque ya fue atendido.

Por último, muestra un mensaje informando al usuario que el ticket fue atendido, incluyendo su ID.

Cabe destacar que solo atiende un ticket por vez: el primero que encuentre con estado "Pendiente". Esto permite que el proceso sea controlado paso a paso, siempre atendiendo al más prioritario disponible (ya que los tickets están ordenados por prioridad si se usó correctamente la función anterior).

4. Esta función muestra todos los tickets que están registrados en el sistema, dividiéndolos en dos grupos: pendientes y atendidos.

Primero, revisa si hay tickets pendientes en la lista correspondiente. Si no hay ninguno, muestra un mensaje diciendo que no hay tickets pendientes.

Si sí hay, recorre la lista y por cada ticket que tenga el estado "Pendiente", muestra sus datos: ID, RUT, nombre del cliente, prioridad y observación. (Aunque también menciona una hora en el mensaje, el código no la maneja, así que sería un dato pendiente de implementar).

Luego, hace lo mismo pero con la lista de tickets que ya fueron atendidos. Si no hay ninguno, también lo informa. Si hay, recorre la lista y muestra todos los tickets con estado "Atendido", con la misma información.

En resumen, es una función de reporte general que le permite al usuario ver en qué estado está cada ticket del sistema.

5. Esta función permite al usuario buscar un ticket específico por su ID.

Primero, verifica si la lista de tickets está vacía. Si lo está, le avisa al usuario que no hay registros.

Si hay tickets, pide al usuario que ingrese el ID del ticket que desea consultar. Luego, recorre la lista de tickets uno por uno, comparando el ID ingresado con el de cada ticket.

Si encuentra uno que coincida, muestra todos sus datos: ID, RUT, nombre del cliente, prioridad, observación y estado. Esto le permite al usuario revisar todos los detalles de un ticket específico.

Si recorre toda la lista y no encuentra ningún ticket con ese ID, informa al usuario que no se encontró.

