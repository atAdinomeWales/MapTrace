#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IoHandler.h"
#include "MapTrace.h"

void serializeDrawable(drawable_t* drawable, const char* fileName){
    if (drawable == NULL){
        return;
    }

    FILE* file = fopen(fileName, "ab");

    if (!file){
        fprintf(stderr, "Could not open file");
        return;
    }

    // Write color info
    FileHeader rgbMarker = { "RGB:" };
    fwrite(&rgbMarker, sizeof(FileHeader), 1, file);

    fwrite(&drawable->color, sizeof(RGB_t), 1, file);

    //Write XY draw nodes
    FileHeader xyMarker = { "XY:" };
    fwrite(&xyMarker, sizeof(FileHeader), 1, file);

    drawNode_t* curr = drawable->head;
    while (curr){
        fwrite(&curr->x, sizeof(float), 1, file);
        fwrite(&curr->y, sizeof(float), 1, file);
        curr = curr->next;
    }

    fclose(file);
    return;
}

bool deserializeDrawable(const char* fileName, drawable_t* newDrawable){
    FILE* file = fopen(fileName, "rb");
    if (!file){
        fprintf(stderr, "Could not open file");
        return false;
    }
    // Read the marker
    FileHeader marker;
    fread(&marker, sizeof(FileHeader), 1, file);

    // Check the marker to determine the type of data
    printf("firts steps\n");
    if (strcmp(marker.marker, "RGB:") == 0) {
        // Read RGB information
        fread(&newDrawable->color, sizeof(RGB_t), 1, file);
        printf("R: %d, G: %d, B: %d, A: %d\n", newDrawable->color.r, newDrawable->color.g, newDrawable->color.b, newDrawable->color.a);
        printf("always\n");
        // Read the marker for XY coordinates
        fread(&marker, sizeof(FileHeader), 1, file);
    }
    printf("seem tiny\n");


    newDrawable->head = NULL;
    newDrawable->tail = NULL;

    float xBuff, yBuff;
    while(fread(&xBuff, sizeof(float), 1, file) && fread(&yBuff, sizeof(float), 1, file)){
        addNode(newDrawable, xBuff, yBuff);
        printf("node added: x = %f, y = %f\n", xBuff, yBuff);
    }

    fclose(file);

    return true;
}