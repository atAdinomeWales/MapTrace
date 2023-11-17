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

long scanForHeader(FILE* file, FileHeader hed){
    if (file == NULL || !hed){
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if(size == 0){
        return -1;
    }
    char buffer[100];

    while (fread(buffer, sizeof(buffer), 1, file)){
        if (fileHeaderCmp(buffer, hed) == 0){
            return ftell(file) - sizeof(FileHeader);
        }
    }

    return -1;
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
    //fwrite("|XY|", sizeof(FileHeader), 1, file);
    //fwrite("|FI|", sizeof(FileHeader), 1, file);

    fclose(file);

    return;
}

bool deserializeDrawable(const char* fileName, drawable_t* newDrawable){
    FILE* file = fopen(fileName, "rb");
    printf("opened\n");
    if (!file){
        fprintf(stderr, "Could not open file");
        return false;
    }

    FileHeader marker;
    //Check for FILE headers
    fread(&marker, sizeof(FileHeader), 1, file);
    printf("read first marker, it is %s\n", marker);
    if (fileHeaderCmp(marker, ":FI:")){
        printf("dindt find :FI:, found this though %s\n", marker);
        return false;
    } else {
        /*fseek(file, -sizeof(FileHeader), SEEK_END);
        printf("went to the last header\n");
        fread(marker, sizeof(FileHeader), 1, file);
        printf("read last header\n");
        if (fileHeaderCmp(marker, "|FI|")){
            printf("this header is not |FI| it is in fact %s\n", marker);
            return false;
        }
        fseek(file, sizeof(FileHeader), SEEK_SET);
        printf("Went right back to the second header\n");*/
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
        while (fread(&xBuff, sizeof(float), 1, file) && fread(&yBuff, sizeof(float), 1, file)){
            addNode(newDrawable, xBuff, yBuff);
            printf("node added: x = %f, y = %f\n", xBuff, yBuff);
        } // I want to scan for an end XY header, store its position and store XY data till the position is hit
    }


    fclose(file);

    return true;
}

//nodeCompareFromeFile();