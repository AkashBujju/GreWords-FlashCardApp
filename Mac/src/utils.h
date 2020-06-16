#ifndef UTILS_H
#define UTILS_H

int read_file(const char* filename, char *buffer);
char* combine_string(const char* str_1, const char* str_2);
void read_floats_from_file(const char* filename, float *buffer);
void get_substring(const char* source_string, char* destination_string, unsigned int from, unsigned int to);

#endif
