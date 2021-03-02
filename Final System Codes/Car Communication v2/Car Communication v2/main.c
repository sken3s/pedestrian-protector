/*
 * Car Communication v2.c
 *
 * Created: 23/01/2020 13:40:41
 * Author : devi
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "system_defines.h"
//Function Defines
void system_setup(void);
void config_system(void);
int ultrasonic(void);
//Global Variables
int active = 0;
int t1_timer = 0;
int t0_timer = 0;
int t0_overflow = 0;
int waiting_condition = 0;
int crossing_condition = 0;
int ul_distance = 0;


int main(void)
{
	system_setup();
	while (1)
	{
		if (BIT_OFF(PINB, CONFIG))
		{
			config_system();
		}
		else
		{
			ul_distance = ultrasonic();
			
			if (ul_distance < UL_CONDITION && waiting_condition == 1)
			{
				crossing_condition = 1;
				waiting_condition = 0;
				t0_overflow = 0;
				GICR &= ~(1 << INT0);
			}
			
			if (crossing_condition == 1)
			{
				ON(PORTB, BIT(RF_TXD));
				DELAY_3;
				OFF(PORTB, BIT(RF_TXD));
				DELAY_3;
			}
			else if (waiting_condition == 1)
			{
				ON(PORTB, BIT(RF_TXD));
				DELAY_2;
				OFF(PORTB, BIT(RF_TXD));
				DELAY_2;
			}
			else
			{
				ON(PORTB, BIT(RF_TXD));
				DELAY_1;
				OFF(PORTB, BIT(RF_TXD));
				DELAY_1;
			}
		}
	}
}

void system_setup(void)
{
	DDRB |= (1 << U_OUT | 1 << IR_OUT | 1 << RF_TXD);
	DDRB &= ~(1 << CONFIG);
	
	DDRD |= (1 << U_TRIG | 1 << RF_TXD);
	DDRD &= ~(1 << U_ECHO | 1 << IR);
	
	PORTB |= (1 << CONFIG);
	
	GICR |= (1 << INT0 | 1 << INT1);
	MCUCR |= (1 << ISC00 | 1 << ISC10);
	
	TIMSK |= (1 << TOIE0);
	
	sei(); //Enabling Global Interrupts
}

void config_system(void)
{
	while (BIT_OFF(PINB, CONFIG))
	{
		ul_distance = ultrasonic();
		
		if (ul_distance < UL_CONDITION)
			OFF(PORTB, BIT(U_OUT));
		else
			ON(PORTB, BIT(U_OUT));
		
		if (BIT_OFF(PIND, IR))
			OFF(PORTB, BIT(IR_OUT));
		else
			ON(PORTB, BIT(IR_OUT));
		
		ON(PORTB, BIT(RF_TXD));
		DELAY_2;
		OFF(PORTB, BIT(RF_TXD));
		DELAY_2;
	}
	
	TCCR0 = 0;
	waiting_condition = 0;
	crossing_condition = 0;
	active = 0;
	t1_timer = 0;
}

int ultrasonic(void)
{
	ON(PORTD, BIT(U_TRIG));
	_delay_us(10);
	OFF(PORTD, BIT(U_TRIG));
	return t1_timer/58;
}

ISR(INT0_vect)
{
	if (active == 1)
	{
		TCCR1B = 0;
		t1_timer = TCNT1;
		TCNT1 = 0;
		active = 0;
	}
	
	if (active == 0)
	{
		TCCR1B |= (1 << CS10);
		active = 1;
	}
}

ISR(INT1_vect)
{
	if (crossing_condition == 0 && waiting_condition == 0)
	{
		TCCR0 |= (1 << CS00 | 1 << CS02);
		waiting_condition = 1;
	}
	
}

ISR(TIMER0_OVF_vect)
{
	t0_overflow++;
	
	if (t0_overflow > 38)
	{
		/*GICR |= (1 << INT1);*/
		if (waiting_condition == 1)
			waiting_condition = 0;
		else if (crossing_condition == 1)
		{
			crossing_condition = 0;
			GICR |= (1 << INT0);
		}
		TCCR0 = 0;
		TCNT0 = 0;
		t0_overflow = 0;
	}
}

