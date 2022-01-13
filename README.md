# Atmega-Stopwatch-and-Timer
Stopwatch or Timer Modes chosen to be display on 6 Multiplexed 7-segments 

This project has two modes ( Timer and Stop Watch ) coded on Atmega16 AVR microcontroller. If you select the stop watch mode the time keeps incrementing every second. If you choose timer mode you can use time selector
button to select your time ( first when you are selecting seconds an LED for seconds will turn on indicating that you can increment or decrement seconds ). Same is applied on
the minutes and hours. After you select time turn off Selection Mode ( No selection LEDS should be on ). You can now start timer to count down and when time is over a LED turns on
as an inication of time out.

Also there are 3 External Interrupts that can be used any time ( Reset / Pause / Resume ).

![Screenshot Proteus](https://user-images.githubusercontent.com/63435727/149385882-3b3b445c-aa84-4ef0-a1a7-4d4c16edb75d.png)
