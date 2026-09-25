#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>

typedef enum
{
    RENDER_OK = 0,

    RENDER_INIT_FAILED,
    RENDER_WINDOW_CREATE_FAILED,
} RenderError;

struct RenderData
{
    SDL_Window* window;
    SDL_Renderer* renderer;
};
typedef struct RenderData RenderData;

RenderError rendererInit(RenderData** data, uint32_t window_width, uint32_t window_height, const char* window_title);
void rendererDestroy(RenderData* data);

void rendererClear(RenderData* data, float r, float g, float b);
void rendererDisplay(RenderData* data);

#endif