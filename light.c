#include <avr/io.h>
#include <util/delay.h>
#include "light.h"
#include "adc.h"

#define LIGHT_CHANNEL 0  // PC0
#define THRESHOLD 10
#define LIGHT_BUTTON (1 << PD4)

void light_init(void)
{
    // Initialize tri-state as high
    PORTD |= LIGHT_BUTTON;
    DDRD |= LIGHT_BUTTON;

    adc_init();
}

unsigned char light_sample(void)
{
    return adc_sample(LIGHT_CHANNEL);
}

unsigned char check_light(void)
{
    if (light_sample() < THRESHOLD)
        return 0;
    return 1;
}

void light_push_button(void)
{
    // Enable tri-state buffer to let GND through
    PORTD &= ~LIGHT_BUTTON;
    _delay_ms(250);
    // Disable tri-state
    PORTD |= LIGHT_BUTTON;
}
