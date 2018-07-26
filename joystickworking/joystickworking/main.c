/*
 * joystickworking.c
 *
 * Created: 6/6/18 11:36:50 PM
 * Author : alicenguyen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/delay.h>
#include "bit.h"
#include "timer.h"
#include "io.c"
#include "io.h"

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADSCRA: ADC status and control register A
	// ADEN: Writing '1' to ADEN enables ADC
	// ADSC: Analog-digital start conversion: ADEN must be enabled first or at the same time
	//		 Writing '1' to this starts conversion, hardware sets back to '0'
	// ADATE:AD Auto-Trigger enable. ADC will start conversion automatically on a positive edge of trigger signal
}

void jTick()
{
	ADMUX = 1;
	static unsigned stickInput;
	 stickInput= ADC;
	
	//DELAY?
	_delay_ms(20);	

	/*/if(stickInput < 100 && ADMUX == 0){ //pushed left
				
		LCD_DisplayString(1, "PUSHED LEFT");
	}
	else */if(stickInput > 0 && stickInput < 100 && ADMUX == 1){ //pushed down
				
		LCD_DisplayString(1, "PUSHED DOWN");

	}
	/*else if(stickInput > 900 && ADMUX == 0){ //pushed right
				
		LCD_DisplayString(1, "PUSHED RIGHT");

	}*/
	else if(stickInput > 900 && ADMUX == 1){ //pushed up
				
		LCD_DisplayString(1, "PUSHED UP");

	}
	else
	{
		LCD_DisplayString(1, "NO MOVEMENT");
	}
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;

	ADC_init();

	LCD_init();

	TimerSet(1000);
	TimerOn();
	
	unsigned char count = 0;
	
    /* Replace with your application code */
    while (1) 
    {
		if(count == 0)
		{
			LCD_DisplayString(1, " settings | menu            ");
			LCD_WriteData('|');
			LCD_Cursor(33);
			count++;
		}
		else if(count == 1)
		{
			LCD_DisplayString(1, " settings | menu      ");
			LCD_WriteData('|');
			LCD_Cursor(33);
			count++;
		}
		else if(count == 2)
		{
			count = 0;
		}	
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

