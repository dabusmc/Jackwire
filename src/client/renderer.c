#include "renderer.h"

#include <stdlib.h>
#include <stdio.h>

RenderError rendererInit(RenderData** data, uint32_t window_width, uint32_t window_height, const char* window_title)
{
    RenderData* d = (RenderData*)malloc(sizeof(RenderData));

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        free(d);
        return RENDER_INIT_FAILED;
    }

    if(!SDL_CreateWindowAndRenderer(window_title, window_width, window_height, 0, &(d->window), &(d->renderer)))
    {
        printf("Window/Renderer creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(d);
        return RENDER_WINDOW_CREATE_FAILED;
    }

    *data = d;
    return RENDER_OK;
}

void rendererDestroy(RenderData* data)
{
    SDL_DestroyWindow(data->window);
    SDL_Quit();

    free(data);
}

void rendererClear(RenderData* data, float r, float g, float b)
{
    SDL_SetRenderDrawColorFloat(data->renderer, r, g, b, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(data->renderer);
}

void rendererDisplay(RenderData* data)
{
    SDL_RenderPresent(data->renderer);
}