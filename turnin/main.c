/*	Author: adevi002
 *  Partner(s) Name: John Huh
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0;
unsigned char blinkingLED = 0;

enum threeLEDsSM {zero, one, two} state1;
	
int Tick_threeLEDsSM(int state) {
	switch(state) {
		case zero:
			threeLEDs = 0x01;
			state = one;
			break;
		case one:
			threeLEDs = 0x02;
			state = two;
			break;
		case two:
			threeLEDs = 0x04;
			state = zero;
			break;
		default:
			state = zero;
			break;
	}
	return state;
}

enum BlinkingLEDSM { flashOn, flashOff } state2;

int Tick_BlinkingLEDSM(int state) {
	switch(state) {
		case flashOn:
			blinkingLED = 0x08;
			state = flashOff;
			break;
		case flashOff:
			blinkingLED = 0x00;
			state = flashOn;
			break;
		default:
			state = flashOn;
			break;
	}
	return state;
}

enum CombineLEDsSM { combine } state3;

int Tick_CombineLEDsSM(int state) {
	switch(state) {
		case combine:
			state1 = Tick_threeLEDsSM(state1);
			state2 = Tick_BlinkingLEDSM(state2);
			PORTB = threeLEDs + blinkingLED;
			break;
		default:
			state = combine;
			break;
	}
	return state;
}

int main(void)
{
    DDRB = 0xFF;	PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	/* Replace with your application code */
    while (1) 
    { 
		state3 = Tick_CombineLEDsSM(state3);
		while(!TimerFlag) {}
		TimerFlag = 0;
    }
}
