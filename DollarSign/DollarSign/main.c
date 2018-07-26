#include <avr/io.h>
#include <avr/interrupt.h>
#include "bit.h"
#include "timer.h"
#include "io.c"
#include "io.h"
#include "math.h"
#include <stdio.h>

//--------Find GCD function --------------------------------------------------

//--------End find GCD function ----------------------------------------------
    
// Implement scheduler code from PES.
int main()
{
    // Set Data Direction Registers
    // Buttons PORTA[0-7], set AVR PORTA to pull down logic
    DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
	
	unsigned char eruo[] = {0x00,0x0E,0x09,0x1E,0x1E,0x09,0x0E,0x00};


	LCD_init();
	LCD_ClearScreen();	
	    
	TimerSet(1000);
	TimerOn();
	
	
    while(1) {

		//LCDcustomChar(1, eruo);

		LCD_WriteData(0x23);	
		
	
        while(!TimerFlag);
        TimerFlag = 0;
    }
    // Error: Program should not exit!
    return 0;
}
