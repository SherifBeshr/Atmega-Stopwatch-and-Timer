/*
 * 	Project_2	Stop Watch + TIMER
 *
 *  Created on: Jan 10, 2022
 *      Author: Sherif Beshr
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "main.h"


unsigned char sec_units = 0;
unsigned char sec_tenth = 0;
unsigned char min_units = 0;
unsigned char min_tenth = 0;
unsigned char hrs_units = 0;
unsigned char hrs_tenth = 0;


/*	FCPU = 1 MHz	*/

void Timer1_Init(void)
{
	TCNT1 = 0;				// Bottom = 0
	OCR1A = 977;			// Clock for 1 second
	TIMSK |= (1<<OCIE1A);	// Set Module flag
	/*
	 * 	CTC Mode , Pre-scalar = 1024 >> Time per clock cycle = 1024/1MHz = 1.024mS
	 *	FOC1A = 1 , WGM12 = 1 , CS12 = 1 , CS10 = 1
	 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}

void Ext_INT0_Reset(void)
{
	DDRD &= ~(1<<RESET);				// External Interrupt 0 as input
	PORTD |= (1<<RESET);				// External Interrupt 0 Internal pull-up activated
	MCUCR |= (1<<ISC01);				// Falling Edge activated
	GICR |= (1<<INT0);					// External Interrupt Request 0 Enable
}

void Ext_INT1_Pause(void)
{
	DDRD &= ~(1<<Pause);				// External Interrupt 1 as input
	MCUCR |= (1<<ISC11) | (1<<ISC10);	// Rising Edge activated
	GICR |= (1<<INT1);					// External Interrupt Request 1 Enable
}

void Ext_INT2_Resume()
{
	DDRB &= ~(1<<RESUME);				// External Interrupt 2 as input
	PORTB |= (1<<RESUME);				// External Interrupt 2 Internal pull-up activated
	MCUCSR &= ~(1<<ISC2);				// Falling Edge activated
	GICR |= (1<<INT2);					// External Interrupt Request 2 Enable
}

ISR (TIMER1_COMPA_vect)					// Interrupt Every Second occurs
{
	if (!(PIND & (1<<Mode_Selector)))	// Stop Watch Mode
	{
		++sec_units;
	}
	else								// Timer Mode
	{
		--sec_units;
	}
}

ISR (INT0_vect)							// Reset Interrupt resets all numbers to 0
{
	sec_units = sec_tenth = min_units = min_tenth = hrs_units = hrs_tenth = 0;
}

ISR (INT1_vect)							// Pause Interrupt stops the timer
{
	TCCR1B = 0;
}

ISR (INT2_vect)							// Resume Interrupt initializes timer again
{
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}

/*************************************** Timer Mode Time Selector ***************************************/

char selector = 0;						// Selector for Time ( Seconds / Minutes / Hours )

void Mode_Selection_Trial()
{
	if (selector == 1)					// Change Seconds
		Seconds_Selector();
	else if (selector == 2)				// Change Minutes
		Minutes_Selector();
	else if (selector == 3)				// Change Hours
		Hours_Selector();
	else
		return;
}

void Seconds_Selector()
{
	while (PINB & (1<<Time_Selector)) {display();}		// Keep displaying while the Selector button is pushed
	PORTB &= ~(1<<Seconds_LED);
	while (selector == 1)
	{
		PORTB |= (1<<Seconds_LED);						// Keep Seconds LED ON while in seconds selection mode
		display();										// Keep display ON
		if (PINB & (1<<Time_Selector))					// Check if Time selector is pressed
		{
			_delay_ms(50);
			if (PINB & (1<<Time_Selector))				// Button de-bounce check
			{
				PORTB &= ~(1<<Seconds_LED);				// Turn OFF seconds LED
				selector = 1;
				return;
			}
		}
		if (PINB & (1<<Timer_Inc))						// Increment time button
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Inc))					// Button de-bounce check
			{
				++sec_units;							// Increment seconds
				if (sec_units == 10)					// If units reached top
				{
					++sec_tenth;
					sec_units = 0;
				}
				if (sec_tenth == 6)						// If tenth reached top
				{
					sec_tenth = sec_units = 0;
				}
			}
			while ((PINB & (1<<Timer_Inc))) {display();}
		}
		else if (PINB & (1<<Timer_Dec))					// Decrement time button
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Dec))					// Button de-bounce check
			{
				--sec_units;							// Decrement seconds
				if (sec_units == 255)					// Ex: 30 seconds >> 29 seconds
				{
					--sec_tenth;
					sec_units = 9;
				}
				else if (sec_tenth == 0 && sec_units == 0)	// Ex: 00 seconds >> 59 seconds
				{
					sec_units = 9; sec_tenth = 5;
				}
			}
			while ((PINB & (1<<Timer_Dec))) {display();}
		}
	}
}

