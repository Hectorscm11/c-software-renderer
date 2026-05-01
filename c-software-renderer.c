#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 1400
#define HEIGHT 900

#define PIXELS_PER_POINT 250

#define SWAP(type, a, b) do { type temp = a; a = b; b = temp; } while (0)

typedef struct point{
    float x;
    float y;
    float z;
}point, vec3;

typedef struct triangle{
    int a;
    int b;
    int c;
    char visible;
}triangle;

typedef struct edge{
    int a;
    int b;
}edge;

typedef struct figure{
    point* vertices;
    point* transformed_vertices;
    int n_vertices;
    triangle* triangles;
    int n_triangles;
    edge* edges;
    int n_edges;
    point position;
    float angle_x;
    float angle_y;
}figure;




char is_dragging = 0;
int last_mouse_x = 0;
int last_mouse_y = 0;
float angle_x = 0.0f;
float angle_y = 0.0f;


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color);
void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);
int project(point *v, point* p_v);
void draw_triangles_edges(uint32_t* pixels, figure* figure);
void draw_edges(uint32_t* pixels, point* vertices, edge* edges,int n_edges, uint32_t color);
point rotate_point(point p, float angle_x, float angle_y);
void rotate_figure(figure* figure);
void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure, uint32_t color);
void draw_triangle(uint32_t* pixels, float* z_buffer, figure* figure, triangle* triangle, uint32_t color);
void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color);
static inline float interpolate(int y, int y0, int y1, float v0, float v1);
void calc_triangle_visibility(figure* figure, vec3 camera_pos);
vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_cross(vec3 u, vec3 v);
float vec3_dot(vec3 v1, vec3 v2);


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
        {0, 1, 2, 1}, {0, 2, 3, 1},
        
        //back face (z = 1)
        {5, 4, 7, 1}, {5, 7, 6, 1},
        
        //left face (x = -1)
        {4, 0, 3, 1}, {4, 3, 7, 1},
        
        //rigth face (x = 1)
        {1, 5, 6, 1}, {1, 6, 2, 1},
        
        //upper face (y = 1)
        {3, 2, 6, 1}, {3, 6, 7, 1},
        
        //lower face (y = -1)
        {4, 5, 1, 1}, {4, 1, 0, 1}
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

        calc_triangle_visibility(&cube, camera_pos);

        draw_triangles(pixels, z_buffer, &cube, 0xFFFF0000);

        draw_triangles_edges(pixels, &cube);

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


static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color){
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

void draw_triangles_edges(uint32_t* pixels,figure* figure){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1){
                
            point vA = figure->transformed_vertices[figure->triangles[i].a];
            point vB = figure->transformed_vertices[figure->triangles[i].b];
            point vC = figure->transformed_vertices[figure->triangles[i].c];

            point pA, pB, pC;
            if (project(&vA, &pA) == 0 && 
                project(&vB, &pB) == 0 && 
                project(&vC, &pC) == 0) {
                
                draw_line(pixels, &pA, &pB, 0xFF00FF00); 
                draw_line(pixels, &pB, &pC, 0xFF00FF00);
                draw_line(pixels, &pC, &pA, 0xFF00FF00);
            }
        }
    }
}


int project(point* v, point* p_v) {
    if(v->z == 0){
        return -1;
    }
    
    p_v->x = (v->x / v->z) * PIXELS_PER_POINT + (WIDTH / 2);
    p_v->y = (v->y / v->z) * PIXELS_PER_POINT + (HEIGHT / 2);
    p_v->z = v->z;

    return 0;
}

void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, uint32_t color){
    point projected_points[100];
    for(int i = 0; i < 8; i++){
        if (project(&vertices[i], &projected_points[i]) == 0) {
            projected_points[i].z += 3.0f;
        }
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

void rotate_figure(figure* figure){
    for(int i = 0; i < figure->n_vertices; i++){
        figure->transformed_vertices[i] = rotate_point(figure->vertices[i], figure->angle_x, figure->angle_y);
        figure->transformed_vertices[i].z += 3.0f;
    }
}

void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color){
    int total_width = x_rigth - x_left;
    float t, inv_z_pixel;
    for(int i = x_left; i < x_rigth; i++){
        if(total_width == 0) t = 0;
        else t = (float)(i - x_left) / total_width;

        inv_z_pixel = inv_z_left + (t * (inv_z_rigth - inv_z_left));

        if(i >= 0 && i < WIDTH && y >= 0 && y < HEIGHT) {
            if(inv_z_pixel > z_buffer[(y * WIDTH) + i]) {
            draw_pixel(pixels, i, y, color);
            z_buffer[(y * WIDTH) + i] = inv_z_pixel;
            }
        }
    }
}

void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure, uint32_t color){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1) draw_triangle(pixels, z_buffer, figure, &figure->triangles[i], color);
    }
}

