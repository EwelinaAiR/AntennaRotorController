#include "encoder_as5040.h"

/*void EncoderAS5040::Fsm(uint16_t data) {

	switch (fsmState) {

		default:
			SPI_CS() = 0;
		break;
	}
*/



//float_t EncoderAS5040::angleValue;

void EncoderAS5040::HardwareInit()
{

	// set CLK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	// GPIO: PB14 - MISO, PB13 - CLK, PB12 - nCS
	// set speed of GPIOs
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_1 | GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR15_1;

	// GPIOs remapping (based on manual STM32F407)
	GPIOB->AFR[1] &= ~((uint32_t) 0xF << ((14-8)*4) | (uint32_t) 0xF << ((13-8)*4));
	GPIOB->AFR[1] |= (uint32_t) 5 << ((14-8)*4) | (uint32_t) 5 << ((13-8)*4);

	// wybor trybu pracy linii portu
	GPIOB->MODER &= ~(GPIO_MODER_MODER14 |GPIO_MODER_MODER13 | GPIO_MODER_MODER12);
	GPIOB->MODER |= GPIO_MODER_MODER14_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER12_0;

	// ustawienia SPI - transmisja 16-bitowa z programowa kontrola przeplywu
	SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_BR_1 |SPI_CR1_CPOL| SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_DFF;
	SPI2->CR2 = 0x00;

	// wlaczenie urzadzenia
	SPI2->CR1 |= SPI_CR1_SPE;
}

