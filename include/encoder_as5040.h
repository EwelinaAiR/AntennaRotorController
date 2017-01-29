#pragma once

#include "stm32f4xx.h"
#include "hdr_bitband.h"
#include <math.h>


class EncoderAS5040
{
	static uint8_t const ZYXDA_BIT = 0x08;

	static float stateAngle =  0.3515625;

	// Definicje stanow automatu do obslugi akcelerometru
	enum EncoderState
	{
		STATE_STATUS,
		STATE_ANGLE
	};

	volatile uint16_t u16Data;

	EncoderState encoderState;

	static volatile unsigned long & SPI_CS()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOB->ODR, 12);
	}

	void HardwareInit();

public:
	 const int stat_S1 = (1 << 15);
	 const int stat_S2 = (1 << 14);
	 const int stat_S3 = (1 << 13);
	 const int stat_S4 = (1 << 12);
	 const int stat_S5 = (1 << 11);
	 const int stat_S6 = (1 << 10);

	 bool error;

	 static float angleValue;
	 uint16_t data;

	volatile bool isDataReady;

	// Wymiana danych na SPI z blokowaniem
	uint16_t WriteReadBlock(uint16_t data)
	{
		SPI_CS() = 0;
		__NOP(); __NOP();
		while(!(SPI2->SR & SPI_SR_TXE)) {};
		SPI2->DR = data;
	    while(SPI2->SR & SPI_SR_BSY) {};
	    while(!(SPI2->SR & SPI_SR_RXNE)) {};

	    SPI_CS() = 1;
	    __NOP(); __NOP();
	    data = SPI2->DR;
	    return data;
	}

	void WriteReadStart()
	{
		SPI_CS() = 0; __NOP(); __NOP();
		// rozkaz wysylany
		SPI2->DR = 0;
	}

	void Init()
	{
		HardwareInit();

		isDataReady = false;
		//encoderState = STATE_IDLE;
		u16Data = SPI2->DR;
		// ustawienie akcelerometru
		// zezwolenie na obsluge przerwan od odbiornika SPI
		SPI2->CR2 |= SPI_CR2_RXNEIE;
	}

	void Irq()
	{

		SPI_CS() = 1;
		data = (SPI2->DR);

	}


	void ScaleData()
	{
		if ((data & stat_S1) == stat_S1){
			error = false;
		}
		if ((data & stat_S2) == stat_S2){
			error = false;
		}
		if ((data & stat_S3) == stat_S3){
			error = false;
		}
		if ((data & stat_S4) == stat_S4){
			error = true;
		}
		if ((data & stat_S5) == stat_S5){
			error = true;
		}
		if ((data & stat_S6) == stat_S6){
			error = false;
		}
	}

	void CalculateAngles()
		{
			data = data >> 6; //data D0:D9
			angleValue = data * stateAngle; //calculate angle
			angleValue = STATE_ANGLE;
		}

};

