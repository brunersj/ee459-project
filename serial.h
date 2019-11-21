#ifndef SERIAL_H_   /* Include guard */
#define SERIAL_H_

void serial_init(unsigned short ubrr);
void serial_string(char* str);
void serial_out(char ch);
char serial_in();

#endif // FOO_H_