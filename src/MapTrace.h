#ifndef MAP_TRACE_H
#define MAP_TRACE_H

#include <SDL2/SDL.h>

#define ASSERT(_e, ...) if(!(_e)) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define WINDOW_WIDTH 860
#define WINDOW_HEIGHT 640
#define PIXEL_THRESHOLD 2

typedef struct drawNode_t{
    float x;
    float y;
    struct drawNode_t* next;
}drawNode_t;

typedef struct RGB_t{
    uint8_t r, g, b, a;
}RGB_t;

typedef struct drawable_t {
    int resX;
    int resY;
    RGB_t color;
    SDL_Texture* texture;
    struct drawNode_t* head;
    struct drawNode_t* tail;
}drawable_t;

/*typedef struct layer_t {

} layer;*/

void addNode(struct drawable_t* drawable, float x, float y);
void freeDrawNodes(drawNode_t* head);
void freeDrawable(drawable_t* d);
void drawPoint(struct drawNode_t* node, SDL_Renderer* renderer, RGB_t n);
void drawLine(struct drawNode_t* node, SDL_Renderer* renderer, RGB_t n);
void renderDrawable(struct drawable_t* drawable, SDL_Renderer* renderer);

#endif//MAP_TRACE_H