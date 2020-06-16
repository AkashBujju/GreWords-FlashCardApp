#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture1;

void main() {
	vec4 text_color = texture(texture1, TexCoords);
	if(text_color.a < 0.1)
		discard;

	FragColor = text_color;
}
