 /*********************************************
 Author: 		Lyndon  Walker
 Description: 	Plays back a precorded audio clip 
  
 Chip type          : ATtiny85
 Clock frequency    : 8.0 MHz default
 I/O				: Switch input 
					:
 Initial revision:  : 2/4/2014 
 
*********************************************/
/* Includes */
#include <avr/io.h>
#include <util/delay.h>

#define TRIGGER_BIT 2
#define LED_BIT 1
#define SOLENOID_BIT 0

#define BOOT_CYCLE_TIME 15 * 1000
#define BOOT_CYCLES 10
#define SOLENOID_TIME 1200000

int main(void)
{
    const double solenoidTime = SOLENOID_TIME;
    DDRB= (1 << LED_BIT);
    PORTB= 1 << TRIGGER_BIT;
    
    while (PINB & (1 << TRIGGER_BIT))
    {
        _delay_ms(10);
    }
    
    // Start sequence
    int i=0;
    for (i=0; i < BOOT_CYCLES; i++)
    {
        // On
        PORTB |= (1 << LED_BIT);
        _delay_ms(BOOT_CYCLE_TIME);
        
        // Off
        PORTB &= ~(1 << LED_BIT);
        _delay_ms(BOOT_CYCLE_TIME);
    }
    // Now on for long time
    PORTB |= (1 << LED_BIT);
    _delay_ms(solenoidTime);
    PORTB &= ~(1 << LED_BIT);
    
    // On solenoid
    PORTB = (1 << SOLENOID_BIT);
    _delay_ms(solenoidTime);
   
    PORTB = 0;
    return 0;
}