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
	uint16_t offset;
	uint16_t keyPressed;

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
		offset = 3;
		keyPressed = 0;
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

	//-------------------------------------------------------------------------------------------------
	//-------------------------Temporal Workspace------------------------------------------------------
	//-------------------------------------------------------------------------------------------------


	void Move()
	{
		if (wakeClock == 20)
		{
			regulator.SetTrigMotor();
		}
		else if(wakeClock == 30)
		{
			regulator.RstTrigMotor();
			wakeClock = 0;
		}
	}
	void ComparePos()
	{
		if(enc.angleValue < cmdM.desiredAngle-offset)
		{
			needToMove = true;
			dirPlus = true;
		}
		else if(enc.angleValue > cmdM.desiredAngle+offset)
		{
			needToMove = true;
			dirMinus = true;
		}
		else
		{
		needToMove = false;
		dirMinus = false;
		dirPlus = false;
		}
	}
	void Accelerate()
	{
		if(dacOutVal<64000)
		{
			dacOutVal+=50;
		}
	}
	void Decelerate()
	{
		if(dacOutVal>0)
		{
			dacOutVal-=50;
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
			dacOutVal = 64000;
			regulator.SetDir();
			Move();

		break;

		case 2:
			dacOutVal = 64000;
			regulator.RstDir();
			Move();

		break;

		default:
		break;
		}

	}
	void MoveRotorAuto()
	{
		if(needToMove)
		{
			if(dirPlus && ~dirMinus)
			{
				regulator.SetDir();
				Move();
				Accelerate();
			}
			else if(dirMinus && ~dirPlus)
			{
				regulator.RstDir();
				Move();
				Accelerate();
			}
			else if(dirMinus && dirPlus)
			{
				if(regulator.movingPlus)
				{
					Decelerate();
					if(dacOutVal == 0)
					{
						regulator.moving = false;
						dirPlus = false;
						regulator.movingPlus = false;
					}
				}
				if(regulator.movingMinus)
				{
					Decelerate();
					if(dacOutVal == 0)
					{
						regulator.moving = false;
						dirMinus = false;
						regulator.movingMinus = false;
					}
				}
			}


		}

	}



	//----------------------------------------------------------------------------------------


	void PeriodicUpdate()
	{
		tick = true;
		mainClock++;
		auxClock++;
		regulator.wakeClock++;
		testClock++;

		com.PeriodicUpdate();
		enc.WriteReadStart();
		analogOuts.SetOutput1((dacOutVal)>>4);
		analogOuts.SetOutput2((dacOutVal)>>4);
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
							// copy cmdM.desiredAngle to set point value for the regulator
							cmdS.encoderRawData = 200; //enc.data;
							cmdS.status = enc.statusReport;
							com.SendUserData(&cmdS, sizeof(CmdSlave));

						break;

						case 2:	// manual mode
							// copy data from cmdM.manualMode to output of the regulator (open-loop mode)
							cmdS.encoderRawData = 200; //enc.data;
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
