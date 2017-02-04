#pragma once

#include "stm32f4xx.h"
#include "uart_communication_interface.h"
#include "encoder_as5040.h"
#include "led_interface.h"
#include "analog_outputs.h"
#include "regulator.h"
#include "communication_frames.h"

#include "data_recorder.h"

#define CPU_CLK	((uint32_t)168000000)

class App
{

	DataRecorder < 2048, float > rec1;
	//DataRecorder < 512, uint16_t > rec2;

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
	bool tick;

	App()
	{
		mainClock = 0;
		auxClock = 0;
		dacOutVal = 3000;
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
		dacOutVal = 64000;
		analogOuts.SetOutput1((dacOutVal)>>4);
		analogOuts.SetOutput2((dacOutVal)>>4);

		regulator.SetGen();
		if (testClock <= 2000)
		{

		  if (wakeClock == 20){
				  		regulator.SetTrigMotor();
				  }else if(wakeClock == 30){
				  		regulator.RstTrigMotor();
				  		wakeClock = 0;
				  		}

		}
			if (testClock > 4000)
			{
				regulator.RstGen();
				testClock = 0;
				wakeClock = 0;
			}
			dacOutVal = 4000;
		//dacOutVal += 0;
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


		  auxClock = 0;
		}

	}

	void Run()
	{
		while(1)
		{

			if(com.isFrameReceived)
			{
				if(com.CheckFrame())
				{
					static CmdMaster cmdM;
					static CmdSlave cmdS;

					com.GetUserData(&cmdM, sizeof(CmdMaster));

					// now copy from smdM to specific user data

					// check what kind of order you have got from Master
					switch (cmdM.cmd)
					{
						case 1: // set desired angle
							// copy cmdM.desiredAngle to set point value for the regulator
							cmdS.encoderRawData = 200; //enc.data;
							cmdS.status = enc.statusReport;
							com.SendUserData(&cmdS, sizeof(CmdSlave));

						break;

						case 2:	// manual mode
							// copy data from cmdM.manulaMode to output of the regulator (open-loop mode)
							cmdS.encoderRawData = 200; //enc.data;
							cmdS.encoderRawData = enc.statusReport;
							com.SendUserData(&cmdS, sizeof(CmdSlave));


						break;
						// etc. - you can define more orders!

						default:
						break;
					}



				}
				com.isFrameReceived = false;
			}
			if (tick)
			{

			}
		}
	}

};

extern App * pApp;
