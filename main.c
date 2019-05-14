/*
 * LIDAR_Senior_Design.c
 *
 * Created: 5/13/2019 3:19:23 PM
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "USART.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

volatile unsigned char TimerFlag = 0; //TimerISR() sets this to 1. c programmer should clear to 0

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; //start count from here down to 0. default 1ms.
unsigned long _avr_timer_cntcurr =0; //current internal count of 1ms ticks

void TimerOn()
{
	//AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; //bit3 = 0; CTC mode (clear timer on compare)
	//bit2bit1bit0=011: pre-scaler /64
	//00001011: 0x0B
	//SO, 8 MHz clock or 8,000,000 / 64 = 125,000 ticks/s
	//Thus, TCNT1 register will count at 125,000 ticks/s

	//AVR output compare register OCR1A
	OCR1A = 125; // timer interrupt will be generated when TCTN1 == OCR1A
	//We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125;
	//so when TCHTN1 register equals 125,
	//1 ms has passed. Thus, we compare to 125
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1 =0;

	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr milliseconds

	//enable global interrupts
	//SREG |= 0x80;// 0x80 : 1000000
	sei();
}


void TimerOFF()
{
	TCCR1B = 0x00; //bit3bit1bit0: timer off

}
void TimerISR()
{
	TimerFlag = 1;
}

//in our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	//CPU automattically calls when TCNT1==OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; //Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr==0)//results in a more efficient compare
	{
		TimerISR(); //call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;

}

int counter=0;
char counter_test = 0;
unsigned int temp_counter=0;
char lidarVar[200];
int HexToDec= 0;
char buffer[200];
char buffered_string[200];
char print_flag = 0;
const char *temp_Val = 0;
char wait_counter = 0;



enum L_States{L_Initial, L_Wait }L_State;

void LIDAR_Tick()
{
	switch(L_State)
	{
		case L_Initial:
		L_State = L_Wait;
		break;

		case L_Wait:
			if(wait_counter < 0)
			{
				wait_counter++;
			}
			else
			{
				if(USART_HasReceived(0))
				{
					if(counter_test <= 9)
					{
						lidarVar[counter_test] = USART_Receive(0);
						counter_test++;
					}
					else
					{
						if(print_flag == 0)
						{
							counter_test = 0;
							print_flag = 1;
							for(int i=0; i<9; i++)
							{
								itoa(lidarVar[i], buffer, 16);
								strcat(buffered_string, buffer);
								
							}
						}
					}
				}
						
			}

			

			if(temp_counter >= 100)
			{
				LCD_DisplayString(1, buffered_string);
				//memset(buffered_string,'\0',200);
				//itoa(lidarVar, buffer, 16);
				//LCD_DisplayString(1, buffer);
				temp_counter=0;
			}
			else
			{
				temp_counter++;
				
			}

			if(counter>=2)
			{
				PORTB=0x01;
				counter=0;
			}
			else
			{
				PORTB=0x02;
				counter++;
			}
			//USART_Flush(0);
		L_State = L_Wait;
		break;

		default:
		L_State = L_Initial;
		break;

	}
}




int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; //PORTB all ports are set to output
	DDRD = 0xFE; PORTD = 0x01; //PORTD port0 set to input for USART RDXO, ports1-7 set to output for use with lcd screen
	DDRC = 0xFF; PORTC = 0x00; //PORTC set for lcd screen
	
	initUSART(0);
	USART_Flush(0);
	TimerSet(1);
	TimerOn();
	LCD_init();
	LCD_DisplayString(1, "test2");

    /* Replace with your application code */
    while (1) 
    {
		
		LIDAR_Tick();
		
		while(!TimerFlag); //Wait 1 sec
		TimerFlag = 0;
    }
}

