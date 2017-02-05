#pragma once

#include "stm32f4xx.h"

// GPIO: PE14 - TRIG_MOTOR, PE13 - RES_GEN, PE12 = DIR;

class Regulator
{
public:

	bool movingPlus;
	bool movingMinus;
	bool moving;
	volatile uint32_t wakeClock;



	void Init()
	{
		HardwareInit();
	}
	void HardwareInit()
	{
		// Set clocks
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

		//PE2, PE4, PE5 PULL DOWN
		//7GPIOE->PUPDR |= GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5;

		GPIOE->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR14 |GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR12) ;
		GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1 |GPIO_OSPEEDER_OSPEEDR13_1| GPIO_OSPEEDER_OSPEEDR12_1;

		GPIOE->MODER &= ~(GPIO_MODER_MODER14 | GPIO_MODER_MODER13 | GPIO_MODER_MODER12);
		GPIOE->MODER |= GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_0 |GPIO_MODER_MODER12_0;

		GPIOE->ODR |= GPIO_ODR_ODR_13;

	}
	void SetGen()
	{
		GPIOE->ODR |= GPIO_ODR_ODR_13;

	}
	void RstGen()
	{
		GPIOE->ODR &= ~GPIO_ODR_ODR_13;

	}
	void SetDir()
	{
		GPIOE->ODR|= GPIO_ODR_ODR_12;
		movingPlus = true;
		movingMinus = false;
	}
	void RstDir()
	{
		GPIOE->ODR&= ~GPIO_ODR_ODR_12;
		movingPlus = false;
		movingMinus = true;
	}
	void SetTrigMotor()
	{
		GPIOE->ODR|= GPIO_ODR_ODR_14;
	}
	void RstTrigMotor()
	{
		GPIOE->ODR&= ~GPIO_ODR_ODR_14;
	}



};
