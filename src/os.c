/*==================[inclusions]=============================================*/

#include "board.h"
#include "os.h"

#include <string.h>

/*==================[macros and definitions]=================================*/

/** valor de retorno de excepción a cargar en el LR */
#define EXC_RETURN 0xFFFFFFF9

/** id de tarea idle */
#define IDLE_TASK (TASK_COUNT)

/** tamaño de stack de tarea idle */
#define STACK_IDLE_SIZE 256

/** estructura interna de control de tareas */
typedef struct taskControlBlock {
	uint32_t sp;
	const taskDefinition * tdef;
	taskState state;
	uint32_t waiting_time;
}taskControlBlock;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

__attribute__ ((weak)) void * idle_hook(void * p);

/*==================[internal data definition]===============================*/

/** indice a la tarea actual */
static uint32_t current_task = INVALID_TASK;

/** estructura interna de control de tareas */
static taskControlBlock task_control_list[TASK_COUNT];

/** contexto de la tarea idle */
uint8_t stack_idle[STACK_IDLE_SIZE];
static taskDefinition idle_tdef = {
		stack_idle, STACK_IDLE_SIZE, idle_hook, 0
};
static taskControlBlock idle_task_control;

/** lista de tareas READY por prioridad, TASK_COUNT tareas por cada nivel */
static uint32_t ready_list[TASK_PRIORITY_HIGH][TASK_COUNT];
static uint32_t ready_count[TASK_PRIORITY_HIGH];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void add_ready(taskPriority prio, uint32_t id)
{
	ready_list[prio-1][ready_count[prio-1]] = id;
	ready_count[prio-1]++;
}

static void remove_ready(taskPriority prio, uint32_t id)
{
	uint32_t i;
	for (i=0; i<ready_count[prio-1]; i++) {
		if (ready_list[prio-1][i] == id) {
			break;
		}
	}
	if (i < ready_count[prio-1]) {
		uint32_t j;
		for (j=i; j<(ready_count[prio-1]-1); j++) {
			ready_list[prio-1][j] = ready_list[prio-1][j+1];
		}
		ready_count[prio-1]--;
	}
}

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

void task_delay_update(void)
{
	uint32_t i;
	for (i=0; i<TASK_COUNT; i++) {
		if ( (task_control_list[i].state == TASK_STATE_WAITING) &&
				(task_control_list[i].waiting_time > 0)) {
			task_control_list[i].waiting_time--;
			if (task_control_list[i].waiting_time == 0) {
				task_control_list[i].state = TASK_STATE_READY;
				add_ready(task_control_list[i].tdef->prio, i);
			}
		}
	}
}

/*==================[external functions definition]==========================*/

uint32_t os_get_running_task(void)
{
	return current_task;
}

void delay(uint32_t milliseconds)
{
	if ((milliseconds != 0) && (current_task != INVALID_TASK)) {
		task_control_list[current_task].state = TASK_STATE_WAITING;
		task_control_list[current_task].waiting_time = milliseconds;
		schedule();
	}
}

/* rutina de selección de próximo contexto a ejecutarse
 * acá definimos la política de scheduling de nuestro os
 */
int32_t getNextContext(int32_t current_context)
{
	uint32_t returned_stack;

	/* guardo contexto actual si es necesario */
	if (current_task == IDLE_TASK) {
		idle_task_control.sp = current_context;
		idle_task_control.state = TASK_STATE_READY;
	}
	else if (current_task < TASK_COUNT) {
		task_control_list[current_task].sp = current_context;
		if (task_control_list[current_task].state == TASK_STATE_RUNNING) {
			task_control_list[current_task].state = TASK_STATE_READY;
			add_ready(task_list[current_task].prio, current_task);
		}
	}
	/* decido cuál va a ser el contexto siguiente a ejecutar */
	taskPriority p;
	for (p = TASK_PRIORITY_HIGH; p > TASK_PRIORITY_IDLE; p--) {
		if (ready_count[p-1] > 0) {
			current_task = ready_list[p-1][0];
			remove_ready(p, current_task);
			task_control_list[current_task].state = TASK_STATE_RUNNING;
			returned_stack = task_control_list[current_task].sp;
			break;
		}
	}
	if (p == TASK_PRIORITY_IDLE) {
		current_task = IDLE_TASK;
		idle_task_control.state = TASK_STATE_RUNNING;
		returned_stack = idle_task_control.sp;
	}
	return returned_stack;
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
	task_delay_update();
	schedule();
}

int start_os(void)
{
	uint32_t i;

	/* actualizo SystemCoreClock (CMSIS) */
	SystemCoreClockUpdate();

	/* inicializo contexto idle */
	idle_task_control.tdef = &idle_tdef;
	task_create(idle_task_control.tdef->stack,
			idle_task_control.tdef->stack_size,
			&(idle_task_control.sp),
			idle_task_control.tdef->entry_point,
			idle_task_control.tdef->parameter,
			&(idle_task_control.state));

	/* inicializo contextos iniciales de cada tarea */
	for (i=0; i<TASK_COUNT; i++) {
		task_control_list[i].tdef = task_list+i;

		task_create(task_control_list[i].tdef->stack,
				task_control_list[i].tdef->stack_size,
				&(task_control_list[i].sp),
				task_control_list[i].tdef->entry_point,
				task_control_list[i].tdef->parameter,
				&(task_control_list[i].state));

		add_ready(task_list[i].prio, i);
	}

	/* configuro PendSV con la prioridad más baja */
	NVIC_SetPriority(PendSV_IRQn, 255);
	SysTick_Config(SystemCoreClock / 1000);

	/* llamo al scheduler */
	schedule();

	return -1;
}

void * idle_hook(void * p)
{
	while (1) {
		__WFI();
	}
}

/*==================[end of file]============================================*/
