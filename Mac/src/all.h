#ifndef ALL_H
#define ALL_H

typedef struct AllTextures {
	unsigned int background_texture;
} AllTextures;

typedef struct AllShaders {
	unsigned int text_shader;
	unsigned int rect_2d_shader;
} AllShaders;

void make_all_textures(AllTextures *all_textures);
void make_all_shaders(AllShaders *all_shaders);

#endif
