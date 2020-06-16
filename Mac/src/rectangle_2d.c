#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h> // @TMP
#include "rectangle_2d.h"
#include "utils.h"
#include "shader.h"

extern const char* assets_path;

void make_rectangle_2d(Rectangle2D *rectangle_2d, int program, unsigned int texture) {
	rectangle_2d->program = program;

	float *vertices = (float*)malloc(sizeof(float) * 30);
	read_floats_from_file(combine_string(assets_path, "vertices/rectangle_2d_vertices.dat"), vertices);

	glGenVertexArrays(1, &rectangle_2d->vao);
	glGenBuffers(1, &rectangle_2d->vbo);
	glBindVertexArray(rectangle_2d->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_2d->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 30, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	rectangle_2d->texture_id = texture;
	init_matrix(&rectangle_2d->model);
	init_vector2(&rectangle_2d->position, 0, 0);
	init_vector2(&rectangle_2d->scale, 1, 1);
}

void draw_rectangle_2d(Rectangle2D *rectangle_2d) {
	glUseProgram(rectangle_2d->program);
	make_identity(&rectangle_2d->model);

	translate_matrix(&rectangle_2d->model, 0, 0, 0);
	scale(&rectangle_2d->model, rectangle_2d->scale.x, rectangle_2d->scale.y, 1);
	translate_matrix(&rectangle_2d->model, rectangle_2d->position.x, rectangle_2d->position.y, -0.001f);
	set_matrix4(rectangle_2d->program, "model", &rectangle_2d->model);

	glBindTexture(GL_TEXTURE_2D, rectangle_2d->texture_id);
	glBindVertexArray(rectangle_2d->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void delete_rectangle_2d(Rectangle2D *rectangle_2d) {
	glDeleteVertexArrays(1, &rectangle_2d->vao);
	glDeleteBuffers(1, &rectangle_2d->vbo);
}
