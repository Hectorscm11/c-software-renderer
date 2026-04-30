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

typedef struct triangle{
    int a;
    int b;
    int c;
}triangle;

typedef struct edge{
    int a;
    int b;
}edge;

typedef struct figure{
    point* vertices;
    triangle* triangles;
    int n_triangles;
    edge* edges;
    int n_edges;
    point position;
    float angle_x;
    float angle_y;
}figure;




int is_dragging = 0;
int last_mouse_x = 0;
int last_mouse_y = 0;
float angle_x = 0.0f;
float angle_y = 0.0f;


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color);
void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);
int project(point *v, point* p_v);
void draw_edges(uint32_t* pixels, point* vertices, edge* edges,int n_edges, uint32_t color);
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


    figure cube;
    cube.vertices = (point[]){
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  
    };
    cube.triangles = (triangle[]){
        //front face (z = -1)
        {0, 1, 2}, {0, 2, 3},
        
        //back face (z = 1)
        {5, 4, 7}, {5, 7, 6},
        
        //left face (x = -1)
        {4, 0, 3}, {4, 3, 7},
        
        //rigth face (x = 1)
        {1, 5, 6}, {1, 6, 2},
        
        //upper face (y = 1)
        {3, 2, 6}, {3, 6, 7},
        
        //lower face (y = -1)
        {4, 5, 1}, {4, 1, 0}
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
    cube.position = (point){0, 0, 0};
    cube.angle_x = 0;
    cube.angle_y = 0;





    //long long frames = 0;

    uint32_t last_time = SDL_GetTicks(); 
    uint32_t current_time;
    uint32_t frame_count = 0;
    point transformed_points[100];

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


        for (int i = 0; i < WIDTH * HEIGHT; i++) pixels[i] = 0xFF000000;
      


        for(int i = 0; i < 8; i++){
            transformed_points[i] = rotate_point(cube.vertices[i], cube.angle_x, cube.angle_y);
            transformed_points[i].z += 3.0f;
        }
        draw_edges(pixels, transformed_points, cube.edges, cube.n_edges, 0xFFFF0000);

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

void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, uint32_t color){
    point projected_points[100];
    for(int i = 0; i < 8; i++){
        project(&vertices[i], &projected_points[i]);
        projected_points[i].z += 3.0f;
    }

    for(int i = 0; i < n_edges; i++){
        draw_line(pixels, &projected_points[edges[i].a], &projected_points[edges[i].b],color);
    }

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