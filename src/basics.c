#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int argvFind(int argc, char** argv, char* str){
    if (!(argc && argv && str)){
        return -1;
    }

    for (int i = 0; i < argc; i++){
        if(strcmp(argv[i], str) == 0){
            return i;
        }
    }

    return -2;
}

int sweepMemcmp(const void* target, size_t targetSize, const void* buffer, size_t bufferSize){
    if (bufferSize < targetSize || !target || !buffer) {
        return -2;
    }

    for (size_t n = 0; n < bufferSize - targetSize + 1; n++) {
        if (memcmp((const char*)buffer + n, target, targetSize) == 0) {
            return n;
        }
    }

    return -1;
}