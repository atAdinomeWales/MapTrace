#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "MapTrace.h"

typedef struct {
    char marker[4];
} FileHeader;

void serializeDrawable(drawable_t* drawable, const char* fileName);
bool deserializeDrawable(const char* fileName, drawable_t* newDrawable);

#endif//IO_HANDLER_H