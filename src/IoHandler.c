#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IoHandler.h"
#include "MapTrace.h"
#include "basics.h"


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

void extractXYFromFile(FILE* file, drawable_t* drawable, bool keepCurrPos){
    if (!file || !drawable){
        return;
    }
    long currPos = ftell(file);
    float xBuff, yBuff;
    FileHeader marker;
    fread(&marker, sizeof(FileHeader), 1, file);

    if (!fileHeaderCmp(marker, ":XY:")){
        long EndXYpos = scanForHeader(file, "|XY|");
        int nodeCount = (int)( ( EndXYpos - ftell(file) ) / 2 ) / sizeof(xBuff);
        printf("nodes from %ld to %ld, total nodes: %ld\n",ftell(file), EndXYpos, nodeCount);
        while (ftell(file) < EndXYpos){
            fread(&xBuff, sizeof(float), 1, file);
            fread(&yBuff, sizeof(float), 1, file);
            addXYNode(drawable, xBuff, yBuff);
            if (nodeCount < 200) { printf("node added: x = %d, y = %d\n", (int)xBuff, (int)yBuff); }
        }
    }

    if (keepCurrPos) { fseek(file, currPos, SEEK_SET); }
    return;
}

bool nodeCompareFromeFile(FILE* file, drawable_t* drawable, int check){ //TODO finish this function!
    if (!file || !drawable){
        return false;
    }

    long currPos = ftell(file);
    fseek(file, 0, SEEK_END);
    long fSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (check & CMP_DRW_COL){
        scanForHeader(file, ":CO:");
    } if (check & CMP_DRW_XY){

    }

    fseek(file, currPos, SEEK_SET);
    return true;
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
        if (!fileHeaderCmp(buffer, ":FI:")){
            long XYEndHeaderPos = scanForHeader(file, "|XY|");
            if (XYEndHeaderPos < 0){
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
    fwrite(&drawable->color, sizeof(RGBA_t), 1, file);
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
        fread(&newDrawable->color, sizeof(RGBA_t), 1, file);
        printf("R: %d, G: %d, B: %d, A: %d\n", newDrawable->color.r, newDrawable->color.g, newDrawable->color.b, newDrawable->color.a);
        fread(&marker, sizeof(FileHeader), 1, file);
        if (fileHeaderCmp(marker, "|CO|")){
            return false;
        }
    }

    newDrawable->head = NULL;
    newDrawable->tail = NULL;

    extractXYFromFile(file, newDrawable, false);

    fclose(file);

    return true;
}