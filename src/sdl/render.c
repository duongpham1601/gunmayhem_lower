#include "render.h"

void app_render(void *appstate) {
    AppState* state = (AppState*) appstate;

    SDL_RenderClear(state->renderer);
    SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);
    
    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].render) continue;
        entities[i].render(state->renderer);
    }

    SDL_RenderPresent(state->renderer);
}