#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "MapTrace.h"

typedef char FileHeader[4];

enum headerEnum{
    HED_NO_MATCH    = -2,
    HED_NO_HEADER   = -1,
    HED_MATCH       = 0,
    HED_SET_FILE    = 1,
    HED_END_FILE    = 2,
    HED_SET_EXIT    = 3,
    HED_END_EXIT    = 4
};

enum compareDrawables{
    CMP_DRW_XY,
    CMP_DRW_COL
};

uint8_t fileHeaderCmp(FileHeader a, FileHeader b);
long scanForHeader(FILE* file, FileHeader hed);
void extractXYFromFile(FILE* file, drawable_t* drawable, bool keepCurrPos);
bool nodeCompareFromeFile(FILE* file, drawable_t* drawable, int check);
void serializeDrawable(drawable_t* drawable, const char* fileName);
bool deserializeDrawable(const char* fileName, drawable_t* newDrawable);

#endif//IO_HANDLER_H