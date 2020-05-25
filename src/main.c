#define _GNU_SOURCE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "shader.h"
#include "rectangle_2d.h"
#include "text.h"
#include "all.h"

typedef struct Dictionary {
	char words[2000][25];
	char meanings[2000][50];
	unsigned int num_words;
} Dictionary;

Vector3 position, up, front;
float cursor_x;
Font font, m_font;
unsigned int window_width, window_height;
GLFWwindow *window = NULL;
unsigned int current_index;
unsigned int max_words;
uint8_t show_full;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
char* combine_string(const char* str_1, const char* str_2);
void append_char(char* str, char c);
void remove_last_char(char* text);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void render_middle(const char* text, Font *font, unsigned int shader, int x, int y, float r, float g, float b);
void load_file(Dictionary *dictionary);
void get_word_and_meaning(char* from, char *word, char *meaning);
void print_dictionary(Dictionary *dictionary);
void render_word_and_meaning(Dictionary *dictionary, unsigned int index, int shader);
void render_word(Dictionary *dictionary, unsigned int index, int shader);
int get_random_number(int max);

const char* assets_path = "../data/";
const char* shaders_path = "../shaders/";

int main(int argc, char** argv) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // @Note: On intel machines antialiasing has to be turned on.

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
	window_width = video_mode->width;
	window_height = video_mode->height;
	window = glfwCreateWindow(window_width, window_height, "Hello, World", monitor, NULL);

	glfwMaximizeWindow(window);	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1);

	Matrix4 view, projection, ortho_projection;
	AllTextures all_textures;
	AllShaders all_shaders;
	Dictionary dictionary;
	{
		make_identity(&view);
		projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
		ortho_projection = ortho(0, window_width, 0, window_height);
		init_vector(&front, 0, 0, -1.0f);
		init_vector(&position, 0, 0, 20);
		init_vector(&up, 0, 1, 0);

		make_all_textures(&all_textures);
		make_all_shaders(&all_shaders);

		FT_Library ft_1;
		init_freetype(&ft_1);
		init_font(&font, combine_string(assets_path, "fonts/consolas.ttf"), &ft_1, 64);
		FT_Library ft_2;
		init_freetype(&ft_2);
		init_font(&m_font, combine_string(assets_path, "fonts/consolas.ttf"), &ft_2, 32);

		load_file(&dictionary);
		max_words = dictionary.num_words;
		current_index = get_random_number(max_words);
		show_full = 0;
	}

	float fps = 0;
	float start = clock();
	while (!glfwWindowShouldClose(window)) {
		float now = (clock() - start) / CLOCKS_PER_SEC;

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(all_shaders.text_shader, "projection", &ortho_projection);
		set_matrix4(all_shaders.rect_2d_shader, "projection", &ortho_projection);

		if(show_full)
			render_word_and_meaning(&dictionary, current_index, all_shaders.text_shader);
		else
			render_word(&dictionary, current_index, all_shaders.text_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("Ended.\n");
	return 0;
}

void load_file(Dictionary *dictionary) {
	FILE *file = NULL;
	file = fopen("../data/words/words.txt", "r");

	if(file == NULL) {
		printf("File words.txt not found.\n");
	}
	else {
		char *line = NULL;
		size_t len = 0;
		char word[25], meaning[50];

		getline(&line, &len, file);
		dictionary->num_words = atoi(line);

		unsigned int index = 0;
		while (getline(&line, &len, file) != -1) {
			get_word_and_meaning(line, dictionary->words[index], dictionary->meanings[index]);
			int len = strlen(dictionary->meanings[index]);
			dictionary->meanings[index][len - 1] = '\0';
			index += 1;
		}

		free(line);
	}
}

int get_random_number(int max) {
	srand(time(0));
	return rand() % max;
}

void render_word(Dictionary *dictionary, unsigned int index, int shader) {
	render_middle(dictionary->words[index], &font, shader, window_width / 2, window_height / 2 + 150, 1, 0, 0);
}

void render_word_and_meaning(Dictionary *dictionary, unsigned int index, int shader) {
	render_middle(dictionary->words[index], &font, shader, window_width / 2, window_height / 2 + 150, 1, 0, 0);
	render_middle(dictionary->meanings[index], &m_font, shader, window_width / 2, window_height / 2, 0, 1, 0);
}

void print_dictionary(Dictionary *dictionary) {
	for(unsigned int i = 0; i < dictionary->num_words; ++i) {
		printf("%s -> %s", dictionary->words[i], dictionary->meanings[i]);
	}
}

void get_word_and_meaning(char* from, char *word, char *meaning) {
	char* w = strtok(from, "-");
	char* m = strtok(NULL, "-");

	strcpy(word, w);
	strcpy(meaning, m);
}

void render_middle(const char* text, Font *font, unsigned int shader, int x, int y, float r, float g, float b) {
	float start = window_width / 2;
	start -= get_width(text, font, 1) / 2.0f;
	render_text(font, shader, text, start, y, 1, r, g, b);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	else if(key == GLFW_KEY_S && action == GLFW_PRESS)
		show_full = !show_full;
	else if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		current_index = get_random_number(max_words);
}

void append_char(char* str, char c) {
	unsigned int index = 0;
	while(1) {
		if(str[index] == '\0') {
			str[index] = c;
			str[index + 1] = '\0';
			break;
		}

		index += 1;
	}
}

void remove_last_char(char* text) {
	unsigned int index = 0;
	while(1) {
		if(text[index] == '\0' && index > 0) {
			text[index - 1] = '\0';
			break;
		}

		index += 1;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
