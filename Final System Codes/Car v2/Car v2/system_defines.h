
//Defined in LCD header
// #define Data			PORTA
// #define Data_Dir		DDRA
// #define Control			PORTB
// #define Control_Dir		DDRB
// #define RS				PINB3
// #define RW				PINB1
// #define E				PINB2

#define RF_RXD			PIND3
#define BUZZER			PIND4
#define CONFIG			PIND2

#define GREEN			PINB6
#define RED				PINB7

#define ON(x, y)			x |= y
#define OFF(x, y)			x &= ~y
#define BIT(x)				(1 << x)
#define BIT_ON(x, y)		(x & (BIT(y)))
#define BIT_OFF(x, y)		(!(x & (BIT(y))))

#define OVERFLOW_COUNT(s)	(int) (s / 0.26)
#define TIME_T0(overflow)	((overflow * 255) + TCNT0)

/*
The LCD and buzzer in the vehicle reacts as follows
- Displays "Pedestrian Crossing Ahead" if no one is at the crossing with one beep
- Displays "Pedestrians Present" with slow beeping on the buzzer if someone is waiting (IR sensors cut)
- Displays "STOP!!" with fast beeping if someone is crossing (IR & Ultrasonic cut)
*/