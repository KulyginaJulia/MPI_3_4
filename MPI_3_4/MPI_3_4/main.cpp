#include "mpi.h"
#include "Function.h"
#include "HGeneration.h"
#include "Header.h"
#include "string.h"

unsigned char decode_stack[4000];
unsigned int * code_value = (unsigned int *)malloc(TABLE_SIZE * sizeof(unsigned int));
unsigned int * prefix_code = (unsigned int *)malloc(TABLE_SIZE * sizeof(unsigned int));
unsigned char * append_character = (unsigned char *)malloc(TABLE_SIZE * sizeof(unsigned char));

int main(int argc, char *argv[]){
	int ProcNum, ProcRank, dataSize;
	double time1, time2, delta_time_1, delta_time_2, deltaSize;
	FILE *input_file;
	FILE *output_file;
	FILE *lzw_file;
	int Count;
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

	dataSize = (int)(Count / (ProcNum - 1));
	deltaSize = Count % (ProcNum - 1);
	if (ProcRank == 0) {
		Generation(Count);

		fopen_s(&input_file, "test.txt", "rb");
		fopen_s(&lzw_file, "test_lzw.txt", "wb");
		if (input_file == NULL || lzw_file == NULL) {
			printf("Fatal error opening files.\n");
			exit(1);
		}
		time1 = MPI_Wtime();

		compress(input_file, lzw_file);
		time2 = MPI_Wtime();

		delta_time_1 = time2 - time1;

		
		fclose(lzw_file);
		

		fopen_s(&lzw_file, "test_lzw.txt", "rb");
		fopen_s(&output_file, "testout.txt", "wb");
		if (lzw_file == NULL || output_file == NULL) {
			printf("Fatal error opening files.\n");
			exit(1);
		}
		expand(lzw_file, output_file);
		fclose(output_file);
		fopen_s(&lzw_file, "testout.txt", "rb");
		Testing(output_file, input_file, Count);

		fclose(lzw_file);
		fclose(output_file);
		fclose(input_file);
		for (int i = 0; i < TABLE_SIZE; i++) {
			prefix_code[i] = -1;
			code_value[i] = -1;
			append_character[i] = -1;
		}
		
		printf_s("\n Consistetnt Time: %f", delta_time_1);
		printf_s("\n");
	}

	//// создание топологии типа звезда
	//int *index = (int*)malloc((ProcNum-1)*sizeof(int));
	//int *edges = (int *)malloc(2*(ProcNum-1)*sizeof(int));
	//int i;
	//for (i = 0; i < ProcNum-1; i++) {
	//	edges[i] = ProcNum - i;
	//}
	//for (i = ProcNum + 1; i < 2 * (ProcNum-1); i++) {
	//	edges[i] = 0;
	//}
	//index[0] = ProcNum-1;
	//for (i = 1; i < ProcNum; i++) {
	//	index[i] = 1;
	//}
	//MPI_Comm StarComm;
	//MPI_Graph_create(MPI_COMM_WORLD, ProcNum, index, edges, 1, &StarComm);

	//MPI_Datatype StructTable;
	//int count = 4;
	//int *blocklens = (int*)malloc(4*sizeof(int));
	//MPI_Aint *indices = (MPI_Aint *)malloc(4 * sizeof( MPI_Aint));
	//MPI_Datatype* oldtypes = (MPI_Datatype*)malloc(4*sizeof(MPI_Datatype));
	//for (i = 0; i<2; i++){
	//	blocklens[i] = TABLE_SIZE;
	//	indices[i] = 3*TABLE_SIZE * sizeof(int);
	//	}
	//oldtypes[0] = oldtypes[1] = MPI_UNSIGNED;
	//oldtypes[2] = MPI_UNSIGNED_CHAR; oldtypes[3] = MPI_INT;
	//blocklens[3] = 1;
	//indices[3] = 1*sizeof(int);

	//MPI_Type_create_struct(count, blocklens, indices, oldtypes, &StructTable);
	//MPI_Type_commit(&StructTable);

	if (ProcRank == 0) {
//		int TableSize = TABLE_SIZE;
//		int *temp;
//		StructTable temp;// = malloc(sizeof(StructTable));
//		unsigned int * temp_code_value = (unsigned int *)malloc(TABLE_SIZE * sizeof(unsigned int));
		int i, j;
		time1 = MPI_Wtime();
		unsigned int *temp_code = (unsigned int *)malloc(TABLE_SIZE * sizeof(unsigned int));
		unsigned int **out_code = (unsigned int **)malloc((ProcNum+1) *TABLE_SIZE* sizeof(unsigned int));
		for (i = 1; i < ProcNum; i++)
			out_code[i] = temp_code;
		fopen_s(&input_file, "test.txt", "rb");

		char *str_file = (char*)malloc(Count * sizeof(char));
		char* out_str = (char*)malloc(Count * sizeof(char));
		while (fgets(str_file, Count, input_file) != NULL);
		
		fclose(input_file);
		fopen_s(&lzw_file, "testp_lzw.txt", "wb");


		char *tempv = str_file;
		for (i = 1; i < ProcNum; i++) {
				MPI_Send(tempv, dataSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				tempv = tempv + dataSize;
	
		}

		//Приняли обратно результат
		char* temp_out = out_str;
		
		for (i = 1; i < ProcNum; i++) {
			MPI_Recv(temp_out, dataSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			temp_out = temp_out + dataSize;
			MPI_Recv(out_code[i], TABLE_SIZE, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
		}
	
		j = ProcNum;//конец массива массивов кодов. jый должен быть последним
		if (deltaSize != 0) {
			out_str = compress(str_file,deltaSize, out_str, out_code[ProcNum+1]);
			j = ProcNum + 1;
		}
		/**/i = 2;
		while (i<j) {
			out_code[i] = Merge(out_code[i], out_code[i-1], TABLE_SIZE);
			i++;
		}
//		/*Упаковка массива в файл
//		и распаковка всего оного чуда
//*/
//
		time2 = MPI_Wtime();
		delta_time_2 = time2 - time1;
		printf_s("\n Parallel Time: %f", delta_time_2);
		printf_s("\n Speed: %f", delta_time_1 / delta_time_2);
		printf_s("\n");
		for(i = 0; i < ProcNum-1;i++)
			free(temp_code);
		free(out_code);
		free(str_file);
		free(out_str);
	}
	
	if (ProcRank != 0) {
		char *str_file1 = (char *)malloc(dataSize * sizeof(char));
		char* out_str1 = (char*)malloc(dataSize * sizeof(char));
		unsigned int *out_code_value = (unsigned int *)malloc(TABLE_SIZE * sizeof(unsigned int));
		for (int i = 0; i < dataSize; i++)
			out_str1[i] = 0;

		MPI_Recv(str_file1, dataSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);//приняли строку

		out_str1 = compress(str_file1, dataSize, out_str1, out_code_value);
		
		MPI_Send(out_str1, Count, MPI_CHAR, 0, 0, MPI_COMM_WORLD);//отправили выходной поток

		MPI_Send(out_code_value, TABLE_SIZE, MPI_UNSIGNED,0,0,MPI_COMM_WORLD);
	
		//декомпрессия

		free(str_file1);
		free(out_str1);
		free(out_code_value);
	}

	free(code_value);
	free(prefix_code);
	free(append_character);

	MPI_Finalize();
	return 0;
}