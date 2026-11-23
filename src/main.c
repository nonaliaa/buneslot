#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

bool draw_img(SDL_Window *window, SDL_Surface *image) 
{
    if (!window) {
        fprintf(stderr, "%s: invalid window\n", __func__);
        return false;
    }

    if (!image) {
        fprintf(stderr, "%s: invalid image\n", __func__);
        return false;
    }

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if (!window_surface) {
        fprintf(stderr, "%s: failed to get window surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_ClearSurface(window_surface, 72, 92, 107, 0.8)) {
        fprintf(stderr, "%s: failed to clear window_surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_BlitSurfaceScaled(image, NULL, window_surface, NULL, SDL_SCALEMODE_LINEAR)) {
        fprintf(stderr, "%s: failed to blit surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_UpdateWindowSurface(window)) {
        fprintf(stderr, "%s: failed to update window surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s: <path>\n", argv[0]);
        return 1;
    }

    char *img_path = realpath(argv[1], NULL);
    if (!img_path) {
        perror("realpath");
        return 1;
    }

    SDL_Surface *img_surface = IMG_Load(img_path);
    if (!img_surface) {
        fprintf(stderr, "failed to load img_surface: %s\n", SDL_GetError());
        goto quit_free;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    char *window_name = strrchr(img_path, '/');
    if (window_name) {
        window_name += 1; /* step over the separator */
    } else {
        fprintf(stderr, "no separator?\n");
        window_name = img_path;
    }

    SDL_Window *window = SDL_CreateWindow(window_name, img_surface->w, img_surface->h, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
        goto error_sdl;
    }

    if (!draw_img(window, img_surface)) {
        fprintf(stderr, "failed initial draw\n");
        goto error_sdl;
    }

    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));

    while(1) {
        while(SDL_WaitEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                goto exit_sdl; /* keep sdl from hanging when you quit? */
            }

            if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED || 
                event.window.type == SDL_EVENT_WINDOW_RESIZED ||
                event.window.type == SDL_EVENT_WINDOW_MOVED) {
                    if (!draw_img(window, img_surface)) {
                        fprintf(stderr, "failed to redraw\n");
                        goto error_sdl;
                    }
            }
        }
    }

    exit_sdl:
    SDL_DestroyWindow(window);
    SDL_DestroySurface(img_surface);
    free(img_path);
    return 0;

    error_sdl:
    SDL_DestroyWindow(window);
    SDL_DestroySurface(img_surface);
    quit_free:
    free(img_path);
    return 1;
}