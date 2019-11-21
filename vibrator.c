#include <avr/io.h>
#include "vibrator.h"

#define VIBRATOR_LEFT (1 << PB2)
#define VIBRATOR_RIGHT (1 << PB1)

void vibrator_init(void)
{
    DDRB |= (VIBRATOR_LEFT | VIBRATOR_RIGHT);
}

void vibrate_on(unsigned char left)
{
    if (left)
        PORTB |= VIBRATOR_LEFT;
    else
        PORTB |= VIBRATOR_RIGHT;    
}

// TODO: Think about vibration interrupts
void vibrate_off(unsigned char left)
{
    if (left)
        PORTB &= ~VIBRATOR_LEFT;
    else
        PORTB &= ~VIBRATOR_RIGHT;   
}
