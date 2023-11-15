#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define ASSERT(_e, ...) if(!(_e)) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define WINDOW_WIDTH 860
#define WINDOW_HEIGHT 640
#define PIXEL_THRESHOLD 10

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
}drawable_t;

/*typedef struct layer_t {

} layer;*/

void addNode(struct drawNode_t** head, float x, float y){
    struct drawNode_t *newNode = malloc(sizeof(struct drawNode_t));
    newNode->x = x;
    newNode->y = y;
    newNode->next = NULL;

    if (*head == NULL){
        *head = newNode;
        return;
    }

    struct drawNode_t* curr = *head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = newNode;
}

void freeDrawNodes(drawNode_t* head){
    if(head == NULL){
        return;
    }
    if(head->next != NULL){
        freeDrawNodes(head->next);
    }
    free(head);
}

void drawPoint(struct drawNode_t* node, SDL_Renderer* renderer, RGB_t n){
    SDL_SetRenderDrawColor(renderer, 44, 100, 150, 255);
    SDL_RenderDrawPoint(renderer, node->x, node->y);
    if(node->next == NULL){
        return;
    }
    drawPoint(node->next, renderer, n);
}

void drawLine(struct drawNode_t* node, SDL_Renderer* renderer, RGB_t n){
    SDL_SetRenderDrawColor(renderer, n.r, n.g, n.b, n.a);
    if(node->next == NULL){
        return;
    }
    SDL_RenderDrawLine(renderer, node->x, node->y, node->next->x, node->next->y);
    drawLine(node->next, renderer, n);
}

void renderDrawable(struct drawable_t* drawable, SDL_Renderer* renderer){
    if(drawable->head == NULL){
        return;
    }
    drawLine(drawable->head, renderer, drawable->color);
    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv){
    ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), "Failed to initialize sdl, error thrown: %s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("MapTrace", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(window, "Failed to create window, error thrown: %s\n", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    ASSERT(renderer, "failed to create renderer, error thrown: %s\n", SDL_GetError());

    bool quit = false;
    SDL_Event event;

    struct drawable_t* blob = malloc(sizeof(struct drawable_t));
    blob->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    blob->resX = 0;
    blob->resY = 0;
    blob->head = NULL;
    blob->color.a = 255;
    blob->color.r = 100;
    blob->color.g = 100;
    blob->color.b = 100;


    int mx = 0;
    int oldmx = 0;
    int my = 0;
    int oldmy = 0;
    int pixT = PIXEL_THRESHOLD;
    bool isDragging = false;

    while(!quit){
        if(SDL_WaitEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    isDragging = true;
                    SDL_GetMouseState(&mx, &my);
                    oldmx = mx;
                    oldmy = my;
                    addNode(&blob->head, (float)mx, (float)my);
                    break;
                case SDL_MOUSEBUTTONUP:
                    isDragging = false;
                    break;
                case SDL_MOUSEMOTION:
                    if(isDragging){
                        SDL_GetMouseState(&mx, &my);
                        if((oldmx - mx > pixT || oldmx - mx < -pixT) || (oldmy - my > pixT || oldmy - my < -pixT)){
                            addNode(&blob->head, (float)mx, (float)my);
                            oldmx = mx;
                            oldmy = my;
                        }
                        renderDrawable(blob, renderer);
                    }
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_SPACE){
                        renderDrawable(blob, renderer);
                    }
                    break;
            }
        }
    }

    freeDrawNodes(blob->head);
    free(blob);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}