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
	else
	{
		return 0;
	}
	
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

// Implement scheduler code from PES.
int main()
{
    // Set Data Direction Registers
    // Buttons PORTA[0-7], set AVR PORTA to pull down logic
    DDRA = 0xFF; PORTA = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
	unsigned char key; 
	unsigned char number[7] = {0, 0, 0, 0 , 0 , 0 , 0};
	unsigned char count = 0;
	unsigned char place = 0;
	LCD_init();
	LCD_ClearScreen();	
	    
	TimerSet(200);
	TimerOn();
	
    while(1) {
		//unsigned char num = 9;
		//LCD_WriteData(9*pow(10, 1) + 7);
        // Scheduler code
       key = GetKeypadKey();
        if(key !=  '\0' && key != '#')
		{
			LCD_WriteData(key);
			if (key == '*')
			{
				place = count;
			}
			number[count] = key;
			count++;
		}
		unsigned char num1 = 0; 
		unsigned char num2 = 0;
		unsigned char holder2 = place - 1;
		unsigned char holder = 0;
		if (key == '#')
		{
			//LCD_ClearScreen();
			for(int i = 0; i < place; i++)
			{
				//LCD_WriteData(number[i]);
				holder = changetoInteger(number[i]);
				num1 = holder*pow(10, holder2) + num1;
				holder2--;	
			}
			//LCD_WriteData('.');
			holder2 = 1;
			for(int j = place+1; j < count; j++)
			{
				//LCD_WriteData(number[j]);
				holder = changetoInteger(number[j]);
				num2 = holder*pow(10, holder2) + num2;
				holder2--;
			}
		
		double n1 = (double) num1;
		double n2 = (double) num2;
		double dec = n1 + (n2/100);		
		//dec = dec + 1.12;
		double decpart = (dec - (int)dec) * 100;
		int decVal = (int)decpart;
		int intVal = (int)dec;
		
		num1 = (unsigned char)intVal;
		num2 = (unsigned char)decVal;
				
		if(num1 >= 99)
		{
			num1++;
		}		
		place = findHowManyB4Dec(num1);
		
		holder2 = place - 1;
		
		unsigned char holder3 = 0;
	
		for(int i = 0; i < place; i++)
		{
			holder3 = num1/(pow(10,holder2));
			num1 = num1 - (holder3 * pow(10, holder2));
			LCD_WriteData(changetoChar(holder3));
			holder2--;
		}
		holder2 = 1;
		LCD_WriteData('.');
		for(int i = 0; i < 2; i++)
		{
			holder3 = num2/(pow(10,holder2));
			num2 = num2 - (holder3 * pow(10, holder2));
			LCD_WriteData(changetoChar(holder3));
			holder2--;
		}
	}
        while(!TimerFlag);
        TimerFlag = 0;
    }
    // Error: Program should not exit!
    return 0;
}

