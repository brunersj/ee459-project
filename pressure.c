#include <avr/io.h>
#include "pressure.h"
#include "adc.h"

#define PRESSURE_LEFT 1  // PC1
#define PRESSURE_RIGHT 2 // PC2
#define THRESHOLD 100

void pressure_init(void)
{
    adc_init();
}

unsigned char sample_pressure(unsigned char left)
{
    if (left)
        return adc_sample(PRESSURE_LEFT);
    else
        return adc_sample(PRESSURE_RIGHT);
    
}

// Returns 1 if both handles are pressed
// 0 otherwise
unsigned char check_handles(void)
{
    // TODO: Check both left and right handles
    if (sample_pressure(1) < THRESHOLD || 
        sample_pressure(0) < THRESHOLD)
        return 0;
    return 1;
}
