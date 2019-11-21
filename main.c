#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "serial.h"
#include "vibrator.h"
#include "pressure.h"
#include "light.h"
#include "sonar.h"
#include "gyro.h"
#include "gps.h"

#define FOSC 7372800           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1

#define RIGHT 0
#define LEFT 1

#define HANDS_OFF (1 << PD2)
#define DOWNHILL (1 << PD3)

unsigned int slope_count = 0;
unsigned char is_downhill = 0;
unsigned char is_dark = 0;

void process_range(void);
void process_slope(void);
void process_handles(void);
void process_light(void);

int main(void)
{
    // Initialize
    DDRD |= HANDS_OFF | DOWNHILL;
    pressure_init();
    vibrator_init();
    light_init();
    sonar_init();
    gyro_init(BDIV);

    // serial_init(MYUBRR);
    // gps_init(MYUBRR);

    char buff[200];
    unsigned int count = 0;
    // char gps_d[MAXLINELENGTH];

    while (1)
    {
        gyro_compare();
        process_range();
        process_slope();
        process_handles();
        process_light();

        // gps_parse(gps_d);   // Move GPS to a separate unit

        // DEBUGGING
        // if (count > 50)
        // {
        //     count = 0;
        //     snprintf(buff, 200, "Left Pressure: %d / Right Pressure: %d\n\rLeft Range: %u / Right Range: %u\n\rLight: %d\n\r", 
        //             sample_pressure(LEFT), sample_pressure(RIGHT), 
        //             sonar_get_range(LEFT), sonar_get_range(RIGHT),
        //             light_sample());
        //     serial_string(buff);

        //     int x = 0, y = 0, z = 0;
        //     gyro_read(&x, &y, &z);
        //     snprintf(buff, 200, "Gyro:\n\rx: %d, y: %d, z: %d\n\r", x, y, z);
        //     serial_string(buff);
        // }
        // ++count;
    }

    return 0;   /* never reached */
}

void process_range(void)
{
    // Check left
    if (!sonar_is_close(LEFT))
        vibrate_off(LEFT);
    else
        vibrate_on(LEFT);
    
    // Check right
    if (!sonar_is_close(RIGHT))
        vibrate_off(RIGHT);
    else
        vibrate_on(RIGHT);
}

void process_slope(void)
{
    if (is_downhill)
    {
        if (slope_count > 100)
        {
            // serial_string("Not downhill\n\r");
            is_downhill = 0;
            slope_count = 0;
        }
        else if (!gyro_is_downhill())
            ++slope_count;
        else if (gyro_is_downhill() == 1)
            slope_count = 0;

        PORTD |= DOWNHILL;
    }
    else
    {
        if (slope_count > 100)
        {
            // serial_string("Downhill\n\r");
            is_downhill = 1;
            slope_count = 0;
        }
        else if (gyro_is_downhill() == 1)
            ++slope_count;
        else if (!gyro_is_downhill())
            slope_count = 0;

        PORTD &= ~DOWNHILL;
    }
}

void process_handles(void)
{
    if (!check_handles())
    {
        // Enable tri-state
        PORTD &= ~HANDS_OFF;
    }
    else
    {
        // Deactivate tri-state
        PORTD |= HANDS_OFF;
    }
}

void process_light(void)
{
    if (!is_dark && !check_light())
    {
        is_dark = 1;
        light_push_button();
    }
    else if (is_dark && check_light())
    {
        is_dark = 0;
        light_push_button();
    }
}
