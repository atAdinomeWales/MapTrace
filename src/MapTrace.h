#ifndef MAP_TRACE_H
#define MAP_TRACE_H

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 860
#define WINDOW_HEIGHT 640
#define PIXEL_THRESHOLD 2

typedef struct drawNode_t{
    float x;
    float y;
    struct drawNode_t* next;
}drawNode_t;

typedef struct{
    uint8_t r, g, b, a;
}RGBA_t;

typedef struct {
    int resX;
    int resY;
    RGBA_t color;
    SDL_Texture* texture;
    struct drawNode_t* head;
    struct drawNode_t* tail;
    struct layer_t* parent_layer;
}drawable_t;

typedef struct {
    drawable_t* drawable;
    struct drawable_node* next;
    struct drawable_node* prev;
} drawable_node;

typedef struct {
    drawable_node* head;
    drawable_node* tail;
    int flags;
    RGBA_t default_color;
    struct layer_t* next;
} layer_t;


void addXYNode(drawable_t* drawable, float x, float y);
void freeDrawNodes(drawNode_t* head);

drawable_t* addDrawable (layer_t* layer);
void freeDrawableNode (drawable_node* dnode);
void freeDrawableList (drawable_node* dnode);
void freeDrawable (drawable_t* d);

void drawPoint (drawNode_t* node, SDL_Renderer* renderer, RGBA_t n);
void drawLine (drawNode_t* node, SDL_Renderer* renderer, RGBA_t n);
void renderDrawable (drawable_t* drawable, SDL_Renderer* renderer);

layer_t* createLayer (int flags, layer_t* head);
void freeLayer (layer_t* layer);


#endif//MAP_TRACE_H