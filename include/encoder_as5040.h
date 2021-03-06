#pragma once

#include "stm32f4xx.h"
#include "hdr_bitband.h"
#include <math.h>


class EncoderAS5040
{
	static uint8_t const ZYXDA_BIT = 0x08;


	volatile uint16_t u16Data;

	uint8_t status;

	static volatile unsigned long & SPI_CS()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOB->ODR, 12);
	}

	void HardwareInit();

public:
	 const int stat_S1 = (1 << 15); //bit PAR
	 const int stat_S2 = (1 << 14); //bit DEC
	 const int stat_S3 = (1 << 13); //bit INC
	 const int stat_S4 = (1 << 12); //bit LIN
	 const int stat_S5 = (1 << 11); //bit COF
	 const int stat_S6 = (1 << 10); //bit OCF

	 //float_t angleStep =  0.3515625f;
	 //double_t Step = 0.3515625;

	 uint8_t statusReport;
	 bool error;

	 float_t angleValue;
	 uint16_t value;
	 volatile uint16_t data;

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


		data = SPI2->DR & 0x7fff;
		CalculateAngles();
		SPI_CS() = 1;
	}


	void ScaleData()
	{

	}

	void CheckState()
	{
				if ((data & stat_S2) == stat_S2 || (data & stat_S3) == stat_S3){ //b��d odleg�osci magnesu
					error = true;
					statusReport = 1;
				}
				else if ((data & stat_S4) == stat_S4){ //mozliwy nieporawidlowy odczyt z enkodera - krytyczna liniowos� wyjscia
					error = true;
					statusReport = 2;
				}
				else if ((data & stat_S5) == stat_S5){ //blad zakresu odczytu enkodera
					error = true;
					statusReport = 3;
				}
				else if ((data & stat_S6) == stat_S6){ //zakonczono algorytm kompensacji, dane poprawne
					error = false;
					statusReport = 4;
				}
				else {									//wszystko OK
					error = false;
					statusReport = 0;
				}
	}

	float_t CalculateAngles()
		{

			float_t angleStep =  0.3515625f;
			status = data & 0xF;
			value = data;
			value = value >> 5; //data D0:D9
			angleValue = (float)value*angleStep; //calculate angle
			return angleValue;
		}

};

