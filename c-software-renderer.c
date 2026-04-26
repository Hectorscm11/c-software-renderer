#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 1400
#define HEIGHT 900

#define PIXELS_PER_POINT 250

typedef struct point{
    float x;
    float y;
    float z;
}point;


point cube[8] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  
};

int is_dragging = 0;
int last_mouse_x = 0;
int last_mouse_y = 0;
float angle_x = 0.0f;
float angle_y = 0.0f;


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color);
void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);
int project(point *v, point* p_v);
void draw_cube(uint32_t* pixels, point* cube, uint32_t color);
point rotate_point(point p, float angle_x, float angle_y);


int main(void){
    //clock_t t_init, t_final;
    //double sec;

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



    //long long frames = 0;

    uint32_t last_time = SDL_GetTicks(); 
    uint32_t current_time;
    uint32_t frame_count = 0;
    point cube_transformed[8];

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

                angle_y -= delta_x * 0.01f; 
                angle_x += delta_y * 0.01f; 

                last_mouse_x = event.motion.x;
                last_mouse_y = event.motion.y;
            }
        }


        }


        for (int i = 0; i < WIDTH * HEIGHT; i++) pixels[i] = 0xFF000000;
      


        for(int i = 0; i < 8; i++){
            cube_transformed[i] = rotate_point(cube[i], angle_x, angle_y);
            cube_transformed[i].z += 3.0f;
        }
        draw_cube(pixels, cube_transformed, 0xFFFF0000);

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

    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);


    SDL_DestroyWindow(window);
    SDL_Quit();

    //printf("Frames/sec = %lld\n", frames_per_sec);

    return 0;
}


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        pixels[y * WIDTH + x] = color;
    }
}


void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color){
    //Bresenham algorithm
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


int project(point* v, point* p_v) {
    if(v->z == 0){
        return 1;
    }
    
    p_v->x = (v->x / v->z) * PIXELS_PER_POINT + (WIDTH / 2);
    p_v->y = (v->y / v->z) * PIXELS_PER_POINT + (HEIGHT / 2);

    return 0;
}

void draw_cube(uint32_t* pixels, point* cube, uint32_t color){
    point cube_projected[8];
    for(int i = 0; i < 8; i++){
        project(&cube[i], &cube_projected[i]);
    }

    //front face
    draw_line(pixels, &cube_projected[0], &cube_projected[1], color);
    draw_line(pixels, &cube_projected[1], &cube_projected[2], color);
    draw_line(pixels, &cube_projected[2], &cube_projected[3], color);
    draw_line(pixels, &cube_projected[3], &cube_projected[0], color);

    //back face
    draw_line(pixels, &cube_projected[4], &cube_projected[5], 0xFF0000FF);
    draw_line(pixels, &cube_projected[5], &cube_projected[6], 0xFF0000FF);
    draw_line(pixels, &cube_projected[6], &cube_projected[7], 0xFF0000FF);
    draw_line(pixels, &cube_projected[7], &cube_projected[4], 0xFF0000FF);

    //faces conexions
    draw_line(pixels, &cube_projected[0], &cube_projected[4], 0xFF00FF00);
    draw_line(pixels, &cube_projected[1], &cube_projected[5], 0xFF00FF00);
    draw_line(pixels, &cube_projected[2], &cube_projected[6], 0xFF00FF00);
    draw_line(pixels, &cube_projected[3], &cube_projected[7], 0xFF00FF00);
}


point rotate_point(point p, float angle_x, float angle_y){
    point rotated = p;

    float cos_x = cos(angle_x);
    float sin_x = sin(angle_x);
    rotated.y = rotated.y * cos_x - rotated.z * sin_x;
    rotated.z = p.y * sin_x + rotated.z * cos_x;

    float cos_y = cos(angle_y);
    float sin_y = sin(angle_y);
    rotated.x = rotated.x * cos_y - rotated.z * sin_y;
    rotated.z = p.x * sin_y + rotated.z *cos_y;
    
    return rotated;
}