#ifndef BASICS_H
#define BASICS_H

#include <stdio.h>

#define ASSERT(_e, ...) if(!(_e)) {fprintf(stderr, __VA_ARGS__); exit(1);}

int argvFind(int argc, char** argv, char* str);
int sweepMemcmp(const void* target, size_t targetSize, const void* buffer, size_t bufferSize);

#endif//BASICS_H