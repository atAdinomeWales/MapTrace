#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IoHandler.h"
#include "MapTrace.h"


uint8_t fileHeaderCmp(FileHeader a, FileHeader b){
    if (!a || !b){
        return HED_NO_HEADER;
    }

    if (memcmp(a, b, sizeof(FileHeader)) == 0){
        return HED_MATCH;
    }

    if ((memcmp(a, ":FI:", sizeof(FileHeader)) == 0)){
        return HED_SET_FILE;
    } if ((memcmp(a, "|FI|", sizeof(FileHeader)) == 0)){
        return HED_END_FILE;
    } if ((memcmp(a, ":EX:", sizeof(FileHeader)) == 0)){
        return HED_SET_EXIT;
    } if ((memcmp(a, "|EX|", sizeof(FileHeader)) == 0)){
        return HED_END_EXIT;
    }
    return HED_NO_MATCH;
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

long scanForHeader(FILE* file, FileHeader hed){
    if (file == NULL || !hed){
        return -1;
    }

    long currpos = ftell(file);

    fseek(file, 0, SEEK_END);
    long fSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fSize == 0){
        return -1;
    }

    int buffSize = sizeof(FileHeader);
    char buffer[buffSize];

    while (fread(buffer, sizeof(buffer), 1, file)){
        if (sweepMemcmp(hed, sizeof(FileHeader), buffer, buffSize) >= 0){
            long ret  = ftell(file) - sizeof(FileHeader);
            fseek(file, currpos, SEEK_SET);
            return ret;
        }
    }

    fseek(file, currpos, SEEK_SET);
    return -2;
}

void serializeDrawable(drawable_t* drawable, const char* fileName){
    if (drawable == NULL){
        return;
    }

    FILE* file = fopen(fileName, "wb");

    if (!file){
        fprintf(stderr, "Could not open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    FileHeader buffer;
    drawNode_t* curr = drawable->head;

    if (fileSize != 0){
        fread(&buffer, sizeof(FileHeader), 1, file);
        // If the file already exsists, start writing from before |XY|
        if (!fileHeaderCmp(buffer, ":FI:")){
            long XYEndHeaderPos = scanForHeader(file, "|XY|");
            if(XYEndHeaderPos == -1){
                return; // TODO: Check for |FI| then erase the file and start again is the save is invalid
            }
            fseek(file, XYEndHeaderPos, SEEK_SET);
            while (curr){
                fwrite(&curr->x, sizeof(float), 1, file);
                fwrite(&curr->y, sizeof(float), 1, file);
                curr = curr->next;
            }
            return;
        }
    }

    fwrite(":FI:", sizeof(FileHeader), 1, file);
    fwrite(":CO:", sizeof(FileHeader), 1, file);
    fwrite(&drawable->color, sizeof(RGB_t), 1, file);
    fwrite("|CO|", sizeof(FileHeader), 1, file);
    fwrite(":XY:", sizeof(FileHeader), 1, file);
    while (curr){
        fwrite(&curr->x, sizeof(float), 1, file);
        fwrite(&curr->y, sizeof(float), 1, file);
        curr = curr->next;
    }
    fwrite("|XY|", sizeof(FileHeader), 1, file);
    fwrite("|FI|", sizeof(FileHeader), 1, file);

    fclose(file);

    return;
}

bool deserializeDrawable(const char* fileName, drawable_t* newDrawable){
    FILE* file = fopen(fileName, "rb");
    if (!file){
        return false;
    }

    FileHeader marker;
    //Check for FILE headers
    fread(&marker, sizeof(FileHeader), 1, file);
    if (fileHeaderCmp(marker, ":FI:")){
        return false;
    } else {
        fseek(file, -sizeof(FileHeader), SEEK_END);
        fread(marker, sizeof(FileHeader), 1, file);
        if (fileHeaderCmp(marker, "|FI|")){
            return false;
        }
        fseek(file, sizeof(FileHeader), SEEK_SET);
    }

    fread(&marker, sizeof(FileHeader), 1, file);
    if (!fileHeaderCmp(marker, ":CO:")) {
        fread(&newDrawable->color, sizeof(RGB_t), 1, file);
        printf("R: %d, G: %d, B: %d, A: %d\n", newDrawable->color.r, newDrawable->color.g, newDrawable->color.b, newDrawable->color.a);
        fread(&marker, sizeof(FileHeader), 1, file);
        if (fileHeaderCmp(marker, "|CO|")){
            return false;
        }
    }

    newDrawable->head = NULL;
    newDrawable->tail = NULL;
    float xBuff, yBuff;

    fread(&marker, sizeof(FileHeader), 1, file);
    if (!fileHeaderCmp(marker, ":XY:")){
        long EndXYpos = scanForHeader(file, "|XY|");
        printf("nodes from %ld to %ld",ftell(file), EndXYpos);
        while (ftell(file) < EndXYpos){
            fread(&xBuff, sizeof(float), 1, file);
            fread(&yBuff, sizeof(float), 1, file);
            addNode(newDrawable, xBuff, yBuff);
            printf("node added: x = %f, y = %f\n", xBuff, yBuff);
        }
    }

    fclose(file);

    return true;
}

//nodeCompareFromeFile();