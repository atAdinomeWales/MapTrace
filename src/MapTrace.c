#include <SDL2/SDL.h>
#include "MapTrace.h"

void addXYNode(drawable_t* drawable, float x, float y){
    drawNode_t *newNode = malloc(sizeof(struct drawNode_t));
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
    if (head == NULL){
        return;
    }
    if (head->next != NULL){
        freeDrawNodes(head->next);
    }
    free(head);
}

drawable_t* addDrawable (layer_t* layer){
    if (!layer){
        drawable_t* drawable = malloc(sizeof(drawable_t));
        drawable->head = NULL;
        drawable->tail = NULL;
        drawable->parent_layer = NULL;
        return drawable;
    }

    drawable_node* dnode = malloc(sizeof(drawable_node));
    dnode->next = NULL;
    dnode->prev = NULL;

    if (layer->head == NULL || layer->tail == NULL){
        layer->head = dnode;
        layer->tail = dnode;
    } else {
        layer->tail->next = dnode;
        dnode->prev = layer->tail;
        layer->tail = dnode;
    }

    dnode->drawable = malloc(sizeof(drawable_t));
    dnode->drawable->color = layer->default_color;
    dnode->drawable->head = NULL;
    dnode->drawable->tail = NULL;

    return dnode->drawable;
};

void freeDrawableNode(drawable_node* dnode){
    if (!dnode){
        return;
    }

    if(dnode->drawable){
        freeDrawable(dnode->drawable);
    }

    free(dnode);

    return;
}

void freeDrawableList(drawable_node* dnode){
    if (!dnode){
        return;
    }

    if(dnode->next){
        freeDrawableList(dnode->next);
    }

    if(dnode->prev){
        freeDrawableList(dnode->prev);
    }

    freeDrawableNode(dnode);

    return;
}

void freeDrawable(drawable_t* d){
    if(!d){
        return;
    }

    if (d->head == NULL || d->tail == NULL){
        free(d);
        return;
    }

    freeDrawNodes(d->head);
    if (d->texture){
        SDL_DestroyTexture(d->texture);
    }

    free(d);

    return;
}

void drawPoint(drawNode_t* node, SDL_Renderer* renderer, RGBA_t n){
    SDL_SetRenderDrawColor(renderer, 44, 100, 150, 255);
    SDL_RenderDrawPoint(renderer, node->x, node->y);
    if (node->next == NULL){
        return;
    }
    drawPoint(node->next, renderer, n);
}

void drawLine(drawNode_t* node, SDL_Renderer* renderer, RGBA_t n){
    SDL_SetRenderDrawColor(renderer, n.r, n.g, n.b, n.a);
    if (node->next == NULL){
        return;
    }
    SDL_RenderDrawLine(renderer, node->x, node->y, node->next->x, node->next->y);
    drawLine(node->next, renderer, n);
}

void renderDrawable(drawable_t* drawable, SDL_Renderer* renderer){
    if (drawable->head == NULL){
        return;
    }
    drawLine(drawable->head, renderer, drawable->color);
    SDL_RenderPresent(renderer);
}

layer_t* createLayer(int flags, layer_t* head){
    layer_t* newLayer = malloc(sizeof(layer_t));
    if (!newLayer){
        free(newLayer);
        return NULL;
    }

    newLayer->flags = flags;
    RGBA_t col = {255, 255, 255, 255};
    newLayer->default_color = col;
    newLayer->head = NULL;
    newLayer->tail = NULL;
    newLayer->next = NULL;

    if (!head){
        head = newLayer;
        return newLayer;
    }

    layer_t* curr;
    while (curr->next != NULL){
        curr = curr->next;
    }

    curr->next = newLayer;

    return newLayer;
}

void freeLayer(layer_t* layer){
    if (!layer){
        return;
    }

    if (layer->head){
        freeDrawableList(layer->head);
    } if (layer->tail){
        freeDrawableList(layer->tail);
    }

    return;
}