void draw_triangle(uint32_t* pixels, float* z_buffer, figure* figure, triangle* triangle, uint32_t color){

    point p0, p1, p2;
    if (project(&figure->transformed_vertices[triangle->a], &p0) != 0 ||
        project(&figure->transformed_vertices[triangle->b], &p1) != 0 ||
        project(&figure->transformed_vertices[triangle->c], &p2) != 0) {
        return; 
    }


    float inv_z0 = 1.0 / figure->transformed_vertices[triangle->a].z;
    float inv_z1 = 1.0 / figure->transformed_vertices[triangle->b].z;
    float inv_z2 = 1.0 / figure->transformed_vertices[triangle->c].z;

    if (p0.y > p1.y) {
        SWAP(point, p0, p1);
        SWAP(float, inv_z0, inv_z1); 
    }
    if (p0.y > p2.y) {
        SWAP(point, p0, p2);
        SWAP(float, inv_z0, inv_z2);
    }
    if (p1.y > p2.y) {
        SWAP(point, p1, p2);
        SWAP(float, inv_z1, inv_z2);
    }

    int total_height = p2.y - p0.y;
    if (total_height == 0) return;

    for(int i = p0.y; i <= p2.y; i++){

        int is_second_half = (i > p1.y) || (p1.y == p0.y);

        int segment_height = is_second_half ? (p2.y - p1.y) : (p1.y - p0.y);
        if (segment_height == 0) continue;

        int x_long = (int)interpolate(i, p0.y, p2.y, p0.x, p2.x);
        float inv_z_long = interpolate(i, p0.y, p2.y, inv_z0, inv_z2);

        int x_sort;
        float inv_z_sort;

        if (!is_second_half) {
            //upper part
            x_sort = (int)interpolate(i, p0.y, p1.y, p0.x, p1.x);
            inv_z_sort = interpolate(i, p0.y, p1.y, inv_z0, inv_z1);
        }
        else {
            //lower part
            x_sort = (int)interpolate(i, p1.y, p2.y, p1.x, p2.x);
            inv_z_sort = interpolate(i, p1.y, p2.y, inv_z1, inv_z2);
        }


        int x_left = x_long;
        int x_rigth = x_sort;
        float inv_z_left = inv_z_long;
        float inv_z_rigth = inv_z_sort;


        if (x_left > x_rigth) {
            SWAP(int, x_left, x_rigth);
            SWAP(float, inv_z_left, inv_z_rigth);
        }

        draw_horizontal_line(pixels, z_buffer, i, x_left, x_rigth, inv_z_left, inv_z_rigth, color);

    }

}

static inline float interpolate(int y, int y0, int y1, float v0, float v1){
    if (y0 == y1) {
        return v0;
    }
    
    float t = (float)(y - y0) / (y1 - y0);
    
    return v0 + t * (v1 - v0);
}

void calc_triangle_visibility(figure* figure, vec3 camera_pos){
    for(int i = 0; i < figure->n_triangles; i++){
        triangle* tri = &figure->triangles[i];
        point vertex_a = figure->transformed_vertices[tri->a];
        point vertex_b = figure->transformed_vertices[tri->b];
        point vertex_c = figure->transformed_vertices[tri->c];

        vec3 u = vec3_sub((vec3)vertex_b, (vec3)vertex_a);
        vec3 v = vec3_sub(vertex_c, vertex_a);

        vec3 normal = vec3_cross(v, u);

        vec3 camera_vec = vec3_sub(vertex_a, camera_pos);

        float aliniation = vec3_dot(normal, camera_vec);

        if(aliniation < 0) tri->visible = 1;
        else tri->visible = 0;
    }
}

vec3 vec3_sub(vec3 v1, vec3 v2){
    vec3 v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    return v3;
}

vec3 vec3_cross(vec3 u, vec3 v){
    vec3 w;
    w.x = (u.y * v.z) - (u.z * v.y);
    w.y = (u.z * v.x) - (u.x * v.z);
    w.z = (u.x * v.y) - (u.y * v.x);
    return w;
}

float vec3_dot(vec3 v1, vec3 v2){
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}