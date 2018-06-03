#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "bit.h"
#include "timer.h"
#include "io.c"
#include "io.h"
	
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
	else
	{
		return 0;
	}
	
}

//--------End Task scheduler data structure-----------------------------------
enum RDStates{start, options, write, display, display2, write1} rdstate;
unsigned char key;
unsigned char number[7] = {0, 0, 0, 0 , 0 , 0 , 0};
unsigned char count = 0;
unsigned char place = 0;
unsigned char num1 = 0;
unsigned char num2 = 0;
unsigned char holder2 = 0;
unsigned char holder = 0;
float decpart = 0;
float dec = 0;
int decVal = 0;
int intVal = 0;
//--------Shared Variables----------------------------------------------------
void ReadOrDisplay()
{
	switch(rdstate)
	{
		case start:
		{
			rdstate = options;
			LCD_DisplayString(1, "A = Write              B = Read ");
			count = 0;
			place = 0;
			num1 = 0;
			num2 = 0;
			holder2 = 0;
			holder = 0;
			decpart = 0;
			dec = 0;
			decVal = 0;
			intVal = 0;
			break;
		}
		case options:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				rdstate = write;
				LCD_DisplayString(1, "enter price:              ");
			}
			else if(key == 'B')
			{
				rdstate = display;
				LCD_DisplayString(1, "price is:              ");
			}
			break;
		}
		case write:
		{
			key = GetKeypadKey();
			if(key == '#')
			{
				rdstate = write1;
			}
			break;
		}
		case write1:
		{
			rdstate = start;
			break;
		}
		case display:
		{
			rdstate = display2;
			break;
		}
		case display2:
		{
			key = GetKeypadKey();
			if(key == '#')
			{
				rdstate = start;
			}
			break;
		}
		default:
		{
			rdstate = start;
			break;
		}
	}	
	switch(rdstate)
	{
		case start:
		{
			break;
		}
		case options:
		{
			break;
		}
		case write:
		{
			key = GetKeypadKey();
			if(key !=  '\0' && key != '#' && key != 'A' && key != 'B' && key!= 'C' && key != 'D')
			{
				LCD_WriteData(key);
				if (key == '*')
				{
					place = count;
				}
				number[count] = key;
				count++;
			}
			holder2 = place - 1;
			holder = 0;
			break;
		}
		case write1:
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
			{					//LCD_WriteData(number[j]);
				holder = changetoInteger(number[j]);
				num2 = holder*pow(10, holder2) + num2;
				holder2--;
			}
			float n1 = (float) num1;
			float n2 = (float) num2;
			float dec = n1 + (n2/100);
			eeprom_write_float(1, dec);
			break;
		}
		case display:
		{
			dec = eeprom_read_float(1);
			decpart = (dec - (int)dec) * 100;
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
			break;
		}
		case display2:
		{
			break;
		}
		default:
		{
			break;
		}
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
	TimerSet(200);
	TimerOn();
	
	LCD_init();
	LCD_ClearScreen();
	
	rdstate = start;
	while(1) {
		ReadOrDisplay();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}