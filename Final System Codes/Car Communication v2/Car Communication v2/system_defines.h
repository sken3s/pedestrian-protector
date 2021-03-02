#define U_TRIG				PIND1
#define U_ECHO				PIND2
#define IR					PIND3

#define U_OUT				PINB0
#define IR_OUT				PINB1

#define RF_TXD				PINB2

#define CONFIG				PINB7

#define ON(x, y)			x |= y
#define OFF(x, y)			x &= ~y
#define BIT(x)				(1 << x)
#define BIT_ON(x, y)		(x & (BIT(y)))	
#define BIT_OFF(x, y)		(!(x & (BIT(y))))

#define OVERFLOW_COUNT(s)	(int) (s / 0.26)
#define TIME_T0(overflow)	((overflow * 255) + TCNT0)
#define TIME_T2(overflow)	((overflow * 255) + TCNT2)

#define UL_CONDITION		100			

#define DELAY_1				_delay_ms(20);
#define DELAY_2				_delay_ms(28);
#define DELAY_3				_delay_ms(35);


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