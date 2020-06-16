#ifndef RECTANGLE_2D_H
#define RECTANGLE_2D_H

#include "math.h"

struct Rectangle2D {
	unsigned int vao;
	unsigned int vbo;
	unsigned int program;
	unsigned int texture_id;
	Vector2 position;
	Vector2 scale;
	Matrix4 model;
};
typedef struct Rectangle2D Rectangle2D;

void make_rectangle_2d(Rectangle2D *rectangle_2d, int program, unsigned int texture);
void delete_rectangle_2d(Rectangle2D *rectangle_2d);
void draw_rectangle_2d(Rectangle2D *rectangle_2d);

#endif
