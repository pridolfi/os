/*==================[inclusions]=============================================*/

#include "sched.h"
#include "board.h"
#include <string.h>

/*==================[macros and definitions]=================================*/

typedef int32_t (*entry_point_t)(void *);

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/* pilas de cada tarea */
uint32_t stack1[128];
uint32_t stack2[128];

/* punteros de pila de cada tarea */
uint32_t sp1;
uint32_t sp2;

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int32_t getNextContext(int32_t actualContext)
{
	static int actualTask;

	if (actualTask == 0) {
		actualTask = 1;
		return sp1;
	}
	else if(actualTask == 1) {
		actualTask = 2;
		sp1 = actualContext;
		return sp2;
	}
	else if (actualTask == 2) {
		actualTask = 1;
		sp2 = actualContext;
		return sp1;
	}

	/* si llegamos a este punto, error en la política de scheduling!!! */
	while(1);
}

void return_hook(void)
{
	/* por si una tarea ejecuta return, vengo acá */
	while(1);
}

/* task_create sirve para crear un contexto inicial */
void task_create(
		uint32_t * stack_frame, /* vector de pila (frame) */
		uint32_t stack_frame_size, /* el tamaño expresado en bytes */
		uint32_t * stack_pointer, /* donde guardar el puntero de pila */
		entry_point_t entry_point, /* punto de entrada de la tarea */
		void * parameter /* parametro de la tarea */)
{
	/* inicializo el frame en cero */
	bzero(stack_frame, stack_frame_size);

	/* último elemento del contexto inicial: xPSR
	 * necesita el bit 24 (T, modo Thumb) en 1
	 */
	stack_frame[stack_frame_size/4 - 1] = 1<<24;

	/* anteúltimo elemento: PC (entry point) */
	stack_frame[stack_frame_size/4 - 2] = (uint32_t)entry_point;

	/* penúltimo elemento: LR (return hook) */
	stack_frame[stack_frame_size/4 - 3] = (uint32_t)return_hook;

	/* elemento -8: R0 (parámetro) */
	stack_frame[stack_frame_size/4 - 8] = (uint32_t)parameter;

	stack_frame[stack_frame_size/4 - 9] = 0xFFFFFFF9;

	/* inicializo stack pointer inicial */
	*stack_pointer = (uint32_t)&(stack_frame[stack_frame_size/4 - 17]);
}

int32_t tarea1(void * param)
{
	int i;
	while (1) {
		Board_LED_Toggle(0);
		for (i=0; i<0x3FFFFF; i++);
	}
	return 0; /* a dónde va? */
}

int32_t tarea2(void * param)
{
	int j;
	while (1) {
		Board_LED_Toggle(2);
		for (j=0; j<0xFFFFF; j++);
	}
	return 1; /* a dónde va? */
}

int main(void)
{
	Board_Init();
	SystemCoreClockUpdate();

	task_create(stack1, 128*4, &sp1, tarea1, (void *)0xAAAAAAAA);
	task_create(stack2, 128*4, &sp2, tarea2, (void *)0xBBBBBBBB);

	SysTick_Config(SystemCoreClock / 1000);
	while(1);
}

/*==================[end of file]============================================*/
