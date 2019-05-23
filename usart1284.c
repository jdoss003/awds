

#include "defs.h"
#include "string.h"

#ifndef F_CPU
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#endif

#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif

/*
 * The AVR manual gives us the equation F_CPU / 16 / buad_rate - 1.
 * However, this can cause an off-by-one, so we use this instead.
 *          (F_CPU / 16 / buad_rate + 0.5f) - 1
 */
#define BAUD_PRESCALE(br) ((short)(((float)(F_CPU / 16UL) / (float)br) - 0.5F))
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
        UBRR0L = BAUD_PRESCALE(baudRate);
        UBRR0H = (BAUD_PRESCALE(baudRate) >> 8);
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

void USART_autoReceive(unsigned char b, unsigned char usartNum)
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
    sei();
}

inline unsigned char USART_hasLine(unsigned char usartNum)
{
	if (!usartNum)
	{
		return index0[curBufRead0] > 0 && readBuf0[curBufRead0][index0[curBufRead0] - 1] == '\n';
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

inline void USART_sendLine(const char* l, unsigned char usartNum)
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
    sei();
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
	if (c == '\n' && index0[curBufWrite0] != 0)
	{
		readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
		readBuf0[curBufWrite0][index0[curBufWrite0]] = 0;
		curBufWrite0 = (curBufWrite0 + 1) % MAX_BUFS;
		memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
		index0[curBufWrite0] = 0;
	}
	else
	{
		readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
		if (index0[curBufWrite0] == MAX_BUF)
		{
			memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
			index0[curBufWrite0] = 0;
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
