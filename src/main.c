#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "MapTrace.h"
#include "IoHandler.h"
#include "basics.h"

#define FILE_NAME "savefile.bin"

int main(int argc, char** argv){
    // SDL Setup
    ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), "Failed to initialize sdl, error thrown: %s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("MapTrace", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(window, "Failed to create window, error thrown: %s\n", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    ASSERT(renderer, "failed to create renderer, error thrown: %s\n", SDL_GetError());

    bool quit = false;
    SDL_Event event;


    layer_t* layer = createLayer(0, NULL);
    drawable_t* blob = addDrawable(layer);

    deserializeDrawable(FILE_NAME, blob);


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
                    addXYNode(blob, (float)mx, (float)my);
                    break;
                case SDL_MOUSEBUTTONUP:
                    isDragging = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (isDragging){
                        SDL_GetMouseState(&mx, &my);
                        if ((oldmx - mx > pixT || oldmx - mx < -pixT) || (oldmy - my > pixT || oldmy - my < -pixT)){
                            addXYNode(blob, (float)mx, (float)my);
                            oldmx = mx;
                            oldmy = my;
                        }
                        renderDrawable(blob, renderer);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE){
                        renderDrawable(blob, renderer);
                    }
                    break;
            }
        }
    }

    if (argvFind(argc, argv, "-ns") < 0){
        serializeDrawable(blob, FILE_NAME);
        printf("saved drawable\n");
    } else {
        printf("save feature disabled, no savefile generated\n");
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}