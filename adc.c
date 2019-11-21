#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
    ADMUX |= (1 << REFS0 ); // Set reference to AVCC
    ADMUX &= ~(1 << REFS1 );
    ADMUX |= (1 << ADLAR ); // Left adjust the output

    ADCSRA |= (7 << ADPS0 ); // Set the prescalar to 128
    ADCSRA |= (1 << ADEN ); // Enable the ADC
}

unsigned char adc_sample(unsigned char channel)
{
    ADMUX |= (channel << MUX0 ); // Select the channel
    ADMUX &= (0xf0 | (channel << MUX0 ));

    ADCSRA |= (1 << ADSC ); // Start a conversion
    while ( ADCSRA & (1 << ADSC )); // wait for conversion complete
    return ADCH ; // Get converted value
}
