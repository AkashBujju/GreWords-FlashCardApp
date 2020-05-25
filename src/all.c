#include "all.h"
#include "shader.h"
#include "utils.h"

extern const char* assets_path;
extern const char* shaders_path;

void make_all_textures(AllTextures *all_textures) {
	all_textures->background_texture = make_texture(combine_string(assets_path, "png/gray.png"));
}

void make_all_shaders(AllShaders *all_shaders) {
	all_shaders->text_shader = compile_shader(combine_string(shaders_path, "v_text.shader"), combine_string(shaders_path, "f_text.shader"));
	all_shaders->rect_2d_shader = compile_shader(combine_string(shaders_path, "v_rect_2d.shader"), combine_string(shaders_path, "f_rect_2d.shader"));
}
