#define CUTE_TILED_IMPLEMENTATION
#include "map.h"

static cute_tiled_map_t* map;
static cute_tiled_layer_t* layer;
static cute_tiled_tileset_t* tileset;
static Texture* texture;

static void render(SDL_Renderer* renderer) {
    cute_tiled_layer_t* temp_layer = layer;

    while (temp_layer) {
        if (!temp_layer->data) {
            temp_layer = temp_layer->next;
            continue;
        }

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                int tile_index = temp_layer->data[y * map->width + x];
                if (tile_index == 0) continue;
                
                Texture* temp_texture = texture;
                Texture* texture_to_use = NULL;

                while (temp_texture) {
                    if (tile_index >= temp_texture->firstgid && tile_index <= temp_texture->firstgid + temp_texture->tilecount - 1) {
                        texture_to_use = temp_texture;
                        break;
                    }
                    temp_texture = temp_texture->next;
                }

                if (!texture_to_use) {
                    continue;
                }

                int tileset_columns = texture_to_use->tileset_width / map->tilewidth;

                SDL_FRect src = {
                    (tile_index - texture_to_use->firstgid) % tileset_columns * map->tilewidth,
                    (tile_index - texture_to_use->firstgid) / tileset_columns * map->tileheight,
                    map->tilewidth,
                    map->tileheight
                };

                SDL_FRect dst = {
                    x * map->tilewidth,
                    y * map->tileheight,
                    map->tilewidth,
                    map->tileheight
                };

                SDL_RenderTexture(renderer, texture_to_use->texture, &src, &dst);
            }
        }

        temp_layer = temp_layer->next;
    }
}

Entity init_map(SDL_Renderer* renderer) {
    const char map_path[] = "./tilted/map1.json";
    map = cute_tiled_load_map_from_file(map_path, NULL);


    if (!map) {
        SDL_Log("Failed to load map");
    }

    layer = map->layers;
    tileset = map->tilesets;

    texture = SDL_malloc(sizeof(Texture));

    Texture* current_texture = texture;

    while (tileset) {
        current_texture->texture = IMG_LoadTexture(renderer, tileset->image.ptr);

        if (!current_texture->texture) {
            SDL_Log("Failed to load texture");
        } else {
            SDL_Log("Texture loaded successfully");
        }

        current_texture->firstgid = tileset->firstgid;
        current_texture->tilecount = tileset->tilecount;
        current_texture->tileset_width = tileset->imagewidth;
        current_texture->tileset_height = tileset->imageheight;

        tileset = tileset->next;
        if (tileset) {
            current_texture->next = SDL_malloc(sizeof(Texture));
            current_texture = current_texture->next;
        } else {
            current_texture->next = NULL;
        }
    }

    Entity map_entity = {
        .render = render
    };

    return map_entity;
}