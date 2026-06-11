#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <float.h>

extern "C" {
    #include "types.h"
    #include "math/math3d.h"
    #include "render/render.h"
    #include "load/load.h"
}

#include "entity/entity.h"

#define WIDTH 1400
#define HEIGHT 900

#define FOV_DEGREES 90

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

    char* file_path = NULL;
    char debug = 0;
    float scale_factor = 1.0f;
    float movement_factor = 0.1f;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug = 1;
        } 
        else if (strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) {
                file_path = argv[i + 1]; 
                i++; 
            } else {
                printf("Error: You must writte a path after '--file'\n");
                printf("Correct use: ./motor [--file <ruta>] [--debug]\n");
                return 1; 
            }
        }
        else if (strcmp(argv[i], "--scale") == 0) {
        if (i + 1 < argc) {
            scale_factor = atof(argv[i + 1]); 
            i++; 
        } else {
            printf("Error: You must write a number after '--scale'\n");
            printf("Correct use: ./motor [--file <ruta>] [--scale <numero>] [--debug]\n");
            return 1; 
        }
    }
        else {
            printf("Unknown argument: %s\n", argv[i]);
        }
    }


    figure* terrain_figure = (figure*)malloc(sizeof(figure));
    Entity terrain(terrain_figure, (point){0.0f, 0.0f, 0.0f}, 0xFFFF0000, false);
    terrain.load_figure(file_path, scale_factor);
    terrain.rotate((float)PI, 0.0f);



    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error trying to inicialice SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        
    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
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


    float yaw = 90.0f;   // looking to the front
    float pitch = 0.0f;  // looking strigt
    float sensitivity = 0.1f;


    vec3 camera_pos = (vec3){0, 0, -0.5};
    vec3 camera_target = (vec3){0, 0, 0};
    vec3 camera_front = (vec3){0.0f, 0.0f, 1.0f};
    vec3 up_vector = (vec3){0, 1, 0};
    vec3 light_origin = (vec3){4 * scale_factor, -3 * scale_factor, -1 * scale_factor};

    mat4x4 proj_matrix = init_projection_matrix((float)FOV_DEGREES, (float)HEIGHT / (float)WIDTH, (float)Z_NEAR, (float)Z_FAR);

    float fov_degrees = (float)FOV_DEGREES;

    //long long frames = 0;

    uint32_t last_time = SDL_GetTicks(); 
    uint32_t current_time;
    uint32_t frame_count = 0;

    int delta_x;
    int delta_y;

    int running = 1;
    SDL_Event event;

    //t_init = clock();
    SDL_SetRelativeMouseMode(SDL_TRUE);
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

                    terrain.rotate(delta_y * 0.01f, -delta_x * 0.01f);

                    last_mouse_x = event.motion.x;
                    last_mouse_y = event.motion.y;
                    
                }else{
                    yaw   -= event.motion.xrel * sensitivity;
                    pitch += event.motion.yrel * sensitivity; 

                    if (pitch > 89.0f)  pitch = 89.0f;
                    if (pitch < -89.0f) pitch = -89.0f;


                    float yaw_rad = yaw * PI / 180.0f;
                    float pitch_rad = pitch * PI / 180.0f;

                    camera_front.x = cos(yaw_rad) * cos(pitch_rad);
                    camera_front.y = sin(pitch_rad);
                    camera_front.z = sin(yaw_rad) * cos(pitch_rad);
                }
            }else if (event.type == SDL_MOUSEWHEEL) { //mouse wheel movement
                if (event.wheel.y > 0 && fov_degrees > 30) {
                    fov_degrees--;
                    proj_matrix = init_projection_matrix((float)fov_degrees, (float)HEIGHT / (float)WIDTH, (float)Z_NEAR, (float)Z_FAR);

                } 
                else if (event.wheel.y < 0 && fov_degrees < 150) {
                    fov_degrees++;
                    proj_matrix = init_projection_matrix((float)fov_degrees, (float)HEIGHT / (float)WIDTH, (float)Z_NEAR, (float)Z_FAR);

                }
            }else if (event.type == SDL_KEYDOWN) { //keyboard inputs
                vec3 camera_right;
                camera_right.x = camera_front.y * up_vector.z - camera_front.z * up_vector.y;
                camera_right.y = camera_front.z * up_vector.x - camera_front.x * up_vector.z;
                camera_right.z = camera_front.x * up_vector.y - camera_front.y * up_vector.x;

                switch (event.key.keysym.sym) {
                    case SDLK_w: 
                        camera_pos.x += camera_front.x * movement_factor;
                        camera_pos.y += camera_front.y * movement_factor;
                        camera_pos.z += camera_front.z * movement_factor;
                        break;
                    case SDLK_s: 
                        camera_pos.x -= camera_front.x * movement_factor;
                        camera_pos.y -= camera_front.y * movement_factor;
                        camera_pos.z -= camera_front.z * movement_factor;
                        break;
                    case SDLK_a: 
                        camera_pos.x += camera_right.x * movement_factor;
                        camera_pos.z += camera_right.z * movement_factor;
                        break;
                    case SDLK_d: 
                        camera_pos.x -= camera_right.x * movement_factor;
                        camera_pos.z -= camera_right.z * movement_factor;
                        break;
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                

            }
        }

        camera_target.x = camera_pos.x + camera_front.x;
        camera_target.y = camera_pos.y + camera_front.y;
        camera_target.z = camera_pos.z + camera_front.z;

        mat4x4 mat_view = matrix_make_lookat(camera_pos, camera_target, up_vector);

        for (int i = 0; i < WIDTH * HEIGHT; i++){
            pixels[i] = 0xFF000000;
            z_buffer[i] = -FLT_MAX;
        } 
        

        terrain.draw(pixels, z_buffer, proj_matrix, mat_view, light_origin, camera_pos);

        //draw_edges(pixels, cube.transformed_vertices, cube.edges, cube.n_edges, proj_matrix, 0xFFFF0000);

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