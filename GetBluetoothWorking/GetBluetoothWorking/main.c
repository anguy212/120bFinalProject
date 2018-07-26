#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "bit.h"
#include "timer.h"
#include "io.c"
#include "io.h"
#include "usart.h"

 #define FOSC 1843200 // Clock Speed
 #define BAUD 9600
 #define MYUBRR FOSC/16/BAUD-1
 
//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a%b;
        if(c==0){return b;}
        a = b;
        b = c;
    }
    return 0;
}
void clearArray(unsigned char *a, unsigned char size)
{
	for(int i = 0; i < size; i++)
	{
		a[i] = 0;
	}
}
//--------End find GCD function ----------------------------------------------

// Keypad input function
unsigned char GetKeypadKey() {
    
    PORTC = 0xEF; // Enable col 4 with 0, disable others with 1?s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('1'); }
    if (GetBit(PINC,1)==0) { return('4'); }
    if (GetBit(PINC,2)==0) { return('7'); }
    if (GetBit(PINC,3)==0) { return('*'); }
    
    // Check keys in col 2
    PORTC = 0xDF; // Enable col 5 with 0, disable others with 1?s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('2'); }
    if (GetBit(PINC,1)==0) { return('5'); }
    if (GetBit(PINC,2)==0) { return('8'); }
    if (GetBit(PINC,3)==0) { return('0'); }
    // ... *****FINISH*****
    
    // Check keys in col 3
    PORTC = 0xBF; // Enable col 6 with 0, disable others with 1?s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    // ... *****FINISH*****
    if (GetBit(PINC,0)==0) { return('3'); }
    if (GetBit(PINC,1)==0) { return('6'); }
    if (GetBit(PINC,2)==0) { return('9'); }
    if (GetBit(PINC,3)==0) { return('#'); }
    
    // Check keys in col 4
    // ... *****FINISH*****
    PORTC = 0x7F;   // Enables column 7 with 0, disable all others with 1
    asm("nop"); // Adds delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('A'); }
    if (GetBit(PINC,1)==0) { return('B'); }
    if (GetBit(PINC,2)==0) { return('C'); }
    if (GetBit(PINC,3)==0) { return('D'); }
    
    return('\0'); // default value
    
}


//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
    /*Tasks should have members that include: state, period,
     a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;

unsigned char changetoInteger(unsigned char var)
{
	switch(var)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		default: return 0;
	}
}
char changetoChar(unsigned char num)
{
	switch(num)
	{
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		default: return '\0';

	}
}
unsigned char findHowManyB4Dec(unsigned char num)
{
	if(num >= 100)
	{
		return 3;
	}
	else if(num >= 10)
	{
		return 2;
	}
	else if(num >= 1)
	{
		return 1;
	}
}

//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states
int main()
{
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	// . . . etc
	
	// Period for the tasks
	
	// Set the timer and turn it on
	//TimerSet(200);
	//TimerOn();
	
	initUSART();
	
	LCD_init();
	LCD_ClearScreen();
	unsigned char NameOfDevice[16] = {};
	
	while(1) {

		USART_Send('A');
		USART_Send('T');
		USART_Send('+');
		USART_Send('N');
		USART_Send('A');
		USART_Send('M');
		USART_Send('E');
		USART_Send('?');
		
		for(int i = 0; i < 16; i++)
		{
			NameOfDevice[i] = USART_Receive();
			LCD_WriteData(NameOfDevice[i]);
		}
	}
	
	// Error: Program should not exit!
	return 0;
}

