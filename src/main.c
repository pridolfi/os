/*==================[inclusions]=============================================*/

#include "os.h"
#include "board.h"

#include <stdlib.h>

/*==================[macros and definitions]=================================*/

/** tamaño de pila para los threads */
#define STACK_SIZE 512

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void * tarea1(void * param);
static void * tarea2(void * param);
static void * tarea3(void * param);
static void * tarea4(void * param);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/* pilas de cada tarea */
uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];
uint8_t stack3[STACK_SIZE];
uint8_t stack4[STACK_SIZE];

const taskDefinition task_list[TASK_COUNT] = {
		{stack1, STACK_SIZE, tarea1, (void *)0xAAAAAAAA, TASK_PRIORITY_LOW},
		{stack2, STACK_SIZE, tarea2, (void *)0xBBBBBBBB, TASK_PRIORITY_MEDIUM},
		{stack3, STACK_SIZE, tarea3, (void *)0xCCCCCCCC, TASK_PRIORITY_HIGH},
		{stack4, STACK_SIZE, tarea4, (void *)0xDDDDDDDD, TASK_PRIORITY_HIGH}
};

/*==================[internal functions definition]==========================*/

static void * tarea1(void * param)
{
	float j=4;
	while (j) {
		j *= 1.1;
		Board_LED_Toggle(0);
		delay(rand() & 0xFF);
	}
	return (void *)0; /* a dónde va? */
}

static void * tarea2(void * param)
{
	int j=4;
	while (j) {
		Board_LED_Toggle(3);
		delay(rand() & 0x1FF);
	}
	return (void *)4; /* a dónde va? */
}

static void * tarea3(void * param)
{
	float j=4;
	while (j+1) {
		j = 1 + 100 * rand();
		Board_LED_Toggle(4);
		delay((int)j & 0x7FF);
	}
	return (void *)0; /* a dónde va? */
}

static void * tarea4(void * param)
{
	int j=4;
	while (j) {
		Board_LED_Toggle(5);
		delay(rand() & 0x3FF);
	}
	return (void *)4; /* a dónde va? */
}

/*==================[external functions definition]==========================*/

int main(void)
{
	/* Inicialización del MCU */
	Board_Init();

	/* Inicio OS */
	start_os();

	/* no deberíamos volver acá */
	while(1);
}

/*==================[end of file]============================================*/
