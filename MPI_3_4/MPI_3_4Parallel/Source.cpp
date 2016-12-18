#include "HGeneration.h"
#include "Header.h"

char * output_code(char *output, unsigned int code, int count)
{
	static int j = 0;
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0L;
	output_bit_buffer = output_bit_buffer | ((unsigned long)code << (32 - BITS - output_bit_count));
	output_bit_count += BITS;
	while ((output_bit_count >= 8) && (j < count) ){

		output[j] = output_bit_buffer >> 24;
		output_bit_buffer = output_bit_buffer << 8;
		output_bit_count -= 8;
		j++;
	}
	return output; 
}
char *compress(char *input, int count, char *output, int count_out, unsigned int* temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code) {
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i = 0;

	next_code = 256;

	for (i = 0; i < TABLE_SIZE; i++) {
		temp_code_value[i] = -1;
		p_prefix_code[i] = -1;
		p_append_character[i] = -1;
	}
	//Initial_dictonary(temp_code_value, p_prefix_code, p_append_character);
	i = 0;
	printf_s("Compressing...\n");
	string_code = input[0];
	i++;

	while (i < count){
		character = input[i];
		// Смотрит, есть ли строка в таблице, если есть, то возвращает код, иначе заносит в таблицу
		index = find_match(string_code, character, temp_code_value, p_append_character,p_prefix_code);
		if (temp_code_value[index] != -1)
			string_code = temp_code_value[index];
		else
		{
			if (next_code <= MAX_CODE)
			{
				temp_code_value[index] = next_code++;
				p_prefix_code[index] = string_code;
				p_append_character[index] = character;
			}
			output = output_code(output, string_code,count_out);
			string_code = character;
		}
		i++;
	}
	output = output_code(output, string_code, count_out);
	return output;
}

unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code)
{
	int index;
	int offset;

	index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
	if (index == 0)
		offset = 1;
	else
		offset = TABLE_SIZE - index;
	while (1)
	{
		if (temp_code_value[index] == -1)
			return(index);
		if ((p_prefix_code[index] == hash_prefix) && (p_append_character[index] == hash_character))
			return(index);
		index -= offset;
		if (index < 0)
			index += TABLE_SIZE;
	}
}




void bubblesort(unsigned int* A, int count){
	int i, j, tmp;
	for (i = count - 1; i >= 0; i--){
		for (j = 0; j < i; j++){
			if (A[j] > A[j + 1]){
				tmp = A[j];
				A[j] = A[j + 1];
				A[j + 1] = tmp;
			}
		}
	}
}

unsigned int* Merge(unsigned int* A,unsigned int* B, int Count){
	int a=0, b=0;
	unsigned int* tmp = (unsigned int*)malloc(TABLE_SIZE*sizeof(unsigned int));
	while (a + b < 2*Count)
	{
		if ((b >= Count) || ((a<Count) && (A[a] <= B[b]))){
			tmp[a + b] = A[a];
			++a;
		}
		else {
			tmp[a + b] = B[b];
			++b;
		}
	}
	return tmp;
}


void Testing(FILE* A, FILE* B, int count) {
	int flag = 1;
	char* ch_1 = (char*)malloc(count * sizeof(char));
	char* ch_2 = (char*)malloc(count * sizeof(char));
	while (!feof(A) && !feof(B))
	{
		fgets(ch_1, count, A);
		fgets(ch_2, count, B);
		if (strcmp(ch_1, ch_2))
			flag = 0;
	}

	if (flag == 1)
		printf_s("Correct!");
	else
		printf_s("Uncorrect!");
	free(ch_1);
	free(ch_2);
}