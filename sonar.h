#ifndef SONAR_H_
#define SONAR_H_

void sonar_init(void);
void sonar_send_signal(void);
unsigned int sonar_get_range(unsigned char);
unsigned char sonar_is_close(unsigned char);
ISR(PCINT0_vect);

#endif