#include <avr/io.h>
#include "gyro.h"
#include "i2c.h"
#include "adc.h"

#define NULL 0
#define READ_SIZE 6
// Gyro address shifted left once (0x18 << 1)
#define GYRO_ADDR 0x30
#define REG_CTRL1 0x20
#define REG_CTRL4 0x23
#define REG_OUT_X_L 0x28

#define THRESHOLD_Z 16000
#define THRESHOLD_Y 2000

#define THROTTLE_IN PC3
#define THROTTLE_CMP (1 << PD5)
#define V_REF 128

unsigned char gyro_init(unsigned char bdiv)
{
	DDRD |= THROTTLE_CMP;
	// Initialize ADC
	adc_init();

	// Initialize I2C
	i2c_init(bdiv);

	uint8_t status;

	// Enable all axes, normal mode, 400Hz
	uint8_t unit_addr = REG_CTRL1;
	uint8_t data = 0x77;
	status = i2c_io(GYRO_ADDR, &unit_addr, 1, &data, 1, NULL, 0);
	if (status != 0)
		return status;
	
	// High res & Enable BDU
	unit_addr = REG_CTRL4;
	data = 0x88;
	status = i2c_io(GYRO_ADDR, &unit_addr, 1, &data, 1, NULL, 0);
	if (status != 0)
		return status;
	
	return status;	
}

void gyro_compare()
{
	unsigned char v_throttle = adc_sample(THROTTLE_IN);
	if (v_throttle > V_REF)
		PORTD |= THROTTLE_CMP;
	else
		PORTD &= ~THROTTLE_CMP;
}

unsigned char gyro_read(int* x, int* y, int* z)
{
	uint8_t a_buf = (REG_OUT_X_L | 0x80);		// Enable auto increment
	uint8_t r_buf[READ_SIZE];		// 2 bytes for each of the axes
	
	// Read from the gyro through I2C
	uint8_t status = i2c_io(GYRO_ADDR, &a_buf, 1, NULL, 0, r_buf, READ_SIZE);
	if (status != 0)
		return status;
	
	uint16_t temp_x = r_buf[0];
	temp_x |= ((uint16_t)r_buf[1]) << 8;
	uint16_t temp_y = r_buf[2];
	temp_y |= ((uint16_t)r_buf[3]) << 8;
	uint16_t temp_z = r_buf[4];
	temp_z |= ((uint16_t)r_buf[5]) << 8;

	*x = (int)temp_x;
	*y = (int)temp_y;
	*z = (int)temp_z;
	
	return status;
}

// Return 0 if not going downhill
// 		  1 if going downhill
//		  2 if error from reading
unsigned char gyro_is_downhill()
{
	int x = 0, y = 0, z = 0;
	// TODO: Make the reading resolution bigger (e.g. use sonar timer with 60ms)
	if (gyro_read(&x, &y, &z))
		return 2;
	
	if (x < 1000 && x > 0 && y > THRESHOLD_Y && z < THRESHOLD_Z)
		return 1;
	else
		return 0;
}

float convert_to_g(unsigned int in)
{
	uint16_t divider = 16384;	// Divider at +/- 2g (default value)
	return (float)in / divider;
}
