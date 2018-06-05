/* Alice Nguyen & anguy212@ucr.edu
 * Lab section 23
 * Assignment: Lab 10, Exercise 3
 * 
 * I acknowledge all content contained herein, excluding template and example code,
 * is my own original work
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "bit.h"
#include "timer.h"
#include "io.c"
#include "io.h"

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

void clearArray(unsigned char *a, unsigned char size)
{
	for(int i = 0; i < size; i++)
	{
		a[i] = 0;
	}
}

// Keypad input function
unsigned char GetKeypadKey() {
    
    PORTC = 0xEF; // Enable col 4 with 0, disable others with 1?s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('1'); }
    if (GetBit(PINC,1)==0) { return('4'); }
    if (GetBit(PINC,2)==0) { return('7'); }
    if (GetBit(PINC,3)==0) { return('.'); }
    
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

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------
unsigned char pos, key, key2;
	
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum MenuStates{startM, pauseM, transaction, settings};
enum TransStates{startT, pauseT, options, options1, picked, picked1, payment, payment1, change, change1};
enum SettingStates{startS, pauseS, opt, codeSet, codeWrite, pricePick, priceSet, priceWrite, done};
enum Keypad500states{startK, wait, press, unpress};
enum LockStates{startL, pauseL, enterPin, checkPin};

int Lock(int state)
{
	static unsigned char count;
	static unsigned char numbers[7] = {0};
	static int address;
	static unsigned char holder;
	
	switch(state)
	{
		case startL:
		{
			pos = 3;
			if(eeprom_read_byte((uint8_t *) 26) != '1')
			{
				eeprom_write_byte((uint8_t *) 20, '0');
				eeprom_write_byte((uint8_t *) 21, '0');
				eeprom_write_byte((uint8_t *) 22, '0');
				eeprom_write_byte((uint8_t *) 23, '0');
			}
			state = pauseL;
			break;
		}
		case pauseL:
		{
			count = 0;
			address = 20;
			holder = 0;
			clearArray(numbers, 7);
			if(pos == 3)
			{
				state = enterPin;
				LCD_DisplayString(1, "enter pin: ");
				//LCD_WriteData(eeprom_read_byte((uint8_t *) 20));
				//LCD_WriteData(eeprom_read_byte((uint8_t *) 21));
				//LCD_WriteData(eeprom_read_byte((uint8_t *) 22));
				//LCD_WriteData(eeprom_read_byte((uint8_t *) 23));
			}
			break;
		}
		case enterPin:
		{
			if(key == '#')
			{
				state = checkPin;
			}
			break;
		}
		case checkPin:
		{
			state = pauseL;
			break;
		}
		default:
		{
			state = startL;
			break;
		}
	}
	switch(state)
	{
		case startL:
		{
			break;
		}
		case pauseL:
		{
			break;
		}
		case enterPin:
		{
			key = GetKeypadKey();
			if(key != '\0')
			{
				LCD_WriteData(key);
				numbers[count] = key;
				count++;
			}
			break;
		}
		case checkPin:
		{
			holder = 0;
			address = 20;
			LCD_WriteData(' ');
			for(int i = 0; i < count-1 ; i++)
			{
				//LCD_WriteData(numbers[i]);
				//LCD_WriteData(eeprom_read_byte((uint8_t *) address));
				if(eeprom_read_byte((uint8_t *) address) != numbers[i])
				{
					holder = 1;
					//LCD_WriteData(numbers[i]);
				}
				address++;
			}
			
			if(holder == 0)
			{
				pos = 1;
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
	return state;
}

int Menu(int state)
{
	switch(state)
	{
		case startM:
		{
			//pos = 3;
			state = pauseM;
			break;
		}
		case pauseM:
		{
			if(pos == 1)
			{
				state = transaction;
			}
			break;
		}
		case transaction:
		{
			if(pos == 1)
			{
				state = settings;
			}
			else
			{
				state = pauseM;
			}
			break;
		}
		case settings:
		{
			if(pos == 1)
			{
				state = transaction;
			}
			else
			{
				state = pauseM;
			}
			break;
		}
		default:
		{
			state = startM;
			break;
		}
	}
	switch(state)
	{
		case startM:
		{
			break;
		}
		case pauseM:
		{
			break;
		}
		case transaction:
		{
			LCD_DisplayString(1, "A. Transactions");
			if(key2 == 'A')
			{
				pos = 2;
			}
			else if(key2 == 'B')
			{
				pos = 4;
			}
			break;
		}
		case settings:
		{
			LCD_DisplayString(1, "B. Settings");
			if(key2 == 'A')
			{
				pos = 2;
			}
			else if(key2 == 'B')
			{
				pos = 4;
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return state;
}


int trans(int state)
{
	static float totalMoney;
	static float changeMoney;
	static float holderMoney;
	static unsigned char number[7] = {0};
	static unsigned char place;
	static unsigned char count;
	static unsigned char holder, holder2, holder3, num1, num2;
	static int decval, intval;
	
	switch (state)
	{
		case startT:
		{
			state = pauseT;
			break;
		}
		case pauseT:
		{
			totalMoney = 0;
			changeMoney = 0;
			holderMoney = 0;
			place = 0;
			count = 0;
			holder = 0;
			holder2 = 0;
			holder3 = 0;
			num1 = 0;
			num2 = 0;
			clearArray(number, 7);
			
			if(pos == 2)
			{
				state = options;
			}
			break;
		}
		case options:
		{
			state = options1;
			break;
		}
		case options1:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = picked;
				totalMoney += eeprom_read_float((float *)1);
			}
			else if(key == 'B')
			{
				state = picked;
				totalMoney += eeprom_read_float((float *)10);
			}
			break;
		}
		case picked:
		{
			state = picked1;
			break;
		}
		case picked1:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = payment;
			}
			else if(key == 'B')
			{
				state = options;
			}
			break;
		}
		case payment:
		{
			state = payment1;
			break;
		}
		case payment1:
		{
			if(key == '#')
			{
				state = change;
			}
			break;
		}
		case change:
		{
			state = change1;
		}
		case change1:
		{
			key = GetKeypadKey();
			if(key == '#')
			{
				pos = 3;
				state = pauseT;
			}
			break;
		}
		default:
		{
			state = startT;
			break;
		}
	}
	switch (state)
	{
		case startT:
		{
			break;
		}
		case pauseT:
		{
			break;
		}
		case options:
		{
			LCD_DisplayString(1, "A       B           ");
			intval = (int)holderMoney;
			holderMoney = (holderMoney - (int)holderMoney) * 100;
			decval = (int)holderMoney;
			
			num1 = (unsigned char)intval;
			num2 = (unsigned char)decval;
			
			
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
			
			//writing $$ of B
			holderMoney = eeprom_read_float((float *)10);
			
			intval = (int)holderMoney;
			holderMoney = (holderMoney - (int)holderMoney) * 100;
			decval = (int)holderMoney;
			
			num1 = (unsigned char)intval;
			num2 = (unsigned char)decval;
			
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
			
			if(num1 >= 99)
			{
				num1++;
			}
			place = findHowManyB4Dec(num1);
			
			holder2 = place - 1;
			
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
			state = pricePick;
			break;
		}
		case options1:
		{
			break;
		}
		case picked:
		{
			LCD_DisplayString(1, "Done  |  AddItem A    |    B");
			break;
		}
		case picked1:
		{
			break;
		}
		case payment:
		{
			LCD_DisplayString(1, "Enter Payment:   ");
			break;
		}
		case payment1:
		{
			key = GetKeypadKey();
			if(key !=  '\0' && key != 'A' && key != 'B' && key!= 'C' && key != 'D')
			{
				LCD_WriteData(key);
				if (key == '.')
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
		case change:
		{
			LCD_DisplayString(1, "change:          ");
			for(int i = 0; i < place; i++)
			{
				LCD_WriteData(number[i]);
				holder = changetoInteger(number[i]);
				num1 = holder*pow(10, holder2) + num1;
				holder2--;
			}
			LCD_WriteData('.');
			holder2 = 1;
			for(int j = place+1; j < count-1; j++)
			{	
				LCD_WriteData(number[j]);
				holder = changetoInteger(number[j]);
				num2 = holder*pow(10, holder2) + num2;
				holder2--;
			}
			
			holderMoney = (float)num1 + ((float)num2/100);
			changeMoney = holderMoney - totalMoney;
			holderMoney = (changeMoney - (int)changeMoney) * 100;
			decval = (int)holderMoney;
			intval = (int)changeMoney;
			
			num1 = (unsigned char)intval;
			num2 = (unsigned char)decval;
			
			LCD_WriteData(' ');
			
			
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
		case change1:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	return state;
}

int setting(int state)
{
	static float holderMoney;
	static unsigned char place, count, whichAddr;
	static unsigned char holder, holder2, holder3, num1, num2;
	static int decval, intval;
	static unsigned char number[7] = {0};

	
	switch(state)
	{
		case startS:
		{
			state = pauseS;
			break;
		}
		case pauseS:
		{
			holderMoney = 0;
			place = 0;
			count = 0;
			holder = 0;
			holder2 = 0;
			whichAddr = 0;
			holder3 = 0;
			num1 = 0;
			num2 = 0;
			clearArray(number, 7);
			if(pos == 4)
			{
				state = opt;
				LCD_DisplayString(1, "  A    |   B      code |   items");
			}
			break;
		}
		case opt:
		{
			key = GetKeypadKey();
			if (key == 'A')
			{
				state = codeSet;
				LCD_DisplayString(1, "enter old code:  ");
			}
			else if(key == 'B')
			{
				LCD_DisplayString(1, "A     B           ");
				
				//writing $$ of A
				holderMoney = eeprom_read_float((float *)1);
				
				intval = (int)holderMoney;
				holderMoney = (holderMoney - (int)holderMoney) * 100;
				decval = (int)holderMoney;
				
				num1 = (unsigned char)intval;
				num2 = (unsigned char)decval;
								
				
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
				
				//writing $$ of B
				holderMoney = eeprom_read_float((float *)10);
				
				intval = (int)holderMoney;
				holderMoney = (holderMoney - (int)holderMoney) * 100;
				decval = (int)holderMoney;
				
				num1 = (unsigned char)intval;
				num2 = (unsigned char)decval;
				
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				
				
				if(num1 >= 99)
				{
					num1++;
				}
				place = findHowManyB4Dec(num1);
				
				holder2 = place - 1;
				
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
				state = pricePick;
			}
			break;
		}
		case codeSet:
		{
			if (key == '#')
			{
				state = codeWrite;
				intval = 20;
			}
			break;
		}
		case codeWrite:
		{
			state = done;
			LCD_DisplayString(1, "A        |   #  settings | menu");
			break;
		}
		case pricePick:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = priceSet;
				whichAddr = 1;
				LCD_DisplayString(1, "Enter Price:     ");
				//variable set to indicate A chosen;
			}
			else if(key == 'B')
			{
				state = priceSet;
				whichAddr = 2;
				LCD_DisplayString(1, "Enter Price:     ");
				//variable set to indicate B chosen;
			}
			break;
		}
		case priceSet:
		{
			if (key == '#')
			{
				state = priceWrite;
			}
			break;
		}
		case priceWrite:
		{
			LCD_DisplayString(1, "A        |   #  settings | menu");	
			state = done;
			break;
		}
		case done:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = opt;
			}
			else if(key == '#')
			{
				pos = 3;
				state = pauseS;
			}
			break;
		}
		default:
		{
			state = startS;
			break;
		}
	}	
	switch(state)
	{
		case startS:
		{
			break;
		}
		case pauseS:
		{
			break;
		}
		case opt:
		{
			break;
		}
		case codeSet:
		{
			key = GetKeypadKey();
			if(key !=  '\0')
			{
				LCD_WriteData(key);
				number[count] = key;
				count++;
			}
			break;
		}
		case codeWrite:
		{
			eeprom_write_byte(((uint8_t *) 26), '1');
			for(int i = 0; i < count ; i++)
			{
				eeprom_write_byte(((uint8_t *) intval), number[i]);
				//LCD_WriteData(eeprom_read_byte((uint8_t *) intval));
				intval++;
			}
			break;
		}
		case pricePick:
		{
			break;
		}
		case priceSet:
		{
			key = GetKeypadKey();
			if(key !=  '\0' && key != 'A' && key != 'B' && key!= 'C' && key != 'D')
			{
				LCD_WriteData(key);
				if (key == '.')
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
		case priceWrite:
		{
			//write to corresponding eeprom price
			holder2 = place - 1;
			holder = 0;
			LCD_DisplayString(1, "change:          ");
			for(int i = 0; i < place; i++)
			{
				//LCD_WriteData(number[i]);
				holder = changetoInteger(number[i]);
				num1 = holder*pow(10, holder2) + num1;
				holder2--;
			}
			//LCD_WriteData('.');
			holder2 = 1;
			for(int j = place+1; j < count-1; j++)
			{
				//LCD_WriteData(number[j]);
				holder = changetoInteger(number[j]);
				num2 = holder*pow(10, holder2) + num2;
				holder2--;
			}
			
			holderMoney = (float)num1 + ((float)num2/100);
			if(whichAddr == 1)
			{
				eeprom_write_float((float *)1, holderMoney);
			}	
			else if(whichAddr == 2)
			{
				eeprom_write_float((float *)10, holderMoney);
			}	
			break;
		}
		case done:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	return state;
}

unsigned char count, x, y;
int keypad(int state)
{
	switch(state)
	{
		case startK:
		{
			count = 0;
			state = wait;
			break;
		}
		case wait:
		{
			x = GetKeypadKey();
			if(x != '\0')
			{
				state = press;
			}
			if(count == 9)
			{
				state = startK;
			}
			break;
		}
		case press:
		{
			y = GetKeypadKey();
			if(y == '\0')
			{
				state = unpress;
			}
			if(count == 9)
			{
				state = startK;
			}
			break;
		}
		case unpress:
		{
			key2 = x;
			if(count == 9)
			{
				state = startK;
			}
			break;
		}
		default:
		{
			state = startK;
			break;
		}
	}
	switch(state)
	{
		case startK:
		{
			break;
		}
		case wait:
		{
			count++;
			break;
		}
		case press:
		{
			count++;
			break;
		}
		case unpress:
		{
			count++;
			break;
		}
		default:
		{
			break;
		}
	}
	return state;
}
	
int main()
{
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	// . . . etc
	
	// Period for the tasks
	unsigned long int lockperiod = 300;
	unsigned long int Menuperiod = 1000;
	unsigned long int transperiod = 200;
	unsigned long int settingperiod = 200;
	unsigned long int keypadperiod = 100;

	
	//Calculating GCD
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick0_period = lockperiod/100;
	unsigned long int SMTick1_period = Menuperiod/100;
	unsigned long int SMTick2_period = transperiod/100;
	unsigned long int SMTick3_period = settingperiod/100;
	unsigned long int SMTick5_period = keypadperiod/100;

	
	//Declare an array of tasks
	static task j,a,b,c,e;
	task *tasks[] = {&j, &a, &b, &c, &e};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	j.state = startL;//Task initial state.
	j.period = SMTick0_period;//Task Period.
	j.elapsedTime = SMTick0_period;//Task current elapsed time.
	j.TickFct = &Lock;//Function pointer for the tick.
	
	a.state = startM;//Task initial state.
	a.period = SMTick1_period;//Task Period.
	a.elapsedTime = SMTick1_period;//Task current elapsed time.
	a.TickFct = &Menu;//Function pointer for the tick.
	
	// Task 2
	b.state = startT;//Task initial state.
	b.period = SMTick2_period;//Task Period.
	b.elapsedTime = SMTick2_period;//Task current elapsed time.
	b.TickFct = &trans;//Function pointer for the tick.
	
	c.state = startS;//Task initial state.
	c.period = SMTick3_period;//Task Period.
	c.elapsedTime = SMTick3_period;//Task current elapsed time.
	c.TickFct = &setting;//Function pointer for the tick.
	
	e.state = startK;//Task initial state.
	e.period = SMTick5_period;//Task Period.
	e.elapsedTime = SMTick5_period;//Task current elapsed time.
	e.TickFct = &keypad;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(100);
	TimerOn();
	
	LCD_init();
	LCD_ClearScreen();
	
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}