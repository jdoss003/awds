/*
 * File: timer.c
 * Author : Justin Doss
 *
 * Methods to control timer 0/3 which can be configured for ms time periods.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */
#include "defs.h"

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
volatile unsigned long _avr_timer_a_M = 1;         // Start count from here, down to 0. Default 1 ms.
volatile unsigned long _avr_timer_a_cntcurr = 0;   // Current internal count of 1ms ticks
volatile unsigned long _avr_timer_b_M = 1;         // Start count from here, down to 0. Default 1 ms.
volatile unsigned long _avr_timer_b_cntcurr = 0;   // Current internal count of 1ms ticks

void TimerOnA()
{
    // AVR timer/counter controller register TCCR1
    TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
    // bit2bit1bit0=011: pre-scaler /64
    // 00001011: 0x0B
    // SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
    // Thus, TCNT1 register will count at 125,000 ticks/s
    // AVR output compare register OCR1A.
    OCR1A = F_CPU / 64UL / 1000UL;    // Timer interrupt will be generated when TCNT1==OCR1A
    // We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
    // So when TCNT1 register equals 125,
    // 1 ms has passed. Thus, we compare to 125.
    // AVR timer interrupt mask register
    TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

    //Initialize avr counter
    TCNT1 = 0;

    _avr_timer_a_cntcurr = _avr_timer_a_M;
    // TimerISR will be called every _avr_timer_cntcurr milliseconds

    //Enable global interrupts
    sei(); // 0x80: 1000000
}

void TimerOffA()
{
    TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

// Set TimerISR() to tick every M ms
void TimerSetA(unsigned long M)
{
    _avr_timer_a_M = M;
    _avr_timer_a_cntcurr = _avr_timer_a_M;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_a_cntcurr--;                 // Count down to 0 rather than up to TOP
    if (_avr_timer_a_cntcurr == 0)
    {                                       // results in a more efficient compare
        TIMER_A_ISR();                      // Call the ISR that the user uses
        _avr_timer_a_cntcurr = _avr_timer_a_M;
    }
}

void TimerOnB()
{
    // AVR timer/counter controller register TCCR0
    TCCR0A = 0x02;// bit1 = 1: CTC mode (clear timer on compare)
    TCCR0B = 0x03;
    // bit2bit1bit0=011: pre-scaler /64
    // SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
    // Thus, TCNT0 register will count at 125,000 ticks/s
    // AVR output compare register OCR0A.
    OCR0A = F_CPU / 64UL / 1000UL;    // Timer interrupt will be generated when TCNT0==OCR0A
    // We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
    // So when TCNT0 register equals 125,
    // 1 ms has passed. Thus, we compare to 125.
    // AVR timer interrupt mask register
    TIMSK0 = 0x02; // bit1: OCIE0A -- enables compare match interrupt

    //Initialize avr counter
    TCNT0 = 0;

    _avr_timer_b_cntcurr = _avr_timer_b_M;
    // TimerISR will be called every _avr_timer_cntcurr milliseconds

    //Enable global interrupts
    sei(); // 0x80: 1000000
}

void TimerOffB()
{
    TCCR0B = 0x00; // bit3bit1bit0=000: timer off
}

// Set TimerISR() to tick every M ms
void TimerSetB(unsigned long M)
{
    _avr_timer_b_M = M;
    _avr_timer_b_cntcurr = _avr_timer_b_M;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER0_COMPA_vect)
{
    // CPU automatically calls when TCNT0 == OCR0 (every 1 ms per TimerOn settings)
    _avr_timer_b_cntcurr--;                 // Count down to 0 rather than up to TOP
    if (_avr_timer_b_cntcurr == 0)
    {                                       // results in a more efficient compare
        TIMER_B_ISR();                      // Call the ISR that the user uses
		_avr_timer_b_cntcurr = _avr_timer_b_M;
    }
}