#include <avr/io.h>
#include "testIO.h"

#define B_IO ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB7))
#define C_IO ((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5))
#define D_IO ((1<<PD0) | (1<<PD1) | (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))

void InitializeIO(void)
{
    // Initialize B
    PORTB &= ~B_IO;
    DDRB |= B_IO;

    // Initialize C
    PORTC &= ~C_IO;
    DDRC |= C_IO;

    // Initialize D
    PORTD &= ~D_IO;
    DDRD |= D_IO;
}

void LoopIO(void)
{
    // Trigger
    PORTB |= (1<<PB0);
    PORTB &= ~(1<<PB0);

    // Check PORTB
    char mask = 1;
    // Skip trigger
    mask <<= 1;
    while (mask)
    {
        char masked = (DDRB & mask);
        if (masked)
        {
            PORTB |= masked;
            PORTB &= ~masked;
        }
        mask <<= 1;
    }

    // // Check PORTC
    mask = 1;
    // Skip trigger
    while (mask)
    {
        char masked = (DDRC & mask);
        if (masked)
        {
            PORTC |= masked;
            PORTC &= ~masked;
        }
        mask <<= 1;
    }

    // // Check PORTD
    mask = 1;
    while (mask)
    {
        char masked = (DDRD & mask);
        if (masked)
        {
            PORTD |= masked;
            PORTD &= ~masked;
        }
        mask <<= 1;
    }
}