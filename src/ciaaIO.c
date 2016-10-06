/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ciaaIO.h"
#include "os.h"
#include "queue.h"

extern queue_t q;

static ciaaPin_t inputs[4] = { {0,4},{0,8},{0,9},{1,9} };
static ciaaPin_t outputs[6] = { {5,0},{5,1},{5,2},{0,14},{1,11},{1,12} };

void irq_tec1(void)
{
	queuePut(&q, 1.1);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
}

void irq_tec2(void)
{
	queuePut(&q, 2.2);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
}

void irq_tec3(void)
{
	queuePut(&q, 3.3);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
}

void irq_tec4(void)
{
	queuePut(&q, 4.4);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
}

void ioInit(void)
{
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* LEDs */
   Chip_SCU_PinMux(2,0,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[0], LED0R */
   Chip_SCU_PinMux(2,1,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[1], LED0G */
   Chip_SCU_PinMux(2,2,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[2], LED0B */
   Chip_SCU_PinMux(2,10,MD_PUP|MD_EZI,FUNC0); /* GPIO0[14], LED1 */
   Chip_SCU_PinMux(2,11,MD_PUP|MD_EZI,FUNC0); /* GPIO1[11], LED2 */
   Chip_SCU_PinMux(2,12,MD_PUP|MD_EZI,FUNC0); /* GPIO1[12], LED3 */

   Chip_GPIO_SetDir(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<14),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<11)|(1<<12),1);

   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<14));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 1,(1<<11)|(1<<12));

   /* Switches */
   Chip_SCU_PinMux(1,0,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], SW1 */
   Chip_SCU_PinMux(1,1,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[8], SW2 */
   Chip_SCU_PinMux(1,2,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[9], SW3 */
   Chip_SCU_PinMux(1,6,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO1[9], SW4 */

   Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<4)|(1<<8)|(1<<9),0);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<9),0);

   /* IRQs */
   Chip_PININT_Init(LPC_GPIO_PIN_INT);
   Chip_SCU_GPIOIntPinSel(0, 0, 4);
   Chip_SCU_GPIOIntPinSel(1, 0, 8);
   Chip_SCU_GPIOIntPinSel(2, 0, 9);
   Chip_SCU_GPIOIntPinSel(3, 1, 9);
   Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0 | PININTCH1 | PININTCH2 | PININTCH3);
   Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0 | PININTCH1 | PININTCH2 | PININTCH3);

   os_attach_irq(PIN_INT0_IRQn, irq_tec1);
   os_attach_irq(PIN_INT1_IRQn, irq_tec2);
   os_attach_irq(PIN_INT2_IRQn, irq_tec3);
   os_attach_irq(PIN_INT3_IRQn, irq_tec4);
}

uint32_t ioReadInput(uint32_t inputNumber)
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,
			inputs[inputNumber].port,
			inputs[inputNumber].bit);
}

uint32_t ioWriteOutput(uint32_t outputNumber, uint32_t value)
{
	Chip_GPIO_SetPinState(LPC_GPIO_PORT,
			outputs[outputNumber].port,
			outputs[outputNumber].bit,
			value);
	return 0;
}

void ioToggleOutput(uint32_t outputNumber)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT,
			outputs[outputNumber].port,
			outputs[outputNumber].bit);
}
