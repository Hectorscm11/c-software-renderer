#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "types.h"
#include "math3d.h"
#include "render.h"

#define WIDTH 1400
#define HEIGHT 900

#define PIXELS_PER_POINT 250

#define SWAP(type, a, b) do { type temp = a; a = b; b = temp; } while (0)

char is_dragging = 0;
int last_mouse_x = 0;
int last_mouse_y = 0;
float angle_x = 0.0f;
float angle_y = 0.0f;


int main(int argc, char* argv[]){
    //clock_t t_init, t_final;
    //double sec;

    char debug = 0;
    if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
        debug = 1;
    }



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

    uint32_t* pixels = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    float* z_buffer = (float*)malloc(WIDTH * HEIGHT * sizeof(float)); //buffer that allocate de depth of each pixel

    SDL_Texture* color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );

    vec3 camera_pos = (vec3){0, 0, 0};


    figure cube;
    cube.vertices = (point[]){
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  
    };
    cube.transformed_vertices = (point[]){
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  
    };
    cube.triangles = (triangle[]){
        //front face (z = -1)
        {0, 1, 2, 1, 0}, {0, 2, 3, 1, 0},
        
        //back face (z = 1)
        {5, 4, 7, 1, 0}, {5, 7, 6, 1, 0},
        
        //left face (x = -1)
        {4, 0, 3, 1, 0}, {4, 3, 7, 1, 0},
        
        //rigth face (x = 1)
        {1, 5, 6, 1, 0}, {1, 6, 2, 1, 0},
        
        //upper face (y = 1)
        {3, 2, 6, 1, 0}, {3, 6, 7, 1, 0},
        
        //lower face (y = -1)
        {4, 5, 1, 1, 0}, {4, 1, 0, 1, 0}
    };
    cube.edges = (edge[]){
        //front face
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        
        //back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        
        //conecting edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    cube.n_triangles = 12;
    cube.n_edges = 12;
    cube.n_vertices = 8;
    cube.position = (point){0, 0, 0};
    cube.angle_x = 0;
    cube.angle_y = 0;





    //long long frames = 0;

    uint32_t last_time = SDL_GetTicks(); 
    uint32_t current_time;
    uint32_t frame_count = 0;

    int delta_x;
    int delta_y;

    int running = 1;
    SDL_Event event;

    //t_init = clock();

    while (running) {
        //events
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_QUIT) running = 0; //close window
        else if (event.type == SDL_MOUSEBUTTONDOWN) { //mouse left click down
            if (event.button.button == SDL_BUTTON_LEFT) {
                is_dragging = 1;
                last_mouse_x = event.button.x;
                last_mouse_y = event.button.y;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) { //mouse left click up
            if (event.button.button == SDL_BUTTON_LEFT) {
                is_dragging = 0;
            }
        }
        else if (event.type == SDL_MOUSEMOTION) { //mouse movement
            if (is_dragging) {
                delta_x = event.motion.x - last_mouse_x;
                delta_y = event.motion.y - last_mouse_y;

                cube.angle_y -= delta_x * 0.01f; 
                cube.angle_x += delta_y * 0.01f; 

                last_mouse_x = event.motion.x;
                last_mouse_y = event.motion.y;
            }
        }


        }


        for (int i = 0; i < WIDTH * HEIGHT; i++){
            pixels[i] = 0xFF000000;
            z_buffer[i] = 0;
        } 
      


        rotate_figure(&cube);

        calc_triangle_aliniation(&cube, camera_pos);

        draw_triangles(pixels, z_buffer, &cube);

        if(debug) draw_triangles_edges(pixels, &cube);

        //draw_edges(pixels, cube.transformed_vertices, cube.edges, cube.n_edges, 0xFFFF0000);

        //update window
        SDL_UpdateTexture(color_buffer_texture, NULL, pixels, WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        frame_count++;

        current_time = SDL_GetTicks();

        if(current_time - last_time >= 1000){
            printf("FPS:%d\n",frame_count);
            frame_count = 0;
            last_time = current_time;
        }

        //frames++;
    }

    //t_final = clock();

    //sec = (double)(t_final - t_init) / CLOCKS_PER_SEC;

    //long long frames_per_sec = frames / (long long)sec;

    free(pixels);
    free(z_buffer);

    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);


    SDL_DestroyWindow(window);
    SDL_Quit();

    //printf("Frames/sec = %lld\n", frames_per_sec);

    return 0;
}