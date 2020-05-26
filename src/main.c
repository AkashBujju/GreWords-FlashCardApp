#define _GNU_SOURCE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>

#include "shader.h"
#include "rectangle_2d.h"
#include "text.h"
#include "all.h"

#define MAX_WORDS 24

typedef struct WeekDictionary {
	unsigned int week;
	char words[MAX_WORDS][50];
	char meanings[MAX_WORDS][100];
} WeekDictionary;

typedef struct Dictionary {
	WeekDictionary dictionary[100];
	unsigned int num_weeks;
} Dictionary;

typedef struct DoneIndices {
	unsigned int indices[2000];
	unsigned int length;
} DoneIndices;

Vector3 position, up, front;
float cursor_x;
Font font, m_font, s_font;
unsigned int window_width, window_height;
GLFWwindow *window = NULL;
unsigned int current_week;
unsigned int current_word;
unsigned int max_weeks;
unsigned int max_words;
unsigned int times_pressed;
uint8_t show_full;
char conf_type[10], conf_week[10];
DoneIndices done_indices;

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
void load_full(Dictionary *dictionary);
void get_word_and_meaning(char* from, char *word, char *meaning);
void print_dictionary(Dictionary *dictionary);
void render_word_and_meaning(Dictionary *dictionary, unsigned int week_index, unsigned int word_index, int shader);
void render_word(Dictionary *dictionary, unsigned int week_index, unsigned int word_index, int shader);
int get_random_number(int max);
int convert_week_and_word_to_index(int week_index, int word_index);

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
		init_font(&font, combine_string(assets_path, "fonts/GeorgiaRegular.ttf"), &ft_1, 40);
		FT_Library ft_2;
		init_freetype(&ft_2);
		init_font(&m_font, combine_string(assets_path, "fonts/GeorgiaRegular.ttf"), &ft_2, 26);
		FT_Library ft_3;
		init_freetype(&ft_3);
		init_font(&s_font, combine_string(assets_path, "fonts/Consolas.ttf"), &ft_3, 16);

		load_file(&dictionary);
		max_weeks = dictionary.num_weeks;
		max_words = MAX_WORDS;
		done_indices.length = 0;
		{
			current_word = get_random_number(max_words);
			if(strcmp(conf_type, "full") == 0)
				current_week = get_random_number(max_weeks);
			else if(strcmp(conf_type, "weekly") == 0)
				current_week = atoi(conf_week) - 1;
			if(strcmp(conf_type, "full") == 0)
				done_indices.indices[done_indices.length++] = convert_week_and_word_to_index(current_week, current_word);
			else if(strcmp(conf_type, "weekly") == 0)
				done_indices.indices[done_indices.length++] = convert_week_and_word_to_index(atoi(conf_week) - 1, current_word);
		}
		show_full = 0;
		times_pressed = 0;
		srand(time(0));
	}

	float fps = 0;
	float start = clock();
	while (!glfwWindowShouldClose(window)) {
		float now = (clock() - start) / CLOCKS_PER_SEC;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(all_shaders.text_shader, "projection", &ortho_projection);
		set_matrix4(all_shaders.rect_2d_shader, "projection", &ortho_projection);

		if(show_full)
			render_word_and_meaning(&dictionary, current_week, current_word, all_shaders.text_shader);
		else
			render_word(&dictionary, current_week, current_word, all_shaders.text_shader);

		if(strcmp(conf_type, "full") == 0) {
			render_text(&s_font, all_shaders.text_shader, "Type: full", 0.05f * window_width, 0.05f * window_height, 1, 0.5f, 0.5f, 0.5f);
		}
		else if(strcmp(conf_type, "weekly") == 0) {
			char str[30];
			strcpy(str, "Type: weekly, Number: ");
			strcat(str, conf_week);
			render_text(&s_font, all_shaders.text_shader, str, 0.05f * window_width, 0.05f * window_height, 1, 0.5f, 0.5f, 0.5f);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("Ended.\n");
	return 0;
}

int convert_week_and_word_to_index(int week_index, int word_index) {
	return week_index * 24 + word_index;
}

void load_file(Dictionary *dictionary) {
	/* Config file */
	{
		FILE *file = NULL;
		file = fopen("../config.conf", "r");
		if(file == NULL) {
			printf("\nconfig.conf file not found!\n");
			assert(file != NULL);
		}

		char tmp[50];
		fscanf(file, "%s", tmp);
		fscanf(file, "%s", conf_type);
		fscanf(file, "%s", tmp);
		fscanf(file, "%s", conf_week);

		fclose(file);
	}

	/* Words files */
	{
		load_full(dictionary);
	}
}

void load_full(Dictionary *dictionary) {
	FILE *file = NULL;
	file = fopen("../data/words/words.words", "r");

	if(file == NULL) {
		printf("File words.txt not found.\n");
	}
	else {
		char *line = NULL;
		size_t len = 0;
		char word[50], meaning[100];

		unsigned int week_index = 0;
		unsigned int word_index = 0;
		unsigned int num_weeks = 0;
		while (getline(&line, &len, file) != -1) {
			if(line[0] == '@') {
				int week_number = atoi(line + 1);

				week_index = week_number - 1;
				word_index = 0;
				num_weeks += 1;
				continue;
			}
			get_word_and_meaning(line, dictionary->dictionary[week_index].words[word_index], dictionary->dictionary[week_index].meanings[word_index]);
			int len = strlen(dictionary->dictionary[week_index].meanings[word_index]);
			dictionary->dictionary[week_index].meanings[word_index][len - 1] = '\0';
			word_index += 1;
		}

		dictionary->num_weeks = num_weeks;
		free(line);
		fclose(file);
	}
}

int get_random_number(int max) {
	if(times_pressed % 10 == 0)
		srand(time(0));

	int random_number = rand() % max;
	for(int i = 0; i < done_indices.length; ++i) {
		if(done_indices.indices[i] == random_number) {
			random_number = rand() % max;
			if(i == max - 1) {
				done_indices.length = 0;
				return 0;
			}
			i = 0;
		}
	}

	return random_number;
}

void render_word(Dictionary *dictionary, unsigned int week_index, unsigned int word_index, int shader) {
	render_middle(dictionary->dictionary[week_index].words[word_index], &font, shader, window_width / 2, window_height / 2 + 100, 1, 0, 0);
}

void render_word_and_meaning(Dictionary *dictionary, unsigned int week_index, unsigned int word_index, int shader) {
	render_middle(dictionary->dictionary[week_index].words[word_index], &font, shader, window_width / 2, window_height / 2 + 100, 1, 0, 0);
	render_middle(dictionary->dictionary[week_index].meanings[word_index], &m_font, shader, window_width / 2, window_height / 2, 0, 1, 0);
}

void print_dictionary(Dictionary *dictionary) {
	unsigned int num = 1;
	for(unsigned int i = 0; i < dictionary->num_weeks; ++i) {
		for(unsigned int j = 0; j < MAX_WORDS; ++j) {
			printf("%d: %s -> %s\n", num, dictionary->dictionary[i].words[j], dictionary->dictionary[i].meanings[j]);
			num += 1;
		}
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
		show_full = 1;
	else if(key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		current_word = get_random_number(max_words);

		if(strcmp(conf_type, "full") == 0)
			current_week = get_random_number(max_weeks);
		else if(strcmp(conf_type, "weekly") == 0)
			current_week = atoi(conf_week) - 1;

		if(strcmp(conf_type, "full") == 0)
			done_indices.indices[done_indices.length++] = convert_week_and_word_to_index(current_week, current_word);
		else if(strcmp(conf_type, "weekly") == 0)
			done_indices.indices[done_indices.length++] = convert_week_and_word_to_index(atoi(conf_week) - 1, current_word);

		times_pressed += 1;
		show_full = 0;
	}
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
