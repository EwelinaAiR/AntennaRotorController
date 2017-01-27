#include "encoder_as5040.h"

float EncoderAS5040::angleValue;

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
	SPI2->CR1 = SPI_CR1_CPOL | SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPHA | SPI_CR1_DFF;
	SPI2->CR2 = 0x00;

	// wlaczenie urzadzenia
	SPI2->CR1 |= SPI_CR1_SPE;
}

void EncoderAS5040::EncoderUpdate()
{
	for (int i = 16; i >= 0; i--)
			  {
			    int k = data >> i;
			    if (k & 1)
			    	buff[16-i] = 1;
			    else
			    	buff[16-i] = 0;
			  }
			 bitOCF = buff[5];
			 bitCOF = buff[4];
			 bitLIN = buff[3];
			 bitINC = buff[2];
			 bitDEC = buff[1];
			 bitPAR = buff[0];
	switch(encoderState)
	{
		case STATE_STATUS:
				if(bitINC || bitDEC)
				{
					//komunikat o bledzie ustawienia magnesu
				}
				if(bitCOF)
				{
					//komunikat o bledzie wartosci odczytu
				}
				if(bitLIN)
				{
					//komunikat o MOZLIWYM bledzie odczytu
				}
				else {
					encoderState = STATE_POSITION;
				}
		case STATE_POSITION:
		{


		}
			/*for(int i = 16; i >= 6; i--)
			{
				anglebits += buff[i]*2^(i-6);
				encoderState = STATE_STATUS;
			}*/


	}
}
