/************************************************************************************************************
 *  Oprogramowanie mikontrolera STM32F4xx
 *
 *	Nazwa pracy, itd.
 *	Autorzy:
 *
 *	 Na podstawie: D. Pazderski, Katedra Sterowania i In¿ynierii Systemów 2016
 *
 ************************************************************************************************************/

#include "common.h"

App * pApp;
App app;

int main(void)
{

	pApp = &app;
	app.Init();
	Led::Init();

	__enable_irq();

	app.Run();
}


