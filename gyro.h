#ifndef GYRO_H_
#define GYRO_H_

unsigned char gyro_init(unsigned char);
unsigned char gyro_read(int*, int*, int*);
unsigned char gyro_is_downhill(void);
void gyro_compare(void);
float convert_to_g(unsigned int);

#endif