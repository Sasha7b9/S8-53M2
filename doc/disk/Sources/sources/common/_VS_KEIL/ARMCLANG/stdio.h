#pragma once

#ifdef WIN32
#pragma warning(push, 0)
#endif
int vsprintf(char *__restrict, const char *__restrict, struct va_list);
#ifdef WIN32
#pragma warning(pop)
#endif
int sprintf(char *__restrict, const char *, ...);
int snprintf(char *__restrict, unsigned int, const char *, ...);
int printf(const char *__restrict, ...);
