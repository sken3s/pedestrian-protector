/*
 * LCD20x4.c
 *
 * Created: 28-May-19 20:08:36
 * Author : Devishke
 */ 

#ifndef LCD
#define LCD

#include <avr/io.h>
#include <util/delay.h>

#define Data			PORTA
#define Data_Dir		DDRA
#define Control			PORTB
#define Control_Dir		DDRB
#define RS				PINB0
#define RW				PINB1
#define E				PINB2

void Initialise_LCD();
void ClearScreen();
void SecondLine();
void CheckBusy();
void Flash();
void SendCommand(unsigned char command);
void SendCharacter(unsigned char character);
void SendString(char *string);
void CustomChar(unsigned char CGLoc, unsigned char *custom);
void GoToAddress(unsigned char line, unsigned char pos);


void Initialise_LCD()
{
	Control_Dir |= 1 << RS | 1 << RW | 1 << E;
	_delay_ms(15);
	
	SendCommand(0x01);
	_delay_ms(2);
	SendCommand(0x38);
	_delay_us(50);
	SendCommand(0b00001100);
	_delay_us(50);
	SendCommand(0x06);
	_delay_ms(50);
}

void ClearScreen()
{
	SendCommand(0b1);
	_delay_ms(2);
}

void SecondLine()
{
	SendCommand(0b11000000);
	_delay_us(50);
}

void CheckBusy() //Check if the LCD can receive a command
{
	Data_Dir = 0;
	Control |= 1 << RW;
	Control &= ~(1 << RS);
	
	while(Data >= 0x80)
	{
		Flash();
	}
	
	Data_Dir = 0xFF;
}

void Flash() //Necessary for sending commands/characters to LCD
{
	Control |= 1 << E;
	asm volatile ("nop");
	asm volatile ("nop");
	Control &= ~ (1 << E);
}

void SendCommand(unsigned char command)
{
	CheckBusy();
	Data = command;
	Control &= ~(1 << RW | 1 << RS);
	Flash();
	
	Data = 0;
}

void SendCharacter(unsigned char character)
{
	CheckBusy();
	Data = character;
	Control &= ~(1 << RW);
	Control |= 1 << RS;
	Flash();
	
	Data = 0;
}

void SendString(char *string)
{
	while (*string)
	{
		SendCharacter(*string++);
	}
}

void CustomChar(unsigned char CGLoc, unsigned char *custom)
{
	if (CGLoc < 8)
	{
		SendCommand(0x40 + (8 * CGLoc));
		_delay_us(50);
		int i;
		for (i = 0; i < 8; i++)
			SendCharacter(*custom++);
		
		SendCommand(0x80);
		_delay_us(50);
	}
}

void GoToAddress(unsigned char line, unsigned char pos)
{
	if (pos < 20)
	{
		if (line == 1)
			SendCommand(0x80 + pos);
		else if (line == 2)
			SendCommand(0xC0 + pos);
		else if (line == 3)
			SendCommand(0x94 + pos);
		else if (line == 4)
			SendCommand(0xD4 + pos);
	}
	
	_delay_us(50);
}

#endif