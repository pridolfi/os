#ifndef _VECTOR_H_
#define _VECTOR_H_

/*==================[inclusions]=============================================*/

#include "board.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** handler de irq del usuario */
typedef void (*user_irq_handler_t)(void);

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

int32_t os_attach_irq(LPC43XX_IRQn_Type irq, user_irq_handler_t callback);
int32_t os_detach_irq(LPC43XX_IRQn_Type irq);

/*==================[end of file]============================================*/
#endif /* #ifndef _VECTOR_H_ */
