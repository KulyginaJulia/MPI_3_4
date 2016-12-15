#include "HGeneration.h"

void Generation(long int N) {
	FILE *gfile;
	fopen_s(&gfile, "test.txt", "w");
	int i;
	char* s = (char *)malloc(N * sizeof(char)); 
	srand(time(NULL));
	for (i = 0; i < N - 1; i++)
		rand() % 5 ? s[i] = 'a' + rand() % 26 : s[i] = ' ';
	s[i] = '\0';
//	puts(s);
	fprintf(gfile, "%s", s);
	fclose(gfile);
	free(s);
}

void Testing(FILE* A, FILE* B, int count) {
	bool flag = true;
	char* ch_1 = (char*)malloc(count*sizeof(char));
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
}