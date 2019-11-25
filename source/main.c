/*	Author: kmaka003
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif


#define Button

// Easy Medium or Hard. 
typedef struct _GameMode{
	// Time allowed to complete Game
	unsigned int TimeAllowed;
	// Time subtracted for each error
	unsigned int TimeOff;
} GameMode;


GameMode Easy;
Easy.TimeAllowed = 0; //???
Easy.TimeOff = 0;

GameMode Medium; 
Medium.TimeAllowed = 0; // ???
Medium.TimeOff = 0; // ??

GameMode ChosenGame;


typedef struct _Task{
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct) (int);

} Task;


const unsigned char tasksSize = 4;
Task tasks[taskSize];


volatile unsigned char TimerFlag = 0; //TimerISR() sets this to a 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; //Start count from here, down to 0. Default to 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1 ms ticks

void TimerOn()
{
	//AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; //bit3 = 0: CTC mode (clear timer on compare)
	//bit2bit1bit0 = 011: pre-scaler /64
	// 00001011: 0x0B
	// so, 8MHz clock or 8,000,000 /64 =125,000 ticks/s
	// Thus, TCNT1 register will count as 125,000 ticks/s
	//AVR output compare register OCR1A.
	OCR1A = 125;   // Timer interrupt will be generated when TCNT1 == OCR1A
	// We want a 1 ms tick. 0.001s *125,000 ticks/s = 125
	// so when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	// Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |=0x80; // 0x80: 1000000

}

void TimerOff()
{
	TCCR1B = 0x00; // bit3bitbit0 -000: Timer off
}

void TimerISR()
{
	unsigned char i;
	for (i = 0;i < tasksSize;++i) {
		if (tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += Period;
	}
}

ISR(TIMER1_COMPA_vect)
{
	//CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) //results in a more efficient compare
	{
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PINA = 0xFF;
	DDRB = 0xFF; PINB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTD = 0x00; 
   /* Insert your solution below */
	unsigned char i = 0;
	// 3 Questions
	tasks[i].state = Questions_init;
	tasks[i].period = 100;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_ThreeLEDS;
	i++;
	// BlinkingLEDS task
	tasks[i].state = Blink_init;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkingLEDS;
	i++;
	// CombineLEDS task
	tasks[i].state = 0;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_CombineLEDS;
	i++;
	// Speaker task
	tasks[i].state = Speaker_Off;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Speaker;
	

	TimerSet(2);
	TimerOn();
    while (1) {

    }
    return 1;
}
