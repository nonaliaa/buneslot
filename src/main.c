#include <stdbool.h>
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

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) {
        fprintf(stderr, "%s: failed to get window surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_ClearSurface(surface, /*r=*/72, /*g=*/92, /*b=*/107, /*a=*/1)) {
        fprintf(stderr, "%s: failed to clear surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_BlitSurfaceScaled(image, NULL, surface, NULL, SDL_SCALEMODE_LINEAR)) {
        fprintf(stderr, "%s: failed to blit surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_UpdateWindowSurface(window)) {
        fprintf(stderr, "%s: failed to update window surface: %s\n", __func__, SDL_GetError());
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
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

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
        free(img_path);
        return 1;
    }

    char *window_name = strrchr(img_path, '/');
    if (window_name) {
        /* Step over the separator. */
        window_name += 1;
    } else {
        fprintf(stderr, "no separator?\n");
        window_name = img_path;
    }

    if (atexit(SDL_Quit)) {
        fprintf(stderr, "failed to set exit function\n");
        free(img_path);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *img_surface = IMG_Load(img_path);
    if (!img_surface) {
        fprintf(stderr, "failed to load %s: %s\n", img_path, SDL_GetError());
        free(img_path);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(window_name, img_surface->w, img_surface->h, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
        SDL_DestroySurface(img_surface);
        free(img_path);
        return 1;
    }

    int ret = 1;
    if (!draw_img(window, img_surface)) {
        goto fini;
    }

    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));

    while (1) {
        if (!SDL_WaitEvent(&event)) {
            fprintf(stderr, "SDL_WaitEvent failed: %s\n", SDL_GetError());
            goto fini;
        }

        if (event.type == SDL_EVENT_QUIT) {
            break;
        }

        if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED ||
            event.window.type == SDL_EVENT_WINDOW_RESIZED ||
            event.window.type == SDL_EVENT_WINDOW_MOVED) {
                if (!draw_img(window, img_surface)) {
                    goto fini;
                }
        }
    }

    ret = 0;

fini:
    SDL_DestroyWindow(window);
    SDL_DestroySurface(img_surface);
    free(img_path);

    return ret;
}
