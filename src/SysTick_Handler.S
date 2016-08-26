	.syntax unified
	.global SysTick_Handler

	/* declaro variables globales visibles desde este módulo */
	.extern sp1, sp2, getNextContext

	.text
	.thumb_func
SysTick_Handler:

	/* cargo contexto de la primer tarea en MSP */
	//ldr r1,=sp1  /* r1 = &sp1 */
	//ldr r0,[r1]  /* r0 = *r1 */
	//msr msp,r0    /* msp = r0 */

	/* usamos getNextContext */
	push {r4-r11,lr}

	mrs r0, msp /* r0 = msp */
	bl getNextContext /* r0 = siguiente puntero de pila
	uso la instrucción bl (branch with link) */

	msr msp, r0 /* msp = r0 */

	pop {r4-r11,lr}

	bx lr /* retorno de interrupción,
debería recuperar el contexto armado en task_create */
