#include <SDL2/SDL.h>
#include "MapTrace.h"

void addNode(struct drawable_t* drawable, float x, float y){
    struct drawNode_t *newNode = malloc(sizeof(struct drawNode_t));
    if (newNode == NULL)
        return;
    newNode->x = x;
    newNode->y = y;
    newNode->next = NULL;

    if (drawable->head == NULL){
        drawable->head = newNode;
        drawable->tail = newNode;
        return;
    }

    drawable->tail->next = newNode;
    drawable->tail = newNode;

    return;
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

void freeDrawable(drawable_t* d){
    if(d->head == NULL || d->tail == NULL){
        free(d);
        return;
    }

    freeDrawNodes(d->head);
    if(d->texture){
        SDL_DestroyTexture(d->texture);
    }
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