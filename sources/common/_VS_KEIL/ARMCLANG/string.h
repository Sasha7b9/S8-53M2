#pragma once


unsigned int strlen(const char *);
int strcmp(const char *, const char *);
char *strcat(char *__restrict, const char *__restrict);
char *strcpy(char *__restrict, const char *__restrict);

void *memset(void *, int, unsigned int);
int memcmp(const void *, const void *, unsigned int);
void *memcpy(void *, const void *, unsigned int);
void *memmove(void *dest, const void *src, unsigned int);
