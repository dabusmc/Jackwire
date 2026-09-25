#include "renderer.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3_image/SDL_image.h>

#define MAX_TEXTURES 128

RenderError rendererInit(RenderData** data, uint32_t window_width, uint32_t window_height, const char* window_title)
{
    RenderData* d = (RenderData*)malloc(sizeof(RenderData));
    if(d == NULL)
    {
        return RENDER_INIT_FAILED;
    }

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

    d->loaded_texture_count = 0;
    d->loaded_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * MAX_TEXTURES);
    if(d->loaded_textures == NULL)
    {
        free(d);
        return RENDER_INIT_FAILED;
    }

    *data = d;
    return RENDER_OK;
}

void rendererDestroy(RenderData* data)
{
    for(int i = 0; i < data->loaded_texture_count; i++)
    {
        SDL_DestroyTexture(data->loaded_textures[i]);
    }

    SDL_DestroyWindow(data->window);
    SDL_Quit();

    free(data);
}

RenderError rendererLoadTexture(RenderData* data, int* texture_index, const char* name)
{
    char path[256];
    snprintf(path, sizeof(path), "res/images/%s", name);

    SDL_Texture* texture = IMG_LoadTexture(data->renderer, path);
    if (texture == NULL)
    {
        printf("Failed to load texture: %s\n", SDL_GetError());
        return RENDER_TEXTURE_CREATE_FAILED;
    }

    data->loaded_textures[data->loaded_texture_count] = texture;
    data->loaded_texture_count += 1;
    *texture_index = data->loaded_texture_count - 1;

    return RENDER_OK;
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

void rendererDrawTexture(RenderData* data, int texture_index)
{
    if(texture_index < 0 || texture_index >= data->loaded_texture_count)
    {
        return;
    }

    SDL_Texture* to_draw = data->loaded_textures[texture_index];
    SDL_RenderTexture(data->renderer, to_draw, NULL, NULL);
}