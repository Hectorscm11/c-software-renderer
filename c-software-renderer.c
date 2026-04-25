#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 1400
#define HEIGHT 900

typedef struct point{
    int x;
    int y;
}point;


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color);
void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);



int main(void){
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Ventana SDL2", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        
    if (!window) {
        printf("Error al crear ventana: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    uint32_t pixels[WIDTH * HEIGHT];

    SDL_Texture* color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );


    point a;
    a.x =300;
    a.y=300;

    point b;
    b.x =50;
    b.y=250;


    int running = 1;
    SDL_Event event;

    while (running) {
        //events (close window)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        for (int i = 0; i < WIDTH * HEIGHT; i++) pixels[i] = 0xFF000000;


      

        draw_line(pixels, &a, &b, 0xFFFF0000);

        //update window
        SDL_UpdateTexture(color_buffer_texture, NULL, pixels, WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);


    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        pixels[y * WIDTH + x] = color;
    }
}


void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color){
   int x0 = a->x;
    int y0 = a->y;
    int x1 = b->x;
    int y1 = b->y;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int e2;
    while(1){
        draw_pixel(pixels, x0, y0, color);

        if(x0 == x1 && y0 == y1) break;

        e2 = 2 *err;

        if(e2 > -dy){
            err -=dy;
            x0 +=sx;
        }

        if(e2 < dx){
            err += dx;
            y0 +=sy;
        }
      
    }


}