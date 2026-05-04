#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "load.h"

int load_model(figure* figure, char* file){
    FILE* fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo '%s'\n", file);
        return -1;
    }

    char line[256];
    figure->n_vertices = 0;
    figure->n_triangles = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "v ", 2) == 0) {
            figure->n_vertices++;
        } else if (strncmp(line, "f ", 2) == 0) {
            figure->n_triangles++;
        }
    }

    figure->vertices = (vec3*)malloc(figure->n_vertices * sizeof(vec3));
    figure->transformed_vertices = (vec3*)malloc(figure->n_vertices * sizeof(vec3));
    figure->triangles = (triangle*)malloc(figure->n_triangles * sizeof(triangle));

    if (!figure->vertices || !figure->transformed_vertices || !figure->triangles) {
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
            v_idx++;
        }

        else if (strncmp(line, "f ", 2) == 0) {
            int a, b, c, d_vt, d_vn;

            sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
                       &a, &d_vt, &d_vn, 
                       &b, &d_vt, &d_vn, 
                       &c, &d_vt, &d_vn);
        
            
            figure->triangles[t_idx].a = a - 1;
            figure->triangles[t_idx].b = b - 1;
            figure->triangles[t_idx].c = c - 1;

            figure->triangles[t_idx].visible = 0;
            figure->triangles[t_idx].aliniation = 0.0f;
            
            t_idx++;
        }
    }

    figure->n_edges = 0;
    figure->edges = NULL;
    figure->n_triangles = t_idx;
    figure->n_vertices = v_idx;
    figure->position = (point){0.0f, 0.0f, 0.0f};
    figure->angle_x = 0.0f;
    figure->angle_y = 0.0f;


    fclose(fp);
    return 0;
}