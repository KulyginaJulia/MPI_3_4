#include "HGeneration.h"
//#include "Function.h"
#include "Header.h"

char *compress(char *input, int count, char *output, unsigned int* temp_code_value){
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i = 0;

	next_code = 256;

	for (i = 0; i < TABLE_SIZE; i++) {
		temp_code_value[i] = -1;
		prefix_code[i] = -1;
		append_character[i] = -1;
	}
	i = 0;
	printf_s("Compressing...\n");
	string_code = input[0];
	i++;

	while (i != count){
		character = input[i];
		// Смотрит, есть ли строка в таблице, если есть, то возвращает код, иначе заносит в таблицу
		index = find_match(string_code, character, temp_code_value);
		if (temp_code_value[index] != -1)
			string_code = temp_code_value[index];
		else
		{
			if (next_code <= MAX_CODE)
			{
				temp_code_value[index] = next_code++;
				prefix_code[index] = string_code;
				append_character[index] = character;
			}
			output = output_code(output, string_code,count);
			string_code = character;
		}
		i++;
	}
	output = output_code(output, string_code, count);
	//for (int i = 0 ; i < count; i++) {
	//	printf_s("output[%d", i);
	//	printf_s("] = %c", output[i]);
	//	printf_s("\n");
	//}
	return output;
}

unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value)
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
		if ((prefix_code[index] == hash_prefix) && (append_character[index] == hash_character))
			return(index);
		index -= offset;
		if (index < 0)
			index += TABLE_SIZE;
	}
}


char * output_code(char *output, unsigned int code, int count)
{
	static int j = 0;
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0L;
	output_bit_buffer = output_bit_buffer | ((unsigned long)code << (32 - BITS - output_bit_count));
	output_bit_count += BITS;
	while ((output_bit_count >= 8) && (j < count) ){
		output[j] = output_bit_buffer >> 24; //oshibka
		output_bit_buffer = output_bit_buffer << 8;
		output_bit_count -= 8;
		j++;
	}
	return output; 
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
	bool flag = true;
	char* ch_1 = (char*)malloc(count * sizeof(char));
	char* ch_2 = (char*)malloc(count * sizeof(char));
	while (!feof(A) && !feof(B))
	{
		fgets(ch_1, count, A);
		fgets(ch_2, count, B);
		if (strcmp(ch_1, ch_2)) {
			flag = false;
			printf_s("Uncorrect!");
		}
	}

	if (flag == true) {
		printf_s("Correct!");
	}
	free(ch_1);
	free(ch_2);
}