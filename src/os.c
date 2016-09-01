/*==================[inclusions]=============================================*/

#include "board.h"
#include "os.h"

#include <string.h>

/*==================[macros and definitions]=================================*/

/** valor de retorno de excepción a cargar en el LR */
#define EXC_RETURN 0xFFFFFFF9

/** id de tarea inválida */
#define INVALID_TASK ((uint32_t)-1)

/** estructura interna de control de tareas */
typedef struct taskControlBlock {
	uint32_t sp;
	const taskDefinition * tdef;
	taskState state;
}taskControlBlock;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/** indice a la tarea actual */
static uint32_t current_task = INVALID_TASK;

/** estructura interna de control de tareas */
static taskControlBlock task_control_list[TASK_COUNT];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/* si una tarea ejecuta return, vengo acá */
static void return_hook(void * returnValue)
{
	while(1);
}

/* task_create sirve para crear un contexto inicial */
static void task_create(
		uint8_t * stack_frame, /* vector de pila (frame) */
		uint32_t stack_frame_size, /* el tamaño expresado en bytes */
		uint32_t * stack_pointer, /* donde guardar el puntero de pila */
		entry_point_t entry_point, /* punto de entrada de la tarea */
		void * parameter, /* parametro de la tarea */
		taskState * state)
{
	uint32_t * stack = (uint32_t *)stack_frame;

	/* inicializo el frame en cero */
	bzero(stack, stack_frame_size);

	/* último elemento del contexto inicial: xPSR
	 * necesita el bit 24 (T, modo Thumb) en 1
	 */
	stack[stack_frame_size/4 - 1] = 1<<24;

	/* anteúltimo elemento: PC (entry point) */
	stack[stack_frame_size/4 - 2] = (uint32_t)entry_point;

	/* penúltimo elemento: LR (return hook) */
	stack[stack_frame_size/4 - 3] = (uint32_t)return_hook;

	/* elemento -8: R0 (parámetro) */
	stack[stack_frame_size/4 - 8] = (uint32_t)parameter;

	stack[stack_frame_size/4 - 9] = EXC_RETURN;

	/* inicializo stack pointer inicial */
	*stack_pointer = (uint32_t)&(stack[stack_frame_size/4 - 17]);

	/* seteo estado inicial READY */
	*state = TASK_STATE_READY;
}

/*==================[external functions definition]==========================*/

/* rutina de selección de próximo contexto a ejecutarse
 * acá definimos la política de scheduling de nuestro os
 */
int32_t getNextContext(int32_t current_context)
{
	if (current_task == INVALID_TASK) {
		current_task = 0;
	}
	else if(current_task == 0) {
		task_control_list[current_task].sp = current_context;
		current_task = 1;
	}
	else if (current_task == 1) {
		task_control_list[current_task].sp = current_context;
		current_task = 0;
	}
	else {
		while(1); /* error!!! */
	}
	return task_control_list[current_task].sp;
}

void schedule(void)
{
	/* activo PendSV para llevar a cabo el cambio de contexto */
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	/* Instruction Synchronization Barrier: aseguramos que se
	 * ejecuten todas las instrucciones en  el pipeline
	 */
	__ISB();
	/* Data Synchronization Barrier: aseguramos que se
	 * completen todos los accesos a memoria
	 */
	__DSB();
}

void SysTick_Handler(void)
{
	schedule();
}

int start_os(void)
{
	uint32_t i;

	/* actualizo SystemCoreClock (CMSIS) */
	SystemCoreClockUpdate();

	/* inicializo contextos iniciales de cada tarea */
	for (i=0; i<TASK_COUNT; i++) {
		task_control_list[i].tdef = task_list+i;

		task_create(task_control_list[i].tdef->stack,
				task_control_list[i].tdef->stack_size,
				&(task_control_list[i].sp),
				task_control_list[i].tdef->entry_point,
				task_control_list[i].tdef->parameter,
				&(task_control_list[i].state));
	}

	/* configuro PendSV con la prioridad más baja */
	NVIC_SetPriority(PendSV_IRQn, 255);
	SysTick_Config(SystemCoreClock / 1000);

	/* llamo al scheduler */
	schedule();

	return -1;
}

/*==================[end of file]============================================*/
