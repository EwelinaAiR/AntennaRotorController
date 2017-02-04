#pragma once

#include "stm32f4xx.h"

// GPIO: PA4 - DAC OUT1, PA5 - DAC OUT2

class AnalogOutputs
{
public:
	void Init()
	{
		HardwareInit();
	}
	void HardwareInit()
	{
		// Set clocks
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		RCC->APB1ENR |= RCC_APB1ENR_DACEN;

		// Analog mode, no pull up/down
		GPIOA->MODER |= GPIO_MODER_MODER4 | GPIO_MODER_MODER5;
		GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5);

		// Set DAC channels
		DAC->DHR12R1 = 0;
		DAC->DHR12R2 = 0;
		DAC->CR = DAC_CR_EN1 | DAC_CR_BOFF1 | DAC_CR_EN2 | DAC_CR_BOFF2;
	}

	void SetOutput1(uint16_t data)
	{
		DAC->DHR12R1 = data;
	}

	void SetOutput2(uint16_t data)
	{
		DAC->DHR12R2 = data;
	}

};
