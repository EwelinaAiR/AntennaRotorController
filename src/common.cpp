#include "common.h"

extern "C"
{
	void SysTick_Handler(void);
	void EXTI0_IRQHandler(void);
	void SPI2_IRQHandler(void);
	void DMA2_Stream7_IRQHandler(void);
	void USART1_IRQHandler(void);
}

void Delay(volatile uint32_t count)
{
	while(count--) {};
}


void EXTI0_IRQHandler(void) {

}

void SPI2_IRQHandler(void) {
	pApp->enc.Irq();
}

// Obsluga przerwania od nadajnika UART poprzez DMA
void DMA2_Stream7_IRQHandler()
{
	pApp->com.IrqDma();
}

// Obsluga przerwania od odbiornika/nadajnika USART2
void USART1_IRQHandler() {
	pApp->com.IrqTx();
}

void SysTick_Handler(void) {
	pApp->PeriodicUpdate();
}


