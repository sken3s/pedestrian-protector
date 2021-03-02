/*
 * Pedestrian Communication v2.c
 *
 * Created: 23/01/2020 11:12:38
 * Author : devi
 */ 

#include <avr/io.h>


#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "system_defines.h"
//Function Defines
void system_setup(void);
void config_system(void);
void beeping(int mode);

//Global System
int detecting = 0;
int t0_timer = 0;
int t0_overflow = 0;
int t1_timer = 0;
float ir_distance = 0.15;
float speed = 0;


int main(void)
{
	system_setup();
	while (1)
	{
		if (BIT_OFF(PINB, CONFIG))
			config_system();
		else
		{
			if (speed > 0)
			{
				if (speed < 60)
				{
					ON(PORTB, BIT(YELLOW));
					OFF(PORTB, NOTYELLOW);
					if (speed < 30)
						beeping(1);
					else if (speed < 60)
						beeping(2);
				}
				else
				{
					ON(PORTB, BIT(RED));
					OFF(PORTB, NOTRED);
					beeping(3);
				}
			}
			else
			{
				ON(PORTB, BIT(GREEN));
				OFF(PORTB, NOTGREEN);
				OFF(PORTB, BIT(BUZZER));
			}
		}
	}
}

void system_setup(void)
{
	DDRB |= (1 << GREEN | 1 << YELLOW | 1 << RED | 1 << BUZZER);
	DDRB &= ~(1 << ULTRASONIC | 1 << IR_CROSSING | 1 << CONFIG);
	
	OFF(PORTB, BIT(BUZZER));
	ON(PORTB, BIT(CONFIG));
	
	DDRD &= ~(1 << IR_ENTRY | 1 << IR_EXIT);
	
	GICR |= (1 << INT0 | 1 << INT1);
	MCUCR |= (1 << ISC01 | 1 << ISC11);
	TIMSK |= (1 << TOIE0);
	
	sei(); //Enabling Global Interrupts
}

void config_system(void)
{
	int i;
	//Checking Buzzer and LEDs
	for (i = 0; i < 5; i++)
	{
		ON(PORTB, 0x7);
		_delay_ms(100);
		OFF(PORTB, 0x7);
		_delay_ms(100);
	}
	
	beeping(1);
	beeping(2);
	
	while (BIT_OFF(PINB, CONFIG))
	{
		if (BIT_OFF(PINB, ULTRASONIC))
		{
			ON(PORTB, BIT(YELLOW));
			OFF(PORTB, BIT(GREEN));
		}
		else
		{
			ON(PORTB, BIT(GREEN));
			OFF(PORTB, BIT(YELLOW));
		}
		
		if (BIT_OFF(PIND, IR_ENTRY) || BIT_OFF(PIND, IR_EXIT) || BIT_OFF(PINB, IR_CROSSING))
			ON(PORTB, BIT(RED));
		else
			OFF(PORTB, BIT(RED));
	}
	
	OFF(PORTB, 0x7);
	OFF(PORTB, BIT(BUZZER));
	
}

void beeping(int mode)
{
	if (mode == 1)
	{
		ON(PORTB, BIT(BUZZER));
		_delay_ms(1000);
		OFF(PORTB, BIT(BUZZER));
		_delay_ms(1000);
	}
	else if (mode == 2)
	{
		ON(PORTB, BIT(BUZZER));
		_delay_ms(250);
		OFF(PORTB, BIT(BUZZER));
		_delay_ms(250);
	}
	else if (mode == 3)
	{
		ON(PORTB, BIT(BUZZER));
	}
}


ISR(INT0_vect)
{
	TCCR1B |= (1 << CS10 | 1 << CS12);
	detecting = 1;
}

ISR(INT1_vect)
{
	if (detecting == 1)
	{
		TCCR1B = 0;
		t1_timer = TCNT1 * 1.024;
		speed = (ir_distance/t1_timer) * 3600;
		TCNT1 = 0;
		detecting = 0;
		TCCR0 |= (1 << CS00 | 1 << CS02);
		GICR &= ~(1 << INT0 | 1 << INT1);
	}
}

ISR(TIMER0_OVF_vect)
{
	t0_overflow++;
	
	if (t0_overflow > OVERFLOW_COUNT(10))
	{
		TCCR0 = 0;
		TCNT0 = 0;
		t0_overflow = 0;
		speed = 0;
		t1_timer = 0;
		GICR |= (1 << INT0 | 1 << INT1);
	}
}

