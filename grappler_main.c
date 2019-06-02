/*
 * Senior_Design_Project.c
 *
 * Created: 5/8/2019 2:29:22 PM
 * Author : user
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 20000000UL

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
	return (b ? (x|(0x01<<k)) :(x &~(0x01<<k)) );
	//set bit to 1		//set bit to 0
}

unsigned char GetBit(unsigned char x, unsigned char k)
{ return ((x & (0x01 << k)) != 0);}



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
	SREG |= 0x80;// 0x80 : 1000000

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

void ADC_init()
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 4.768f) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(F_CPU / (128 * frequency)) - 1; }
		 //else { OCR0A = frequency; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


unsigned char button;
unsigned char grappler = 0x01;

enum S_STATES{INITIAL, OPEN, CLOSE} S_STATE;

void grapplerTick()
{
	button = ~PINA & 0X01;

	switch(S_STATE)
	{
		case INITIAL:
		S_STATE = OPEN;
		break;

		case OPEN:
			if(button)
			{
				set_PWM(0);
				PORTB = 0x01;
			}
			else
			{
				set_PWM(50);
			    PORTB = 0x02;
			}

			S_STATE = OPEN;
		break;

		default:
			S_STATE = INITIAL;
		break;
	}
	
}



int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF; //input
	DDRB = 0xFF; PORTB = 0x00; //output
	DDRC = 0x00; PORTC = 0xFF; //input
	TimerSet(2);
	TimerOn();
	PWM_on();
	//_delay_us(400);
	//set_PWM(95);
    while (1) 
    {

		_delay_us(400);
		set_PWM(0);
		//_delay_us(200);
		//set_PWM(90);
	//	grapplerTick();

		while(!TimerFlag);
		TimerFlag = 0;
    }
}