void Minutes_Selector()
{
	while (PINB & (1<<Time_Selector)) {display();}		// Keep displaying while the Selector button is pushed
	PORTB &= ~(1<<Minutes_LED);
	while (selector == 2)
	{
		PORTB |= (1<<Minutes_LED);						// Keep Minutes LED ON while in minutes selection mode
		display();										// Keep display ON
		if (PINB & (1<<Time_Selector))					// Check if Time selector is pressed
		{
			_delay_ms(50);
			if (PINB & (1<<Time_Selector))				// Button de-bounce check
			{
				PORTB &= ~(1<<Minutes_LED);				// Turn OFF minutes LED
				selector = 2;
				return;
			}
		}
		if (PINB & (1<<Timer_Inc))						// Increment time button
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Inc))					// Button de-bounce check
			{
				++min_units;							// Increment minutes
				if (10 == min_units)					// If units reached top
				{
					++min_tenth;
					min_units = 0;
				}
				if (6 == min_tenth )					// If tenth reached top
				{
					min_tenth = min_units = 0;
				}
			}
			while ((PINB & (1<<Timer_Inc))) {display();}
		}
		else if (PINB & (1<<Timer_Dec))
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Dec))
			{
				--min_units;
				if (min_units == 255)
				{
					--min_tenth;
					min_units = 9;
				}
				else if (min_tenth == 0 && min_units == 0)
				{
					min_units = 9; min_tenth = 5;
				}
			}
			while ((PINB & (1<<Timer_Dec))) {display();}
		}
	}
}

void Hours_Selector()
{
	while (PINB & (1<<Time_Selector)) {display();}		// Keep displaying while the Selector button is pushed
	PORTB &= ~(1<<Hours_LED);
	while (selector == 3)
	{
		PORTB |= (1<<Hours_LED);						// Keep Hours LED ON while in hours selection mode
		display();										// Keep display ON
		if (PINB & (1<<Time_Selector))					// Check if Time selector is pressed
		{
			_delay_ms(50);
			if (PINB & (1<<Time_Selector))				// Button de-bounce check
			{
				PORTB &= ~(1<<Hours_LED);				// Turn OFF hours LED
				selector = -1;
				return;
			}
		}
		if (PINB & (1<<Timer_Inc))
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Inc))					// Button de-bounce check
			{
				++hrs_units;
				if (10 == hrs_units)
				{
					++hrs_tenth;
					hrs_units = 0;
				}
				if (6 == hrs_tenth )
				{
					hrs_tenth = hrs_units = 0;
				}
			}
			while ((PINB & (1<<Timer_Inc))) {display();}
		}
		else if (PINB & (1<<Timer_Dec))
		{
			_delay_ms(30);
			if (PINB & (1<<Timer_Dec))
			{
				--hrs_units;
				if (hrs_units == 255)
				{
					--hrs_tenth;
					hrs_units = 9;
				}
				else if (hrs_tenth == 0 && hrs_units == 0)
				{
					hrs_units = 9; hrs_tenth = 5;
				}
			}
			while ((PINB & (1<<Timer_Dec))) {display();}
		}
	}
}




/*************************************** 7-Segments Display ***************************************/

void display ()
{
	PORTA = (1<<0);												// First 7 segment displays
	PORTC = (PORTC & 0xF0) | (sec_units & 0x0F  ) ;				// the seconds units are displayed
	_delay_us(5) ;

	PORTA = (1<<1);
	PORTC = (PORTC & 0xF0) | (sec_tenth & 0x0F  ) ;
	_delay_ms(5) ;

	PORTA = (1<<2);
	PORTC = (PORTC & 0xF0) | (min_units & 0x0F  ) ;
	_delay_ms(5) ;

	PORTA = (1<<3);
	PORTC = (PORTC & 0xF0) | (min_tenth & 0x0F  ) ;
	_delay_ms(5) ;

	PORTA = (1<<4);
	PORTC = (PORTC & 0xF0) | (hrs_units & 0x0F  ) ;
	_delay_ms(5) ;

	PORTA = (1<<5);
	PORTC = (PORTC & 0xF0) | (hrs_tenth & 0x0F  ) ;
	_delay_ms(5) ;


	if (0 == hrs_tenth && 0 == hrs_units && 0 == min_tenth && 0 == min_units && 0 == sec_tenth && 0 == sec_units)
	{
		PORTD |= (1<<Timer_LED);					// Turn on LED when timer reaches 0
	}
	else
		PORTD &= ~(1<<Timer_LED);
}

/***************************************** Stop Watch Mode *****************************************/

