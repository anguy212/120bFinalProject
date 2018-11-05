# 120b Cash Register:

## This program works with on Atmega1284 microcontroller to make a cash register. The cash register works with a lcd display, keypad, joystick, EEPROM, customizable characters, and servo motor.

* LCD Display:
  * Displays the information from the cash register such as function options, item prices, and user input.
* Keypad:
  * Takes in user input to navigate through functions.
  * Takes in user input for payment amount.
* Joystick:
  * Takes in user input to better navigate through functions.
* EEPROM:
  * Lets users program item prices so that each time the system looses power, the programmed prices are remembered.
* Customizable Characters:
  * Special Characters in ASCII table (i.e. euro sign) are available on the system.
* Servo Motor
  * A motor that works as a lock for the register.
  * The motor will stay in lock position until password and payment are entered.
  
## Faults in the design

* The Servo Motor slowed down the other state machines when added due to the Atmega’s PWM interrupts. This can be fixed by only using the PWM in necessary cases.
