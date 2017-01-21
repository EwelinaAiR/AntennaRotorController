#pragma once

#include "stm32f4xx.h"
#include "hdr_bitband.h"
#include <math.h>

class EncoderAS5040
{
	static uint8_t const ZYXDA_BIT = 0x08;

	// Definicje stanow automatu do obslugi akcelerometru
	enum FsmState
	{
		STATE_STATUS,
		STATE_ANGLE,
	};


	volatile uint16_t u16Data;
	uint16_t data;
	FsmState fsmState;

	static volatile unsigned long & SPI_CS()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOB->ODR, 12);
	}

	void HardwareInit();

public:
	 bool bitOCF;
	 bool bitCOF;
	 bool bitLIN;
	 bool bitINC;
	 bool bitDEC;
	 bool bitPAR;
	 bool buff[16];

	volatile bool isDataReady;
	volatile bool readIsOk;
	float angleValue;


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
		fsmState = STATE_STATUS;
		SPI_CS() = 0; __NOP(); __NOP();
		// rozkaz wysylany
		SPI2->DR = 0;
	}

	void Init()
	{
		HardwareInit();

		isDataReady = false;
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
		 for (int c = 16; c >= 0; c--)
		  {
		    int k = data >> c;
		    if (k & 1)
		    	buff[16-c] = 1;
		    else
		    	buff[16-c] = 0;
		  }
		 bitOCF = buff[5];
		 bitCOF = buff[4];
		 bitLIN = buff[3];
		 bitINC = buff[2];
		 bitDEC = buff[1];
		 bitPAR = buff[0];
	}

	void CalculateAngles()
	{
		data = data >> 6; //otrzymujemy liczbê 10 bitow¹
		angleValue = data * 0,35; // obiczamy k¹t
	}
	//EvenPAR = 1
	//MagINC = 0
	//MagDEC = 0
	//LIN = 0
	//COF = 0
	//OCF = 1
	//takie statusy pozwalaj¹ na poprawny odczyt?
	//100001 = 33(DEC)
	void CheckStatus(uint8_t status){
	status = data && 0x3F;
		if(status == 33){
			readIsOk = true;
		}
	}
};

