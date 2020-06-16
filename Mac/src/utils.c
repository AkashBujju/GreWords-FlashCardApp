#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file(const char* filename, char *buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	const size_t filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(buffer, 1, filesize, file);
	buffer[filesize] = 0;
	
	fclose(file);
	return 0;
}

void read_floats_from_file(const char* filename, float *buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
		return;
	}

	int num_values;
	fscanf(file, "%d", &num_values);

	for(int i = 0; i < num_values; ++i)
		fscanf(file, "%f", buffer + i);

	fclose(file);
}

char* combine_string(const char* str_1, const char* str_2) {
	unsigned int len = 100;
	char* str = (char*)malloc(sizeof(char) * len);

	strcpy(str, str_1);
	strcat(str, str_2);

	return str;
}

void get_substring(const char* source_string, char* destination_string, unsigned int from, unsigned int to) {
	unsigned int index = 0;
	for(unsigned int i = from; i <= to; ++i) {
		destination_string[index++] = source_string[i];
	}
	destination_string[index] = '\0';
}
