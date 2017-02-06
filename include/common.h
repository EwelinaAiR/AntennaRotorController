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

	int16_t dacOutVal;
	uint16_t offset;
	uint16_t keyPressed;
	uint16_t setValue;
	int16_t error;
	int16_t error2;
	uint16_t dV;
	uint16_t ADCmin;
	uint16_t ADCmax;
	int16_t calcAngleValue;
	int16_t calcSetValue;

public:

	AnalogOutputs analogOuts;
	EncoderAS5040 enc;
	UartCommunicationInterface com;
	Regulator regulator;
	CmdMaster cmdM;

	bool test;
	bool tick;
	//------------------
	bool needToMove;
	bool dirPlus;
	bool dirMinus;


	App()
	{
		mainClock = 0;
		auxClock = 0;
		dacOutVal = 0;
		wakeClock = 0;
		testClock = 0;
		keyPressed = 0;
		setValue = 0;
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
		offset = 3;
		dV = 100;
		ADCmin = 0;
		ADCmax = 4095;
		dacOutVal = 0;

	}

	//-------------------------------------------------------------------------------------------------
	//-------------------------Temporal Workspace------------------------------------------------------
	//-------------------------------------------------------------------------------------------------


	void Move()
	{
		if (wakeClock == 20)
		{
			regulator.SetTrigMotor();
		}
		else if(wakeClock >= 30)
		{
			regulator.RstTrigMotor();
			wakeClock = 0;
		}
	}
	void ComparePos()
	{
		//calcAngleValue = enc.angleValue - 180;
		//calcSetValue = setValue - 180;
		error2 = setValue - enc.angleValue;
if(setValue > enc.angleValue)
{
	error = setValue - enc.angleValue;
}
else if (setValue < enc.angleValue)
{
	error = enc.angleValue - setValue;
}


		if(error < offset )
		{


			if (dacOutVal <= 0)
			{
				dacOutVal = ADCmin;
				dirMinus = false;
				dirPlus = false;
			}
			else
			{
				dacOutVal-=dV;
				Move();
			}
		}
		else
		{
			if(error2 > 0)
			{
				if(dacOutVal >= 4095)
				{
					dacOutVal = ADCmax;

				}
				else
				{
					dacOutVal+=dV;
				}

				dirPlus = true;
				MoveRotorAuto();
			}
			if(error2 < 0)
			{
				if(dacOutVal >= 4095)
				{
					dacOutVal = ADCmax;

				}
				else
				{
					dacOutVal+=dV;
				}
				dirMinus = true;
				MoveRotorAuto();
			}
		}

	}

	void ChooseMode()
	{
		switch(cmdM.cmd)
		{
		case 1:
			MoveRotorManual();

		break;

		case 2:
			ComparePos();
			MoveRotorAuto();

		break;

		default:
		break;

		}
	}
	void MoveRotorManual()
	{
		switch (keyPressed)
		{
			case 1:
				dacOutVal = 4095;
				regulator.SetDir();
				Move();

			break;
			case 2:
				dacOutVal = 4095;
				regulator.RstDir();
				Move();

			break;
		default:
		break;
		}
	}
	void MoveRotorAuto()
	{
			if(dirPlus && ~dirMinus)
			{
				regulator.SetDir();
				Move();
			}
			else if(dirMinus && ~dirPlus)
			{
				regulator.RstDir();
				Move();
			}
			else if(dirMinus && dirPlus)
			{

			}


	}
	//----------------------------------------------------------------------------------------


	void PeriodicUpdate()
	{
		tick = true;
		mainClock++;
		auxClock++;
		wakeClock++;
		testClock++;

		com.PeriodicUpdate();
		enc.WriteReadStart();
		analogOuts.SetOutput1((dacOutVal));
		analogOuts.SetOutput2((dacOutVal));
		regulator.SetGen();
		ComparePos();
		MoveRotorAuto();
		if (auxClock == 500)
		{
		  Led::Green()^= 1;
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

					// now copy from cmdM to specific user data

					// check what kind of order you have got from Master
					switch (cmdM.cmd)
					{
						case 1: // set desired angle
							setValue = cmdM.desiredAngle;
							// copy cmdM.desiredAngle to set point value for the regulator
							cmdS.encoderRawData = enc.data;
							cmdS.status = enc.statusReport;
							com.SendUserData(&cmdS, sizeof(CmdSlave));

						break;

						case 2:	// manual mode
							keyPressed = cmdM.remoteControl;// copy data from cmdM.remoteControl to output of the regulator (open-loop mode)
							cmdS.encoderRawData = enc.data;
							cmdS.status = enc.statusReport;
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
