#pragma once

#include "stm32f4xx.h"

// powiazania kanalow DMA z nadajnikiem i odbiornikiem UART
#define DMA_USART_RX	DMA2_Stream5
#define DMA_USART_TX	DMA2_Stream7

class UartCommunicationInterface
{

	// definition of states in FSM (finite state machine)
	enum FsmState
	{
		FR_IDLE,
		FR_START_1,
		FR_START_2,
		FR_SIZE,
		FR_DATA
	};

	static uint16_t const RX_TIMEOUT = 50;	// in [ms]

	volatile uint8_t  rxFrameIndex;
	volatile FsmState  rxState;

	volatile uint16_t rxBufIndexRead;
	uint16_t rxBufIndexWrite;

	uint16_t rxDmaCounterPrev;
	volatile uint32_t clock;

    volatile uint8_t  crcLo;
	volatile uint8_t  crcHi;

	static uint16_t const RX_BUF_SIZE = 256;
	static uint16_t const TX_BUF_SIZE = 256;


	uint8_t rxBuf[RX_BUF_SIZE];
	uint8_t txBuf[TX_BUF_SIZE];

	uint8_t rxFrame[RX_BUF_SIZE];

	uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

	void HardwareInit();

public:

	uint8_t * txData;
	uint8_t * rxData;
	volatile bool isFrameSending;
	volatile bool isFrameReceived;

	volatile uint8_t  rxFrameSize;

	void Init(void);
	void PeriodicUpdate();
	void Send(uint16_t size);

	bool CheckFrame(void)
	{
		auto crc1 = CRC16(rxFrame, rxFrameSize-1);
		auto crc2 = __REV16(0);

		// TO DO! Check CRC.
		crc2 = crc1;
		if (crc1 == crc2)
		{
			return true;
		}
		else return false;
	}

	void IrqDma()
	{
		DMA2->HIFCR = DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7;
		DMA_USART_TX->CR &= ~DMA_SxCR_EN;
		DMA_USART_TX->CR &= ~DMA_SxCR_TCIE;

		USART1->CR1 |= USART_CR1_TCIE;
	}

	void IrqTx()
	{
		if (USART1->SR & USART_SR_TC)
		{
		    USART1->CR1 &= ~USART_CR1_TCIE;
		    isFrameSending = false;
		}
	}
};

