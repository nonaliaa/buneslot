#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#ifdef _WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif /* _WIN32*/

bool draw_img(SDL_Renderer *renderer, SDL_Texture *image)
{
    if (!renderer) {
        fprintf(stderr, "%s: invalid renderer\n", __func__);
        return false;
    }

    if (!image) {
        fprintf(stderr, "%s: invalid image\n", __func__);
        return false;
    }

    if (!SDL_RenderClear(renderer)) {
        fprintf(stderr, "%s: failed to clear renderer: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_RenderTexture(renderer, image, NULL, NULL)) {
        fprintf(stderr, "%s: failed to render texture: %s\n", __func__, SDL_GetError());
        return false;
    }

    if (!SDL_RenderPresent(renderer)) {
        fprintf(stderr, "%s: failed to render frame: %s\n", __func__, SDL_GetError());
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

    #ifdef _WIN32
    char *window_name = strrchr(img_path, '\\');
    #else
    char *window_name = strrchr(img_path, '/');
    #endif
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

    SDL_Window *window = SDL_CreateWindow(window_name, 100, 100, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
        free(img_path);
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        free(img_path);
        return 1;
    }

    if (!SDL_SetRenderDrawColor(renderer, /*r=*/72, /*g=*/92, /*b=*/107, /*a=*/1)) {
        fprintf(stderr, "failed to set default draw color: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        free(img_path);
        return 1;
    }

    SDL_Surface *img_surface = IMG_Load(img_path);
    if (!img_surface) {
        fprintf(stderr, "failed to load %s: %s\n", img_path, SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        free(img_path);
        return 1;
    }

    SDL_Texture *img_texture = SDL_CreateTextureFromSurface(renderer, img_surface);
    if (!img_texture) {
        fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroySurface(img_surface);
        free(img_path);
        return 1;
    }

    SDL_DestroySurface(img_surface);

    if (!SDL_SetRenderLogicalPresentation(renderer, img_texture->w, img_texture->h, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        fprintf(stderr, "failed to set scaling settings: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        free(img_path);
        return 1;
    }

    printf("chosen renderer: %s\n", SDL_GetRendererName(renderer));

    int ret = 1;
    SDL_Event event;
    bool running = true;
    uint64_t current_tick = 0;
    uint64_t delta_time = 0;
    uint64_t last_second = 0;
    int fps = 0;
    int fps_title_length = strlen(window_name) + 10;
    char fps_title[fps_title_length];
    memset(&event, 0, sizeof(SDL_Event));

    while(running) {
        current_tick = SDL_GetTicks();
        fps++;

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.window.type == SDL_EVENT_WINDOW_FOCUS_GAINED ||
                event.window.type == SDL_EVENT_WINDOW_RESIZED ||
                event.window.type == SDL_EVENT_WINDOW_MOVED) {
                    if (!draw_img(renderer, img_texture)) {
                        goto fini;
                    }
            }
        }

        if (current_tick > last_second + 1000) {
            printf("cheep %.2lu\n", current_tick / 1000);
            snprintf(fps_title, fps_title_length, "%s: %d", window_name, fps);
            SDL_SetWindowTitle(window, fps_title);

            last_second = current_tick;
            fps = 0;
        }

        delta_time = SDL_GetTicks() - current_tick;

        /* 16ms is the time between frames at 60 fps
         * TODO: make this a cli arg or smth
         */
        uint64_t delay_time = 16 - delta_time;
        if (delay_time > 16) {
            delay_time = 0;
        }

        if (delta_time > 1) {
            printf("delta: %lu | delay: %lu\n", delta_time, delay_time);
        }

        SDL_Delay(delay_time);
    }

    ret = 0;

fini:
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(img_texture);
    free(img_path);

    return ret;
}