void Stop_Watch_Mode (void)
{
	while ( !(PIND & (1<<Mode_Selector)))			// If Stop Watch mode selected ( Pin is LOW )
	{
		display();

		PORTD &= ~(1<<Timer_LED);					// Turn Off Timer LED
		/* Seconds */
		if (sec_units == 10)						// Ex: 19 seconds >> 20 seconds
		{
			++sec_tenth;
			sec_units = 0;
		}
		if (sec_tenth == 6)							// Ex: 13:59 >> 14:00
		{
			sec_tenth = sec_units = 0;
			++min_units;
		}

		/* Minutes */
		if (min_units == 10)						// Ex: 19 minutes >> 20 minutes
		{
			min_units=0;
			++min_tenth;
		}
		if (min_tenth == 6)							// Ex: 01:59:59 >> 02:00:00
		{
			min_tenth = min_units = 0;
			++hrs_units;
		}

		/* Hours */
		if (hrs_units == 10)
		{
			hrs_units=0;
			++hrs_tenth;
		}
		if (hrs_tenth == 10)
		{
			hrs_tenth = hrs_units = min_tenth = min_units = sec_tenth = sec_units = 0;
		}
	}
}
/***************************************** Timer Mode *****************************************/

void Timer_Mode()
{


	while ( PIND & (1<<Mode_Selector))			// If Timer mode selected ( Pin is HIGH )
	{
		display();

		if (0 == hrs_tenth && 0 == hrs_units && 0 == min_tenth && 0 == min_units && 0 == sec_tenth && 0 == sec_units)
		{
			hrs_tenth = hrs_units = min_tenth = min_units = sec_tenth = sec_units = 0;
			TCCR1B = 0;
		}

		else
		{
			if (sec_units == 255 && sec_tenth == 0 && min_units == 0 && min_tenth == 0 && hrs_units != 0 && hrs_tenth != 0)
			{
				--hrs_units;
				sec_units = 9;	sec_tenth = 5; min_units = 9; min_tenth = 5;
			}

			else if (sec_units == 255 && sec_tenth == 0 && min_units == 0 && min_tenth == 0 && hrs_units == 0 && hrs_tenth != 0)
			{
				--hrs_tenth;
				sec_units = 9;	sec_tenth = 5; min_units = 9; min_tenth = 5; hrs_units = 9;
			}

			else if (sec_units == 255 && sec_tenth == 0 && min_units == 0 && min_tenth == 0 && hrs_units != 0 && hrs_tenth == 0)
			{
				--hrs_units;
				sec_units = 9;	sec_tenth = 5; min_units = 9; min_tenth = 5;
			}

			else if (sec_units == 255 && sec_tenth == 0 && min_units != 0 && min_tenth != 0)
			{
				--min_units;
				sec_units = 9;	sec_tenth = 5;
			}
			else if (sec_units == 255 && sec_tenth == 0 && min_units == 0 && min_tenth != 0)
			{
				--min_tenth;
				sec_units = 9;	sec_tenth = 5;	min_units = 9;
			}

			else if (sec_units == 255 && sec_tenth == 0 && min_units != 0)
			{
				--min_units;
				sec_units = 9;	sec_tenth = 5;
			}
			else if (sec_units == 255 && sec_tenth == 0)
			{
				sec_units = 9;
			}
			else if (sec_units == 255 && sec_tenth != 0)
			{
				--sec_tenth;
				sec_units = 9;
			}
		}

		if (PINB & (1<<Time_Selector))
		{
			_delay_ms(40);
			if (PINB & (1<<Time_Selector))
			{
				while (PINB & (1<<Time_Selector)) {display();}
				++selector;
				Mode_Selection_Trial();
			}
		}
	}
}

int main(void)
{
	DDRC |= 0x0F;					// First four pins of PORTC are output ( for 7447 )
	DDRA |= 0x3F;					// First six pins of PORTA are output ( Refresh all 7 segments )
	PORTC &= 0xF0;					//	All 7-Segments are initialized to 0

	DDRD &= ~(1<<Mode_Selector); 	// Timer / Stop Watch Selector ( Button = 0 >> Stop Watch , Button = 1 >> Timer )
	DDRD |= (1<<Timer_LED); 		// LED to indicate time is finished ( Timer Mode )

	DDRB  &= ~(1<<Time_Selector) & ~(1<<Timer_Inc) & ~(1<<Timer_Dec);			// 3 Buttons for Timer Mode
	DDRB  |= (1<<Seconds_LED) | (1<<Minutes_LED) | (1<<Hours_LED);				// 3 LED for Time Selection
	PORTB &= ~(1<<Seconds_LED) & ~(1<<Minutes_LED) & ~(1<<Hours_LED);			// 3 LEDs Initially OFF
	/******************************************* Initializations *******************************************/

	sei();							// Set Global interrupt on
	Timer1_Init();
	Ext_INT0_Reset();				// Reset Button Interrupt
	Ext_INT1_Pause();				// Pause Button Interrupt
	Ext_INT2_Resume();				// Resume Button Interrupt

	for(;;)
	{

		/***************************************** Stop Watch Mode *****************************************/

		if (!(PIND & (1<<Mode_Selector)))				// If Stop Watch mode selected ( Pin is LOW )
		{
			Stop_Watch_Mode();
		}

		/***************************************** Timer Mode *****************************************/

		else											// If Timer mode Selected ( Pin is High )
		{
			TCCR1B = 0;									// Timer Off until Start button is pressed
			Timer_Mode();
		}
	}
}
