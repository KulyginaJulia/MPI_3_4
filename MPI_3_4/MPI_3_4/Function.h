#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BITS 12                  // Установка длины кода равной 12 битам.
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096 // 2^12

extern unsigned char decode_stack[4095];//Этот массив содержит декодируемые строкид
extern unsigned int code_value[TABLE_SIZE]; //массив для значений коов
extern unsigned int prefix_code[TABLE_SIZE]; // Этот массив содержит префиксы кодов
extern unsigned char append_character[TABLE_SIZE]; //Этот массив содержит добавочные символы

unsigned int input_code(FILE *input, int indicator); 

unsigned char *decode_string(unsigned char *buffer, unsigned int code);
void compress(FILE *input, FILE *output);
void expand(FILE *input, FILE *output, int indicator);
void output_code(FILE *output, unsigned int code);
int find_match(int hash_prefix, unsigned int hash_character);
#endif