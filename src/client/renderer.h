#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>

typedef enum
{
    RENDER_OK = 0,

    RENDER_INIT_FAILED,
    RENDER_WINDOW_CREATE_FAILED,
    RENDER_TEXTURE_CREATE_FAILED
} RenderError;

struct RenderData
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture** loaded_textures;
    int loaded_texture_count;
};
typedef struct RenderData RenderData;

RenderError rendererInit(RenderData** data, uint32_t window_width, uint32_t window_height, const char* window_title);
void rendererDestroy(RenderData* data);

RenderError rendererLoadTexture(RenderData* data, int* texture_index, const char* name);

void rendererClear(RenderData* data, float r, float g, float b);
void rendererDisplay(RenderData* data);
void rendererDrawTexture(RenderData* data, int texture_index);

#endif