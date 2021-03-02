#define GREEN				PINB0
#define YELLOW				PINB1
#define RED					PINB2
#define BUZZER				PINB3

#define NOTGREEN			(1 << PINB1 | 1 << PINB2)
#define NOTYELLOW			(1 << PINB0 | 1 << PINB2)
#define NOTRED				(1 << PINB0 | 1 << PINB1)

#define ULTRASONIC			PINB4
#define IR_CROSSING			PINB5

#define CONFIG				PINB7

#define IR_ENTRY			PIND2
#define IR_EXIT				PIND3

#define ON(x, y)			x |= y
#define OFF(x, y)			x &= ~y
#define BIT(x)				(1 << x)
#define BIT_ON(x, y)		(x & (BIT(y)))	
#define BIT_OFF(x, y)		(!(x & (BIT(y))))

#define OVERFLOW_COUNT(s)	(int) (s / 0.26)
#define TIME_T0(overflow)	((overflow * 255) + TCNT0)
#define TIME_T2(overflow)	((overflow * 255) + TCNT2)			


/*
System Logic
On the crossing side, the LEDs and buzzer will react as follows
- Green with no sound if there are no vehicles.
- Yellow with two different sounds based on the speed of the approaching vehicle => slow beep for slow speed (<30kmph), and another beep for medium speed (<60kmph)
- Red with fast beeping if the vehicle is approaching at a speed above the speed limit

The LCD and buzzer in the vehicle reacts as follows
- Displays "Pedestrian Crossing Ahead" if no one is at the crossing with one beep
- Displays "Pedestrians Present" with slow beeping on the buzzer if someone is waiting (IR sensors cut)
- Displays "STOP!!" with fast beeping if someone is crossing (IR & Ultrasonic cut)
*/