/*==================[inclusions]=============================================*/

#include "os.h"
#include "queue.h"
#include "board.h"
#include "ciaaUART.h"
#include "ciaaIO.h"

#include <stdlib.h>
#include <string.h>

/*==================[macros and definitions]=================================*/

/** tamaño de pila para los threads */
#define STACK_SIZE 1024

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void * tarea1(void * param);
static void * tarea2(void * param);

/*==================[internal data definition]===============================*/

static queue_t q;

/*==================[external data definition]===============================*/

/* pilas de cada tarea */
uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];

const taskDefinition task_list[TASK_COUNT] = {
		{stack1, STACK_SIZE, tarea1, (void *)0xAAAAAAAA, TASK_PRIORITY_LOW},
		{stack2, STACK_SIZE, tarea2, (void *)0xBBBBBBBB, TASK_PRIORITY_MEDIUM},
};

/*==================[internal functions definition]==========================*/

static void * tarea1(void * param)
{
	queueItem_t item;
	char str[50];

	while (1) {
		item = queueGet(&q);
		sprintf(str, "recibido: %f\r\n", item);
		uartSend(str, strlen(str));
	}
	return NULL;
}

static void * tarea2(void * param)
{
	while (1) {
		delay(5000);
		queuePut(&q, 3.1416);
	}
	return NULL;
}

/*==================[external functions definition]==========================*/

int main(void)
{
	/* Inicialización del MCU */
	Board_Init();

	uartInit();

	ioInit();

	queueInit(&q);

	/* Inicio OS */
	start_os();

	/* no deberíamos volver acá */
	while(1);
}

/*==================[end of file]============================================*/
