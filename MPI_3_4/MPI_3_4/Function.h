#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

using namespace std;

#define BITS 12                  // Установка длины кода равной 12 битам.
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096 // 2^12

extern unsigned char decode_stack[4000];//массив для значений кодов

extern unsigned int * code_value; // Этот массив содержит префиксы кодов
extern unsigned int *prefix_code; //Этот массив содержит добавочные символы
extern unsigned char *append_character; //Этот массив содержит декодируемые строки

unsigned int input_code(FILE *input); 

unsigned char *decode_string(unsigned char *buffer, unsigned int code);
void compress(FILE *input, FILE *output);
void expand(FILE *input, FILE *output);
void output_code(FILE *output, unsigned int code);
int find_match(int hash_prefix, unsigned int hash_character);//Ищет соответствующую строку в таблице
#endif