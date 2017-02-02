#pragma once

#include <string.h>
#include "stm32f4xx.h"
#include "uart_communication_interface.h"
#include "encoder_as5040.h"
#include "led_interface.h"
#include "filter2_iir.h"
#include "analog_outputs.h"
#include "regulator.h"

#include "data_recorder.h"


#define CPU_CLK	((uint32_t)168000000)

class App
{

	DataRecorder < 2048, float > rec1;
	//DataRecorder < 512, uint16_t > rec2;

	float filterParams1[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

	Filter2Iir filter1;

	// cykle w [ms]
	volatile uint32_t mainClock;
	volatile uint32_t auxClock;
	volatile uint32_t wakeClock;
	volatile uint32_t testClock;

	uint16_t dacOutVal;

public:

	AnalogOutputs analogOuts;
	EncoderAS5040 enc;
	UartCommunicationInterface com;
	Regulator regulator;

	bool test;
	struct TxFrame
	{
		uint16_t rawData;
		uint8_t stateRaport;
	} toSend;


	bool tick;

	App(): filter1(filterParams1)
	{
		mainClock = 0;
		auxClock = 0;
		dacOutVal = 0;
		wakeClock = 0;
		testClock = 0;
	};

	void GeneralHardwareInit()
	{
		// inicjalizacja mikrokontrolera
		SystemInit();


		// ustawienie zegara systemowego w programie
		if (SysTick_Config(CPU_CLK/1000))
		{
			while (1);
		}

		NVIC_EnableIRQ(DMA2_Stream7_IRQn);
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_EnableIRQ(SPI2_IRQn);
	}

	void Init()
	{
		GeneralHardwareInit();
		enc.Init();
	    com.Init();
		analogOuts.Init();
		regulator.Init();
		tick = false;

	}

	void PeriodicUpdate()
	{
		tick = true;
		mainClock++;
		auxClock++;
		wakeClock++;
		testClock++;

		com.PeriodicUpdate();
		enc.WriteReadStart();
		analogOuts.SetOutput1((dacOutVal)>>4);
		analogOuts.SetOutput2((dacOutVal)>>4);

		if (testClock <= 5000)
		{

		  if (wakeClock == 20){
				  		regulator.SetTrigMotor();
				  }else if(wakeClock == 30){
				  		regulator.RstTrigMotor();
				  		wakeClock = 0;
				  		}
		}
		else
		{
			if (testClock > 10000)
			{
				testClock = 0;
				wakeClock = 0;
			}
		}
		dacOutVal += 256;
		if (auxClock == 500)
		{
		  Led::Green()^= 1;
		  /*if(test == true)
		  {
			  regulator.SetDir();
			  test = false;
		  }
		  else
		  {
			  regulator.RstDir();
			  test = true;
		  }*/

		  com.Send(2);
		  auxClock = 0;
		}

	}
	void PrepareToSend()
	{

		toSend.rawData = enc.data;
		toSend.stateRaport = enc.statusReport;
	}

	void Run()
	{
		while(1)
		{

			if(com.isFrameReceived)
			{
				if(com.CheckFrame())
				{
					// przygotowanie danych do wyslania
					memcpy(com.txData, &toSend, sizeof(toSend));
					com.Send(sizeof(toSend));
				}
				com.isFrameReceived = false;
			}
/*
			if(enc.isDataReady)
			{
				enc.isDataReady = false;
			}*/

			if (tick)
			{

			}
		}
	}

};

extern App * pApp;
