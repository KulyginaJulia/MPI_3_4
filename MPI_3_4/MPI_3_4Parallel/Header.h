#ifndef _HEADER_H_
#define _HEADER_H_

#define BITS 12                  // Установка длины кода равной 12 битам.
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096 // 2^12

extern unsigned int *prefix_code;
extern unsigned char *append_character;

unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value);
char* compress(char *input, int count, char *output, unsigned int *temp_code_value);
char * output_code(char *output, unsigned int code);
#endif
