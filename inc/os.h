#ifndef _OS_H_
#define _OS_H_

/*==================[inclusions]=============================================*/

#include "board.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** prototipo de las tareas o hilos de ejecucion o threads */
typedef void* (*entry_point_t)(void *);

/** estructura de control y estado de tareas */
typedef struct taskControlBlock {
	uint32_t sp;
	uint32_t * stack;
	uint32_t stack_size;
	entry_point_t entry_point;
	void  * parameter;
}taskControlBlock;

/*==================[external data declaration]==============================*/

extern taskControlBlock task_list[];
extern const size_t task_count;

/*==================[external functions declaration]=========================*/

int start_os(void);
void schedule(void);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_H_ */
