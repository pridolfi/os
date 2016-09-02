/*==================[inclusions]=============================================*/

#include "os.h"
#include "board.h"

/*==================[macros and definitions]=================================*/

/** tamaño de pila para los threads */
#define STACK_SIZE 512

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void * tarea1(void * param);
static void * tarea2(void * param);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/* pilas de cada tarea */
uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];

const taskDefinition task_list[TASK_COUNT] = {
		{stack1, STACK_SIZE, tarea1, (void *)0xAAAAAAAA},
		{stack2, STACK_SIZE, tarea2, (void *)0xBBBBBBBB}
};

/*==================[internal functions definition]==========================*/

static void * tarea1(void * param)
{
	while (1) {
		Board_LED_Toggle(0);
		delay(500);
	}
	return (void *)0; /* a dónde va? */
}

static void * tarea2(void * param)
{
	int j=4;
	while (j) {
		Board_LED_Toggle(3);
		for(j=0; j<0xFFFFF; j++);
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
