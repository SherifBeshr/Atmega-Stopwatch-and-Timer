/*
 * main.h
 *
 *  Created on: Jan 12, 2022
 *      Author: Sherif
 */

#ifndef MAIN_H_
#define MAIN_H_


#define Seconds_LED		 PB0
#define Minutes_LED		 PB1
#define Hours_LED		 PB3

#define RESUME		 	 PB2
#define Time_Selector 	 PB5
#define Timer_Inc	 	 PB6
#define Timer_Dec	 	 PB7

#define RESET		 	 PD2
#define Pause		  	 PD3
#define Timer_LED		 PD6
#define Mode_Selector	 PD7

void display();
void Seconds_Selector();
void Minutes_Selector();
void Hours_Selector();
#endif /* MAIN_H_ */
