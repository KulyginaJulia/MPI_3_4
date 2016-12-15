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

