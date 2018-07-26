/*
 * ServoMotor.c
 *
 * Created: 6/7/18 3:44:11 AM
 * Author : alicenguyen
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL								/* Define CPU Frequency e.g. here its 8MHz */
#include <avr/io.h>									/* Include AVR std. library file */
#include <stdio.h>									/* Include std. library file */
#include <util/delay.h>								/* Include Delay header file */

int main(void)
{
	DDRD |= 0xFF;								/* Make OC1A pin as output */
	TCNT1 = 0;										/* Set timer1 count zero */
	ICR1 = 2499;									/* Set TOP count for timer1 in ICR1 register */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);				/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	
	OCR1A = 175;
	
	while(1)
	{
		//OCR1A = 65;									/* Set servo shaft at -90? position */
		//_delay_ms(1000);
		//OCR1A = 175;								/* Set servo shaft at 0? position */
		//_delay_ms(1000);
		//OCR1A = 280;								/* Set servo at +90? position */
		//_delay_ms(1000);


	}
}