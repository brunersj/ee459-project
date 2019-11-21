#include <avr/interrupt.h>
#include <string.h>
#include "gps.h"
#include "serial.h"

static unsigned char strStartsWith(const char* str, const char* prefix);

volatile char line1[MAXLINELENGTH]; // Double buffer
volatile char line2[MAXLINELENGTH];
volatile uint8_t lineidx;           // Character index
volatile char *currentline;         // Pointer to current line buffer
volatile char *lastline;            // Pointer to previous line buffer
volatile unsigned char received;    // Received flag

void gps_init(unsigned short ubrr)
{
    received = 0;
    lineidx = 0;
    currentline = line1;
    lastline = line2;
    serial_init(ubrr);

    UCSR0B |= (1 << RXCIE0); // Enable Receive interrupt
    sei();
}

void gps_parse(char* gps_data)
{
    if (received)
    {
        received = 0;
        if (strStartsWith(lastline, "$GPRMC"))
        {
            strncpy(gps_data, lastline, MAXLINELENGTH);
        }
    }
}

ISR(USART_RX_vect)
{
    char c = UDR0;

    currentline[lineidx++] = c;
    if (lineidx >= MAXLINELENGTH)
        lineidx = MAXLINELENGTH-1;		// ensure there is someplace to put the next received character

    if (c == '\n')
    {
        currentline[lineidx] = '\0';

        if (currentline == line1)
        {
            currentline = line2;
            lastline = line1;
        }
        else
        {
            currentline = line1;
            lastline = line2;
        }

        lineidx = 0;
        received = 1;
    }
}

static unsigned char strStartsWith(const char* str, const char* prefix)
{
    while (*prefix) {
        if (*prefix++ != *str++)
        return 0;
    }
    return 1;
}
