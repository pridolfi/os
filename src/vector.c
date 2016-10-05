/*==================[inclusions]=============================================*/

#include "board.h"
#include "os.h"

#include <string.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

static user_irq_handler_t user_irq_handlers[QEI_IRQn+1];

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void os_irq_handler(LPC43XX_IRQn_Type irq)
{
	/* TODO: check and save calling state */

	if (user_irq_handlers[irq] != NULL) {
		user_irq_handlers[irq]();
	}

	/* TODO: restore state and call scheduler if neccessary */
}

/*==================[external functions definition]==========================*/

int32_t os_attach_irq(LPC43XX_IRQn_Type irq, user_irq_handler_t callback)
{
	int32_t rv = -1;
	if (user_irq_handlers[irq] == NULL) {
		user_irq_handlers[irq] = callback;
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
		rv = 0;
	}
	return rv;
}

int32_t os_detach_irq(LPC43XX_IRQn_Type irq)
{
	int32_t rv = -1;
	if (user_irq_handlers[irq] != NULL) {
		user_irq_handlers[irq] = NULL;
		NVIC_ClearPendingIRQ(irq);
		NVIC_DisableIRQ(irq);
		rv = 0;
	}
	return rv;
}


/*==================[interrupt service routines]=============================*/

void DAC_IRQHandler(void){os_irq_handler(         DAC_IRQn         );}
void M0APP_IRQHandler(void){os_irq_handler(       M0APP_IRQn       );}
void DMA_IRQHandler(void){os_irq_handler(         DMA_IRQn         );}
void FLASH_EEPROM_IRQHandler(void){os_irq_handler(RESERVED1_IRQn   );}
void ETH_IRQHandler(void){os_irq_handler(         ETHERNET_IRQn    );}
void SDIO_IRQHandler(void){os_irq_handler(        SDIO_IRQn        );}
void LCD_IRQHandler(void){os_irq_handler(         LCD_IRQn         );}
void USB0_IRQHandler(void){os_irq_handler(        USB0_IRQn        );}
void USB1_IRQHandler(void){os_irq_handler(        USB1_IRQn        );}
void SCT_IRQHandler(void){os_irq_handler(         SCT_IRQn         );}
void RIT_IRQHandler(void){os_irq_handler(         RITIMER_IRQn     );}
void TIMER0_IRQHandler(void){os_irq_handler(      TIMER0_IRQn      );}
void TIMER1_IRQHandler(void){os_irq_handler(      TIMER1_IRQn      );}
void TIMER2_IRQHandler(void){os_irq_handler(      TIMER2_IRQn      );}
void TIMER3_IRQHandler(void){os_irq_handler(      TIMER3_IRQn      );}
void MCPWM_IRQHandler(void){os_irq_handler(       MCPWM_IRQn       );}
void ADC0_IRQHandler(void){os_irq_handler(        ADC0_IRQn        );}
void I2C0_IRQHandler(void){os_irq_handler(        I2C0_IRQn        );}
void SPI_IRQHandler(void){os_irq_handler(         I2C1_IRQn        );}
void I2C1_IRQHandler(void){os_irq_handler(        SPI_INT_IRQn     );}
void ADC1_IRQHandler(void){os_irq_handler(        ADC1_IRQn        );}
void SSP0_IRQHandler(void){os_irq_handler(        SSP0_IRQn        );}
void SSP1_IRQHandler(void){os_irq_handler(        SSP1_IRQn        );}
void UART0_IRQHandler(void){os_irq_handler(       USART0_IRQn      );}
void UART1_IRQHandler(void){os_irq_handler(       UART1_IRQn       );}
void UART2_IRQHandler(void){os_irq_handler(       USART2_IRQn      );}
void UART3_IRQHandler(void){os_irq_handler(       USART3_IRQn      );}
void I2S0_IRQHandler(void){os_irq_handler(        I2S0_IRQn        );}
void I2S1_IRQHandler(void){os_irq_handler(        I2S1_IRQn        );}
void SPIFI_IRQHandler(void){os_irq_handler(       RESERVED4_IRQn   );}
void SGPIO_IRQHandler(void){os_irq_handler(       SGPIO_INT_IRQn   );}
void GPIO0_IRQHandler(void){os_irq_handler(       PIN_INT0_IRQn    );}
void GPIO1_IRQHandler(void){os_irq_handler(       PIN_INT1_IRQn    );}
void GPIO2_IRQHandler(void){os_irq_handler(       PIN_INT2_IRQn    );}
void GPIO3_IRQHandler(void){os_irq_handler(       PIN_INT3_IRQn    );}
void GPIO4_IRQHandler(void){os_irq_handler(       PIN_INT4_IRQn    );}
void GPIO5_IRQHandler(void){os_irq_handler(       PIN_INT5_IRQn    );}
void GPIO6_IRQHandler(void){os_irq_handler(       PIN_INT6_IRQn    );}
void GPIO7_IRQHandler(void){os_irq_handler(       PIN_INT7_IRQn    );}
void GINT0_IRQHandler(void){os_irq_handler(       GINT0_IRQn       );}
void GINT1_IRQHandler(void){os_irq_handler(       GINT1_IRQn       );}
void EVRT_IRQHandler(void){os_irq_handler(        EVENTROUTER_IRQn );}
void CAN1_IRQHandler(void){os_irq_handler(        C_CAN1_IRQn      );}
void ADCHS_IRQHandler(void){os_irq_handler(       ADCHS_IRQn       );}
void ATIMER_IRQHandler(void){os_irq_handler(      ATIMER_IRQn      );}
void RTC_IRQHandler(void){os_irq_handler(         RTC_IRQn         );}
void WDT_IRQHandler(void){os_irq_handler(         WWDT_IRQn        );}
void M0SUB_IRQHandler(void){os_irq_handler(       M0SUB_IRQn       );}
void CAN0_IRQHandler(void){os_irq_handler(        C_CAN0_IRQn      );}
void QEI_IRQHandler(void){os_irq_handler(         QEI_IRQn         );}

/*==================[end of file]============================================*/
