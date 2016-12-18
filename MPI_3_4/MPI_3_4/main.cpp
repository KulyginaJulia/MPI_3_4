#include "mpi.h"
#include "Function.h"
#include "HGeneration.h"
#include "Header.h"
#include "string.h"

unsigned char decode_stack[4095];
unsigned int code_value[TABLE_SIZE];
unsigned int prefix_code[TABLE_SIZE];
unsigned char append_character[TABLE_SIZE];

int main(int argc, char *argv[]){
	int ProcNum, ProcRank, dataSize;
	double time1, time2, delta_time_1, delta_time_2, deltaSize;
	FILE *input_file;
	FILE *output_file;
	FILE *lzw_file;
	int Count;
	char *str_file = NULL;
	if (code_value == NULL || prefix_code == NULL || append_character == NULL)
	{
		printf("Fatal error allocating table space!\n");
		exit(1);
	}
	if (argc >= 2) {
		Count = atoi(argv[1]);
	}
	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	str_file = (char*)malloc(Count * sizeof(char));
	dataSize = (int)(Count / (ProcNum - 1));
	deltaSize = Count % (ProcNum - 1);
	if (ProcRank == 0) {
		Generation(Count);

		fopen_s(&input_file, "test.txt", "rb");
		fopen_s(&lzw_file, "test.lzw", "wb");
		if (input_file == NULL || lzw_file == NULL) {
			printf("Fatal error opening files.\n");
			exit(1);
		}
		time1 = MPI_Wtime();

		compress(input_file, lzw_file);
		time2 = MPI_Wtime();

		delta_time_1 = time2 - time1;

		fclose(lzw_file);
		
		fopen_s(&lzw_file, "test.lzw", "rb");
		fopen_s(&output_file, "testout.txt", "wb");
		if (lzw_file == NULL || output_file == NULL) {
			printf("Fatal error opening files.\n");
			exit(1);
		}	
		for (int i = 0; i < TABLE_SIZE; i++) {
			prefix_code[i] = -1;
			code_value[i] = -1;
			append_character[i] = 0;
		}
		expand(lzw_file, output_file, 0);
		fclose(output_file);
		fclose(lzw_file);
		fopen_s(&output_file, "testout.txt", "rb");
		Testing(output_file, input_file, Count);

		fclose(output_file);
		fclose(input_file);

		
		printf_s("\n Consistent Time: %f", delta_time_1);
		printf_s("\n");
	}

	if (ProcRank == 0) {
		int i;
	//	int c = (dataSize * (ProcNum-1) / 2) - 1;
		int c = dataSize*(ProcNum+1);
		char *str_file = (char*)malloc((Count+1) * sizeof(char));
		char* out_str = (char*)malloc((c)*ProcNum * sizeof(char));
		char *tempv = str_file;
		char* temp_out = out_str;

		fopen_s(&input_file, "test.txt", "rb");
		fgets(str_file, Count+1, input_file);
		fclose(input_file);

		for (int i = 0; i < Count; i++) {
			printf_s("str[ %d", i);
			printf_s("] = %c", str_file[i]);
			printf_s("\n");
		}

		time1 = MPI_Wtime();

		for (i = 1; i < ProcNum; i++) {
			MPI_Send(tempv, dataSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
			tempv += dataSize;
		}

		//	Приняли обратно результат
		for (i = 1; i < ProcNum; i++) {
			MPI_Recv(temp_out, c, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			temp_out += c;
		}

		if (deltaSize != 0) {
			out_str = compress(str_file, deltaSize, out_str, dataSize, code_value, append_character, prefix_code);
		}

		//	упаковка в файл строки выходного потока
		fopen_s(&lzw_file, "Ptest.lzw", "wb");
		for (i = 0; i < c; i++)
			putc(out_str[i], lzw_file);
		putc(MAX_VALUE, lzw_file);
		putc(0, lzw_file);

		fclose(lzw_file);
		time2 = MPI_Wtime();

		fopen_s(&lzw_file, "Ptest.lzw", "rb");
		fopen_s(&output_file, "Ptestout.txt", "wb");

		expand(lzw_file, output_file, 1);

		fclose(lzw_file);
		fclose(output_file);
		fopen_s(&output_file, "Ptestout.txt", "rb");
		fopen_s(&input_file, "test.txt", "rb");
		Testing(output_file, input_file, Count);

		fclose(output_file);
		fclose(input_file);

		delta_time_2 = time2 - time1;
		printf_s("\n Parallel Time: %f", delta_time_2);
		printf_s("\n Speed: %f", delta_time_1 / delta_time_2);
		printf_s("\n");
		free(str_file);
		free(out_str);
	}
	
	if (ProcRank != 0) {
	//	int c = (dataSize * (ProcNum-1) / 2) -1;
		int c = (ProcNum+1)*dataSize;
		char *str_file1 = (char *)malloc(dataSize * sizeof(char));
		char* out_str1 = (char*)malloc((c) * sizeof(char));
		unsigned int out_code_value[TABLE_SIZE];
		unsigned int  p_prefix_code [TABLE_SIZE];
		unsigned char p_append_character[TABLE_SIZE];
		for (int i = 0; i < c; i++)
			out_str1[i] = -1;
		for (int i = 0; i < TABLE_SIZE; i++)
			out_code_value[i] = -1;
		MPI_Recv(str_file1, dataSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);//приняли строку

		out_str1 = compress(str_file1, dataSize, out_str1, c , out_code_value,p_append_character, p_prefix_code);
		
		MPI_Send(out_str1, c, MPI_CHAR, 0, 0, MPI_COMM_WORLD);//отправили выходной поток

		free(str_file1);
		free(out_str1);
	}

	MPI_Finalize();
	return 0;
}