#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sonar.h"

#define SONAR_IN_LEFT (1<<PD6)
#define SONAR_IN_RIGHT (1<<PD7)
#define SONAR_OUT_LEFT (1<<PB0)
#define SONAR_OUT_RIGHT (1<<PB7)
// #define SONAR_THRESHOLD 21381   // Threshold of 4m
#define SONAR_THRESHOLD 535   // Threshold of ~10cm

volatile unsigned char start = 0;
volatile unsigned char sonar_send = 1;
volatile unsigned char received = 0;
volatile unsigned short sonar_timer_left = -1;
volatile unsigned short sonar_timer_right = -1;
volatile unsigned char sonar_is_left = 0;

void sonar_init(void)
{
    // Set up out pins
    DDRB |= (SONAR_OUT_LEFT | SONAR_OUT_RIGHT);
    // Set up pin change interrupts
    PCICR |= (1<<PCIE2);        // PIND
    PCMSK2 |= (1<<PCINT22);      // PD6
    PCMSK2 |= (1<<PCINT23);      // PD7

    // Set up timer
    TCCR1B |= (1 << WGM12);     // Set for CTC mode.  OCR1A = modulus
    TIMSK1 |= (1 << OCIE1A);    // Enable CTC interrupt
    OCR1A = 55296;              // Set the counter modulus for 60ms
    TCCR1B |= (1 << CS11);      // Set prescaler for divide by 8

    sei();      // Enable global interrupt
}

void sonar_send_signal(void)
{
    if (sonar_send)
    {
        sonar_send = 0;
        sonar_is_left ^= 1;

        // Send capture signal
        PORTB |= (sonar_is_left) ? SONAR_OUT_LEFT : SONAR_OUT_RIGHT;
        _delay_us(10);
        PORTB &= (sonar_is_left) ? ~SONAR_OUT_LEFT : ~SONAR_OUT_RIGHT;
    }
}

// For debugging only
unsigned int sonar_get_range(unsigned char left)
{
    unsigned short sonar_timer = left ? sonar_timer_left : sonar_timer_right;

    // unsigned int res = 1000000 * 8 / 7372800 * sonar_timer / 58;
    // return res;

    return sonar_timer;
}

// Retrieving simply takes the most updated value from one of the two sensors
// Return 0 if the range on the given side is more than the threshold
// Return 1 if the range is less than the threshold
unsigned char sonar_is_close(unsigned char left)
{
    unsigned short sonar_timer = left ? sonar_timer_left : sonar_timer_right;
    if (sonar_timer < SONAR_THRESHOLD)
        return 1;
    else
        return 0;
}

// Pin interrupt for receiving range data
ISR(PCINT2_vect)
{
    unsigned char pin = SONAR_IN_LEFT;
    if (!sonar_is_left)
    {
        pin = SONAR_IN_RIGHT;
    }

    if ((PIND & pin) && !start)
    {
        start = 1;
        // Reset timer value
        TCNT1 = 0;
    }
    else if (!(PIND & pin) && start)
    {
        // Negedge detected
        start = 0;
        received = 1;

        // Get timer value
        if (sonar_is_left)
            sonar_timer_left = TCNT1;
        else
            sonar_timer_right = TCNT1;
    }
}

// Sonar keeps sending measure signal every 60ms cycle
// alternating between the two sensors
ISR(TIMER1_COMPA_vect)
{
    // Whenever 60ms is reached, reset and send a new capture signal
    if (!received)
    {
        // If range not received, update the value to the max value
        if (sonar_is_left)
            sonar_timer_left = -1;
        else
            sonar_timer_right = -1;
    }
    received = 0;
    start = 0;
    TCNT1 = 0;
    sonar_send = 1;
    sonar_send_signal();
}
