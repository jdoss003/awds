#include <avr/interrupt.h>
#include <stdlib.h>
#include "string.h"
#include "io.h"


#ifndef F_CPU
#define F_CPU 20000000UL // Assume uC operates at 20MHz
#endif

#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif

#define BAUD_PRESCALE(br) (((F_CPU / (br * 16UL))))
#define MAX_BUF 80
#define MAX_BUFS 5

unsigned char readBuf0[MAX_BUFS][MAX_BUF];
unsigned char index0[MAX_BUFS];
unsigned char curBufRead0 = 0;
unsigned char curBufWrite0 = 0;

unsigned char readBuf1[MAX_BUFS][MAX_BUF];
unsigned char index1[MAX_BUFS];
unsigned char curBufRead1 = 0;
unsigned char curBufWrite1 = 0;


char* out0;
char* out1;

void USART_initBaud(unsigned char usartNum, unsigned short baudRate)
{
    if (usartNum != 1) {
        UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);            // Turn on receiver and transmitter
        UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);            // Use 8-bit character sizes
        UBRR0L = 10;
        UBRR0H = 0;
    }
    else {
        UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);            // Turn on receiver and transmitter
        UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);            // Use 8-bit character sizes
        UBRR1L = BAUD_PRESCALE(baudRate);
        UBRR1H = (BAUD_PRESCALE(baudRate) >> 8);
    }
}

void USART_init(unsigned char usartNum)
{
	USART_initBaud(usartNum, BAUD_RATE);
}

inline unsigned char USART_hasTransmittedLine(unsigned char usartNum)
{
	return !usartNum ? !bit_is_set(UCSR0B, UDRIE0) : !bit_is_set(UCSR1B, UDRIE1);
}

void USART_autoRecieve(unsigned char b, unsigned char usartNum)
{
    if (!usartNum)
    {
		if (b)
			for (unsigned char i = 0; i < MAX_BUFS; ++i)
			{
				index0[i] = 0;
				memset(&readBuf0[i][0], 0, MAX_BUF);
			}
		UCSR0B = b ? UCSR0B | (1 << RXCIE0) : UCSR0B & ~(1 << RXCIE0);
	}
    else
    {
        if (b)
            for (unsigned char i = 0; i < MAX_BUFS; ++i)
            {
                index1[i] = 0;
                memset(&readBuf1[i][0], 0, MAX_BUF);
            }
        UCSR1B = b ? UCSR1B | (1 << RXCIE1) : UCSR1B & ~(1 << RXCIE1);
    }
}

inline unsigned char USART_hasLine(unsigned char usartNum)
{
	if (!usartNum)
	{
		return index0[curBufRead0] > 0 && index0[curBufRead0] - 1 == 9;
	}
    else
    {
        return index1[curBufRead1] > 0 && readBuf1[curBufRead1][index1[curBufRead1] - 1] == '\n';
    }
}

char* USART_getLine(unsigned char usartNum)
{
    if (!usartNum)
    {
        char* temp = &readBuf0[curBufRead0][0];
        index0[curBufRead0] = 0;
        curBufRead0 = (curBufRead0 + 1) % MAX_BUFS;
        return temp;
    }
    else
    {
        char* temp = &readBuf1[curBufRead1][0];
        index1[curBufRead1] = 0;
        curBufRead1 = (curBufRead1 + 1) % MAX_BUFS;
        return temp;
    }
}

void USART_clearBuf(unsigned char usartNum)
{
    if (!usartNum)
    {
        // TODO USART 0
    }
    else
    {
        // TODO USART 1
    }
}

inline void USART_sendLine(char* l, unsigned char usartNum)
{
    if (!usartNum)
    {
        out0 = l;
        UCSR0B |= (1  << UDRIE0);
    }
    else
    {
        out1 = l;
        UCSR1B |= (1  << UDRIE1);
    }
}

ISR(USART0_UDRE_vect)
{
    UDR0 = *out0;
    ++out0;
    if (*out0 == 0)
        UCSR0B = UCSR0B & ~(1 << UDRIE0);
}

 ISR(USART1_UDRE_vect)
 {
     UDR1 = *out1;
     ++out1;
     if (*out1 == 0)
 		UCSR1B = UCSR1B & ~(1 << UDRIE1);
 }

ISR(USART0_RX_vect)
{
    char c = UDR0;
	if (index0[curBufWrite0] < 0)
	{
		char test = (curBufWrite0 + 1) % MAX_BUFS;
		if (test != curBufRead0)
		{
			curBufWrite0 = test;
			memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
			index0[curBufWrite0] = 0;
		}
	}
	if (index0[curBufWrite0] >= 0)
	{
		if((index0[curBufWrite0] == 0 || index0[curBufWrite0] == 1) && c != 0x59)
		{
			index0[curBufWrite0] = 0;
		}
		else if (index0[curBufWrite0] == 9)
		{
			readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
			readBuf0[curBufWrite0][index0[curBufWrite0]] = 0;
			char test = (curBufWrite0 + 1) % MAX_BUFS;
			if (test != curBufRead0)
			{
				curBufWrite0 = test;
				memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
				index0[curBufWrite0] = 0;
			}
			else
			{
				index0[curBufWrite0] = -1;
			}
		}
		else
		{
			readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
			if (index0[curBufWrite0] == MAX_BUF)
			{
				memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
				index0[curBufWrite0] = 0;
			}
			//PORTB=0x01;
		}
	}
}

 ISR(USART1_RX_vect)
 {
     char c = UDR0;
     if (c == '\n' && index1[curBufWrite1] != 0)
     {
         readBuf1[curBufWrite1][index1[curBufWrite1]++] = c;
         readBuf1[curBufWrite1][index1[curBufWrite1]] = 0;
         curBufWrite1 = (curBufWrite1 + 1) % MAX_BUFS;
         memset(&readBuf1[curBufWrite1][0], 0, MAX_BUF);
         index1[curBufWrite1] = 0;
     }
     else
     {
         readBuf1[curBufWrite1][index1[curBufWrite1]++] = c;
         if (index1[curBufWrite1] == MAX_BUF)
         {
             memset(&readBuf1[curBufWrite1][0], 0, MAX_BUF);
             index1[curBufWrite1] = 0;
         }
     }
 }


////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - Sends an 8-bit char value
//Parameter: Takes a single unsigned char value
//			 usartNum specifies which USART will send the char
//Returns: None
void USART_Send(unsigned char sendMe, unsigned char usartNum)
{
	if (usartNum != 1) {
		while( !(UCSR0A & (1 << UDRE0)) );
		UDR0 = sendMe;
	}
	else {
		while( !(UCSR1A & (1 << UDRE1)) );
		UDR1 = sendMe;
	}
}

////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - receives an 8-bit char value
//Parameter: usartNum specifies which USART is waiting to receive data
//Returns: Unsigned char data from the receive buffer
unsigned char USART_Receive(unsigned char usartNum)
{
	if (usartNum != 1) {
		while ( !(UCSR0A & (1 << RXC0)) ); // Wait for data to be received
		return UDR0; // Get and return received data from buffer
	}
	else {
		while ( !(UCSR1A & (1 << RXC1)) );
		return UDR1;
	}
}