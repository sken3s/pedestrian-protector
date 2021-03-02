/*
 * Car v2.c
 *
 * Created: 21/01/2020 15:09:34
 * Author : devi
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "system_defines.h"
#include "LCD20x4.h"

//Function Defines
void system_setup(void);
void config_system(void);
void beeping(int speed);
void delay_T0(int state);
void delay_T2(int state);
/*void listening(void);*/
//Global Variables
int active = 0; 
int time_t1 = 0;
int overflow_T0 = 0;
int overflow_T2 = 0;
int condition = 0;
int new_condition = 0;
int system_ready = 0;

int main(void)
{
	system_setup();

	while (1)
	{
		if (BIT_OFF(PIND, CONFIG))
		{
			config_system();
			system_ready = 0;
		}
		
		else
		{
			//Let system get ready first
			if (system_ready == 0)
			{
				ClearScreen();
				cli();
				active = 0;
				condition = 0;
				new_condition = 0;
				GoToAddress(2, 0);
				SendString("   GETTING  READY   ");
				_delay_ms(2000);
				GoToAddress(2, 0);
				SendString("       READY        ");
				_delay_ms(1000);
				system_ready = 1;
				ClearScreen();
				sei();
			}
			

			GoToAddress(2, 0);
			if (condition == 0)
			{
				SendString("     Drive Safe     ");
				GoToAddress(3, 0);
				SendString("                    ");
// 				ON(PORTB, BIT(GREEN));
// 				OFF(PORTB, BIT(RED));
// 				OFF(PORTD, BIT(BUZZER));
// 				overflow_T2 = 0;
// 				delay_T2(0);
			}
			else if (condition == 1)
			{
				SendString("   CROSSING AHEAD   ");
				GoToAddress(3, 0);
				SendString("                    ");
// 				ON(PORTB, BIT(GREEN));
// 				OFF(PORTB, BIT(RED));
/*				beeping(1);*/
			}
			else if  (condition == 2)
			{
				SendString("       ALERT        ");
				GoToAddress(3, 0);
				SendString("PEDESTRIAN  DETECTED");
// 				ON(PORTB, BIT(RED));
/*				beeping(2);*/
			}
			else if (condition == 3)
			{
				SendString("     SLOW DOWN!     ");
				GoToAddress(3, 0);
				SendString("                    ");
// 				ON(PORTB, BIT(RED));
// 				OFF(PORTB, BIT(GREEN));
/*				beeping(3);*/
			}
		}
	}
}

void system_setup(void)
{
	DDRD |= (1 << BUZZER);
	DDRD &= ~(1 << RF_RXD | 1 << CONFIG);
	DDRB |= (1 << GREEN | 1 << RED);
	
	PORTD |= (1 << CONFIG | 1 << BUZZER);
	
	GICR |= (1 << INT1);
	MCUCR |= (1 << ISC11);
	
	Initialise_LCD();
	ClearScreen();
	GoToAddress(2, 0);
	SendString("   System Active    ");
	
	ON(PORTD, BIT(BUZZER));
	_delay_ms(1000);
	OFF(PORTD, BIT(BUZZER));
	
	ClearScreen();
	
	unsigned char gear[8] = {0x00,0x00,0x15,0x0E,0x1B,0x0E,0x15,0x00};
	CustomChar(0, gear);
	
	sei();
}

void config_system(void)
{
	GoToAddress(1, 0);
	SendCharacter(0x0);
	GoToAddress(1, 19);
	SendCharacter(0x0);
	
	GoToAddress(4,0);
	SendCharacter(0x0);
	GoToAddress(4, 19);
	SendCharacter(0x0);
	
	GoToAddress(2, 0);
	SendString("    CONFIG MODE     ");
	
	
	while (BIT_OFF(PIND, CONFIG))
	{
		GoToAddress(3, 0);
		
		if (condition == 2)
		{
			SendString("    RF DETECTED     ");
// 			ON(PORTB, BIT(GREEN));
// 			OFF(PORTB, BIT(RED));
		}
		else
		{
			SendString("   WAITING FOR RF   ");
// 			ON(PORTB, BIT(RED));
// 			OFF(PORTB, BIT(GREEN));
		}
	}
}

void beeping(int mode)
{
	delay_T2(1);
	
	if (mode == 1)
	{
		if (overflow_T2 == 0)
			ON(PORTD, BIT(BUZZER));
		else if (overflow_T2 > 2)
			OFF(PORTD, BIT(BUZZER));
	}
	else if (mode == 2)
	{
		if (overflow_T2 % 4 == 0)
			ON(PORTD, BIT(BUZZER));
		else if (overflow_T2 % 2 == 0)
			OFF(PORTD, BIT(BUZZER));
	}
	else if (mode == 3)
	{
		ON(PORTD, BIT(BUZZER));
	}
}


// void listening(void)
// {
// // 	sei();
// // 	
// // 	delay_T0(1);
// // 	while (TIME_T0(overflow_T0))
// // 	{
// // 		if (overflow_T0 > 2)
// // 			break;
// // 	}
// 	
// 	if (time_t1 < 25)
// 		condition = 0;
// 	if (time_t1 > 25 && time_t1 < 40)
// 		condition = 1;
// 	else if  (time_t1 > 40 && time_t1 < 60)
// 		condition = 2;
// 	else if (time_t1 > 60 && time_t1 < 100)
// 		condition = 3;
// 	
// // 	overflow_T0 = 0;
// // 	
// // 	delay_T0(0);
// // 	cli();
// }

void delay_T0(int state)
{
	if (state == 1)
	{
		TCCR0 = (1 << CS00 | 1 << CS02);
		TIMSK |= (1 << TOIE0);
	}
	else
	{
		TCCR0 = 0;
		TCNT0 = 0;
		TIMSK &= ~(1 << TOIE0);
		overflow_T0 = 0;
	}
}

void delay_T2(int state)
{
	if (state == 1)
	{
		TCCR2 = (1 << CS20 | 1 << CS21 | 1 << CS22);
		TIMSK |= (1 << TOIE2);
	}
	else
	{
		TCCR2 = 0;
		TCNT2 = 0;
		TIMSK &= ~(1 << TOIE2);
		overflow_T2 = 0;
	}
}

ISR(TIMER0_OVF_vect)
{
	overflow_T0++;
}

ISR(TIMER2_OVF_vect)
{
	overflow_T2++;
}

ISR(INT1_vect)
{
	if (active == 0)
	{
		TCCR1B |= (1 << CS12 | 1 << CS10);
		active = 1;
		time_t1 = 0;
	}
	else
	{
		TCCR1B = 0;
		time_t1 = TCNT1 * 1.024;
		TCNT1 = 0;
		active = 0;
		
		if (time_t1 < 25)
			new_condition = 0;
		if (time_t1 > 25 && time_t1 < 40)
			new_condition = 1;
		else if  (time_t1 > 40 && time_t1 < 60)
			new_condition = 2;
		else if (time_t1 > 60 && time_t1 < 100)
			new_condition = 3;
			
		//Start timer
		//If new condition !=  old condition for certain amt of time change condition
		//Else keep old condition, turn off timer
		
		if (new_condition != condition)
		{
			delay_T0(1);
			
			if (TCNT0 > 146)
			{
				condition = new_condition;
				delay_T0(0);
			}
		}
		else
		{
			delay_T0(0);
		}
	}
	
	
}

