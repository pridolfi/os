#ifndef _OS_H_
#define _OS_H_

/*==================[inclusions]=============================================*/

#include "board.h"
#include "os_config.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** prototipo de las tareas o hilos de ejecucion o threads */
typedef void* (*entry_point_t)(void *);

/** posibles estados de una tarea */
typedef enum taskState {
	TASK_STATE_ERROR,
	TASK_STATE_READY,
	TASK_STATE_RUNNING,
	TASK_STATE_WAITING,
	TASK_STATE_ZOMBIE,
	TASK_STATE_TERMINATED
}taskState;

/** estructura de definicón de tareas */
typedef struct taskDefinition {
	uint8_t * stack;
	uint32_t stack_size;
	entry_point_t entry_point;
	void  * parameter;
}taskDefinition;

/*==================[external data declaration]==============================*/

extern const taskDefinition task_list[TASK_COUNT];

/*==================[external functions declaration]=========================*/

int start_os(void);
void schedule(void);
void delay(uint32_t milliseconds);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_H_ */
