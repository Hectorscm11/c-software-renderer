#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "load.h"

int load_model(figure* figure, point** transformed_vertices, char* file, float scale_factor){
    FILE* fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Error: File not found '%s'\n", file);
        return -1;
    }

    char line[256];
    figure->n_vertices = 0;
    figure->n_triangles = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "v ", 2) == 0) {
            figure->n_vertices++;
        } else if (strncmp(line, "f ", 2) == 0) {
            
            int v_count = 0;
            char temp_line[256];
            strcpy(temp_line, line); 
            
            char *token = strtok(temp_line + 2, " \r\n");
            while(token != NULL) {
                v_count++;
                token = strtok(NULL, " \r\n");
            }
            
            if (v_count == 3) {
                figure->n_triangles += 1; //triangle
            } else if (v_count == 4) {
                figure->n_triangles += 2; //quad
            }
        }
    }

    figure->vertices = (vec3*)malloc(figure->n_vertices * sizeof(vec3));
    *transformed_vertices = (point*)malloc(figure->n_vertices * sizeof(point));
    figure->triangles = (triangle*)malloc(figure->n_triangles * sizeof(triangle));

    if (!figure->vertices || !transformed_vertices || !figure->triangles) {
        printf("Error: Memory management failed.\n");
        fclose(fp);
        return -2;
    }

    rewind(fp);

    int v_idx = 0;
    int t_idx = 0;


    while (fgets(line, sizeof(line), fp)) {
        
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", 
                   &figure->vertices[v_idx].x, 
                   &figure->vertices[v_idx].y, 
                   &figure->vertices[v_idx].z);

            figure->vertices[v_idx].x *= scale_factor;
            figure->vertices[v_idx].y *= scale_factor;
            figure->vertices[v_idx].z *= scale_factor;

            v_idx++;
        }
        else if (strncmp(line, "f ", 2) == 0) {
            int vertices[4]; 
            int v_count = 0;
            
            char *token = strtok(line + 2, " \r\n");
            while(token != NULL && v_count < 4) {
                sscanf(token, "%d", &vertices[v_count]); 
                v_count++;
                token = strtok(NULL, " \r\n");
            }

            
            if (v_count >= 3) {
                figure->triangles[t_idx].a = vertices[0] - 1;
                figure->triangles[t_idx].b = vertices[1] - 1;
                figure->triangles[t_idx].c = vertices[2] - 1;
                figure->triangles[t_idx].visible = 0;
                figure->triangles[t_idx].aliniation = 0.0f;
                t_idx++;
            }
            
            if (v_count == 4) {
                figure->triangles[t_idx].a = vertices[0] - 1;
                figure->triangles[t_idx].b = vertices[2] - 1; // Diagonal
                figure->triangles[t_idx].c = vertices[3] - 1;
                figure->triangles[t_idx].visible = 0;
                figure->triangles[t_idx].aliniation = 0.0f;
                t_idx++;
            }
        }

    }

    figure->n_edges = 0;
    figure->edges = NULL;
    figure->n_triangles = t_idx;
    figure->n_vertices = v_idx;
    

    fclose(fp);
    return 0;
}