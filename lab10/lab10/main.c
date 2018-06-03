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

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------
unsigned char pos, key;
	
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum MenuStates{startM, pauseM, transaction, settings, profits};
enum TransStates{startT, pauseT, options, options1, picked, picked1, payment, payment1, change, change1};
enum ProfStates{startP, pauseP, display, display1};
enum SettingStates{startS, pauseS, opt, opt1, priceSet, priceSet1, passSet, passSet1, enterPas, enterPas1, enterPrice, enterPrice1, done, done1};
enum Keypad500states{startK, wait, press, unpress};


int Menu(int state)
{
	switch(state)
	{
		case startM:
		{
			pos = 1;
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
				state = profits;
			}
			else
			{
				state = pauseM;
			}
			break;
		}
		case profits:
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
			if(key == 'A')
			{
				pos = 2;
			}
			else if(key == 'B')
			{
				pos = 4;
			}
			else if(key == 'C')
			{
				pos = 3;
			}
			break;
		}
		case settings:
		{
			LCD_DisplayString(1, "B. Settings");
			if(key == 'A')
			{
				pos = 2;
			}
			else if(key == 'B')
			{
				pos = 4;
			}
			else if(key == 'C')
			{
				pos = 3;
			}
			break;
		}
		case profits:
		{
			LCD_DisplayString(1, "C. Profits");
			if(key == 'A')
			{
				pos = 2;
			}
			else if(key == 'B')
			{
				pos = 4;
			}
			else if(key == 'C')
			{
				pos = 3;
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
	switch (state)
	{
		case startT:
		{
			state = pauseT;
			break;
		}
		case pauseT:
		{
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
			}
			else if(key == 'B')
			{
				state = picked;
			}
			else if(key == 'C')
			{
				state = picked;
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
				pos = 1;
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
			LCD_DisplayString(1, "A     B    C    $     $    $");
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
			if(key != '\0' && key != '#')
			{
				LCD_WriteData(key);
			}
			break;
		}
		case change:
		{
			LCD_DisplayString(1, "total-pay=change");
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

int prof(int state)
{
	switch(state)
	{
		case startP:
		{
			state = pauseP;
			break;
		}
		case pauseP:
		{
			if(pos == 3)
			{
				state = display;
			}
			break;
		}
		case display:
		{
			state = display1;
			break;
		}
		case display1:
		{
			key = GetKeypadKey();
			if(key == '#')
			{
				state = pauseP;
				pos = 1;
			}
			break;
		}
		default:
		{
			state = startP;
			break;
		}
	}
	switch(state)
	{
		case startP:
		{
			break;
		}
		case pauseP:
		{
			break;
		}
		case display:
		{
			LCD_DisplayString(1, "Total Profits: ");
			break;
		}
		case display1:
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
	switch(state)
	{
		case startS:
		{
			state = pauseP;
			break;
		}
		case pauseS:
		{
			if(pos == 4)
			{
				state = opt;
			}
			break;
		}
		case opt:
		{
			state = opt1;
			break;
		}
		case opt1:
		{
			key = GetKeypadKey();
			if (key == 'A')
			{
				state = passSet;
			}
			else if(key == 'B')
			{
				state = priceSet;
			}
			break;
		}
		case passSet:
		{
			state = passSet1;
			break;
		}
		case passSet1:
		{
			if (key == '#')
			{
				state = enterPas;
			}
			break;
		}
		case priceSet:
		{
			state = priceSet1;
			break;
		}
		case priceSet1:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = enterPrice;
			}
			else if(key == 'B')
			{
				state = enterPrice;
			}
			else if(key == 'C')
			{
				state = enterPrice;
			}
			break;
		}
		case enterPas:
		{
			state = enterPas1;
			break;
		}
		case enterPas1:
		{
			if (key == '#')
			{
				state = done;
			}
			break;
		}
		case enterPrice:
		{
			state = enterPrice1;
			break;
		}
		case enterPrice1:
		{
			if(key == '#')
			{
				state = done;
			}
			break;
		}
		case done:
		{
			state = done1;
			break;
		}
		case done1:
		{
			key = GetKeypadKey();
			if(key == 'A')
			{
				state = opt;
			}
			else if(key == '#')
			{
				pos = 1;
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
			LCD_DisplayString(1, "  A    |   B      code |   items");
			break;
		}
		case opt1:
		{
			break;
		}
		case passSet:
		{
			LCD_DisplayString(1, "enter old code:  ");
			break;
		}
		case passSet1:
		{
			key = GetKeypadKey();
			if(key != '\0' && key != '#')
			{
				LCD_WriteData(key);
			}
			break;
		}
		case enterPas:
		{
			LCD_DisplayString(1,"set new code:    ");
			break;
		}
		case enterPas1:
		{
			key = GetKeypadKey();
			if(key != '\0' && key != '#')
			{
				LCD_WriteData(key);
			}
			break;
		}
		case priceSet:
		{
			LCD_DisplayString(1, "A     B    C    $     $    $");
			break;
		}
		case priceSet1:
		{
			break;
		}
		case enterPrice:
		{
			LCD_DisplayString(1, "Enter Price:     ");
			break;
		}
		case enterPrice1:
		{
			key = GetKeypadKey();
			if(key != '\0' && key != '#')
			{
				LCD_WriteData(key);
			}
			break;
		}
		case done:
		{
			LCD_DisplayString(1, "A     |   #    settings | menu");
			break;
		}
		case done1:
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
			key = x;
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
	unsigned long int Menuperiod = 1000;
	unsigned long int transperiod = 200;
	unsigned long int settingperiod = 200;
	unsigned long int profitperiod = 200;
	unsigned long int keypadperiod = 100;

	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = 100;
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = 100;
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = Menuperiod/GCD;
	unsigned long int SMTick2_period = transperiod/GCD;
	unsigned long int SMTick3_period = settingperiod/GCD;
	unsigned long int SMTick4_period = profitperiod/GCD;
	unsigned long int SMTick5_period = keypadperiod/GCD;

	
	//Declare an array of tasks
	static task a,b,c,d,e;
	task *tasks[] = {&a, &b, &c, &d, &e};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
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
	
	d.state = startP;//Task initial state.
	d.period = SMTick4_period;//Task Period.
	d.elapsedTime = SMTick4_period;//Task current elapsed time.
	d.TickFct = &prof;//Function pointer for the tick.
	
	e.state = startK;//Task initial state.
	e.period = SMTick5_period;//Task Period.
	e.elapsedTime = SMTick5_period;//Task current elapsed time.
	e.TickFct = &keypad;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
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