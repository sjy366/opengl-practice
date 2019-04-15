#include "objects.h"
#include <deque>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define BASE_SCALE 10

using namespace std;

float distance(float x, float y, float xx, float yy)
{
	return (x - xx)*(x - xx) + (y - yy)*(y - yy);
}

struct slug
{
	float x, y;
	int type;
	float speed;
	int dir;
	int dir_count;
	float count;
	bool flag;
	bool alive;

	slug(float xx, float yy, int t, float s, bool f)
	{
		x = xx; y = yy;
		type = t;
		speed = s;
		count = 0;
		flag = f;
		alive = 1;
		dir = 1;
		dir_count = 0;
	}
	void move()
	{
		float dx, dy;
		switch (type)
		{
		case -1:
			dx = 0; dy = 1;
			break;
		case 0:
			dx = 0.25 * dir; dy = 0.5;
			break;
		case 1:
			dx = 0.25; dy = 0.5;
			break;
		case 2:
			dx = -0.25; dy = 0.5;
			break;
		case 3:
			dx = (float)sin(count) / 3; dy = 0.5;
			break;
		case 4:
			dx = -(float)sin(count) / 3; dy = 0.5;
			break;
		case 5:
			dx = -0.3; dy = 0.5;
			break;
		case 6:
			dx = 0.3; dy = 0.5;
			break;
		case 7:
			dx = -0.7; dy = 0.5;
			break;
		case 8:
			dx = 0.7; dy = 0.5;
			break;
		}
		if (flag) dy *= -1;
		dx *= speed; dy *= speed;
		x += dx; y += dy;
		count += 0.01;
		dir_count++;
		if(dir_count % 70 == 0) dir *= -1;
	}
	void collision_check();
	bool boundary_check()
	{
		if (-win_width/2 < x && x < win_width/2 && -win_height/2 < y && y < win_height/2) return 1;
		else return 0;
	}
};

deque<slug> slug_vec;

struct airplane
{
	float x, y;
	float scale;
	int level;
	bool load;
	bool alive;

	airplane(float xx, float yy)
	{
		x = xx; y = yy;
		scale = 0.5;
		level = 0;
		load = 1;
		alive = 1;
	}
	void move(float dx, float dy)
	{
		x += dx;
		y += dy;
	}
	void level_up()
	{
		if (level < 2)
		{
			level++;
			printf("level up!! [%d]\n", level);
		}
	}
	void level_down()
	{
		if (level == 0) alive = 0;
		else
		{
			level--;
			printf("level down!! [%d]\n", level);
		}
	}
	void shoot()
	{
		if (load)
		{
			switch (level)
			{
			case 0:
				slug_vec.push_back(slug(x, y, -1, 6, 0));
				break;
			case 1:
				slug_vec.push_back(slug(x, y, -1, 8, 0));
				slug_vec.push_back(slug(x, y, 5, 8, 0));
				slug_vec.push_back(slug(x, y, 6, 8, 0));
				break;
			case 2:
				slug_vec.push_back(slug(x, y, -1, 10, 0));
				slug_vec.push_back(slug(x, y, 0, 10, 0));
				slug_vec.push_back(slug(x, y, 1, 10, 0));
				slug_vec.push_back(slug(x, y, 2, 10, 0));
				slug_vec.push_back(slug(x, y, 3, 10, 0));
				slug_vec.push_back(slug(x, y, 4, 10, 0));
				slug_vec.push_back(slug(x, y, 7, 10, 0));
				slug_vec.push_back(slug(x, y, 8, 10, 0));
				break;
			}
			load = 0;
		}
	}
	void reload()
	{
		load = 1;
	}
	bool boundary_check()
	{
		if (-win_width/2 < x && x < win_width/2 && -win_height/2 < y && y < win_height/2) return 1;
		else return 0;
	}
};

#define COCKTAIL 0
#define SHIRT 1
#define HOUSE 2
#define CAR 3

struct enemy
{
	float x, y;
	float dx, dy;
	float rotate;
	int type;
	float scale;
	bool alive;

	enemy(float xx, float yy)
	{
		type = rand() % 4;

		x = xx; y = yy;
		dx = 0; dy = -0.5;
		rotate = 0;
		scale = 2;
		alive = 1;
	}
	void move()
	{
		x += dx; y += dy;
	}
	void shoot()
	{
		switch(type)
		{
		case COCKTAIL:
			slug_vec.push_back(slug(x, y, 0, 4, 1));
			break;
		case SHIRT:
			slug_vec.push_back(slug(x, y, 0, 4, 1));
			slug_vec.push_back(slug(x, y, 5, 4, 1));
			slug_vec.push_back(slug(x, y, 6, 4, 1));
			break;
		case HOUSE:
			slug_vec.push_back(slug(x, y, 0, 4, 1));
			slug_vec.push_back(slug(x, y, 1, 4, 1));
			slug_vec.push_back(slug(x, y, 2, 4, 1));
			break;
		case CAR:
			slug_vec.push_back(slug(x, y, 0, 4, 1));
			slug_vec.push_back(slug(x, y, 1, 4, 1));
			slug_vec.push_back(slug(x, y, 2, 4, 1));
			slug_vec.push_back(slug(x, y, 3, 4, 1));
			slug_vec.push_back(slug(x, y, 4, 4, 1));
			break;
		}
	}
	void die();
	bool boundary_check()
	{
		if (-win_width / 2 < x && x < win_width / 2 && -win_height / 2 < y && y < win_height / 2) return 1;
		else return 0;
	}
};

struct items
{
	float x, y;
	float dx, dy;
	float rotate;
	bool alive;
	items(int xx, int yy)
	{
		x = xx; y = yy;
		dx = 0; dy = -0.5;
		rotate = 0;
		alive = 1;
	}
	void move()
	{
		x += dx; y += dy;
		rotate += 1;
		if (rotate == 360) rotate = 0;
	}
	void collision_check();
	bool boundary_check()
	{
		if (-win_width / 2 < x && x < win_width / 2 && -win_height / 2 < y && y < win_height / 2) return 1;
		else return 0;
	}
};

airplane my_airplane(0, -300);
deque<enemy> enemy_vec;
deque<items> items_vec;
int clock_count = 0;

void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < slug_vec.size(); i++)
	{
		slug& s = slug_vec[i];
		if (s.alive)
		{
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(s.x, s.y, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (!s.flag) draw_slug();
			else draw_slug2();
		}
	}

	for (int i = 0; i < items_vec.size(); i++)
	{
		items& it = items_vec[i];
		if (it.alive)
		{
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(it.x, it.y, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, it.rotate*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_sword();
		}
	}

	for (int i = 0; i < enemy_vec.size(); i++)
	{
		enemy& e = enemy_vec[i];
		if (e.alive)
		{
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(e.x, e.y, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(e.scale, e.scale, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

			switch (e.type)
			{
			case COCKTAIL:
				draw_cocktail();
				break;
			case SHIRT:
				draw_shirt();
				break;
			case HOUSE:
				draw_house();
				break;
			case CAR:
				draw_car();
				break;
			}
		}
	}

	if (my_airplane.alive)
	{
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(my_airplane.x, my_airplane.y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(my_airplane.scale, my_airplane.scale, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane(); // my_airplane
	}

	glFlush();
}

void timer(int value) {
	airplane_clock = (airplane_clock + 1) % 720;

	clock_count %= 5000;
	if (clock_count % 10 == 0) my_airplane.reload();
	if (clock_count % 100 == 0)
	{
		for (int i = 0; i < enemy_vec.size(); i++)
			if (enemy_vec[i].alive) enemy_vec[i].shoot();
	}
	if (clock_count % 1000 == 0)
	{
		enemy_vec.push_back(enemy(win_width / 4, win_height / 2));
		enemy_vec.push_back(enemy(-win_width / 4, win_height / 2));
	}

	// slug
	for (int i = 0; i < slug_vec.size(); i++)
	{
		slug_vec[i].move();
		if (!slug_vec[i].boundary_check()) slug_vec[i].alive = 0;
		slug_vec[i].collision_check();
	}
	while (!slug_vec.empty() && !slug_vec.front().boundary_check()) slug_vec.pop_front();

	// enemy
	for (int i = 0; i < enemy_vec.size(); i++) enemy_vec[i].move();
	while (!enemy_vec.empty() && !enemy_vec.front().boundary_check()) enemy_vec.pop_front();

	// items
	for (int i = 0; i < items_vec.size(); i++)
	{
		items_vec[i].move();
		if (!items_vec[i].boundary_check()) items_vec[i].alive = 0;
		items_vec[i].collision_check();
	}
	while (!items_vec.empty() && !items_vec.front().boundary_check()) items_vec.pop_front();

	glutPostRedisplay();
	clock_count++;
	if (my_airplane.alive) glutTimerFunc(10, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 32: // SPACE key
		my_airplane.shoot();
		//printf("*********************************************\n");
		//for (int i = 0; i < slug_vec.size(); i++)
		//	printf("%d: [%d %d]\n", i, slug_vec[i].x, slug_vec[i].y);
		break;
	}
}

void special(int key, int x, int y) {
#define SENSITIVITY 25.0
	switch (key) {
	case GLUT_KEY_LEFT:
		my_airplane.move(-SENSITIVITY, 0);
		if (!my_airplane.boundary_check()) my_airplane.move(SENSITIVITY, 0);
		break;
	case GLUT_KEY_RIGHT:
		my_airplane.move(SENSITIVITY, 0);
		if (!my_airplane.boundary_check()) my_airplane.move(-SENSITIVITY, 0);
		break;
	case GLUT_KEY_DOWN:
		my_airplane.move(0, -SENSITIVITY);
		if (!my_airplane.boundary_check()) my_airplane.move(0, SENSITIVITY);
		break;
	case GLUT_KEY_UP:
		my_airplane.move(0, SENSITIVITY);
		if (!my_airplane.boundary_check()) my_airplane.move(0, -SENSITIVITY);
		break;
	}
	//printf("x: %d, y: %d\n", my_airplane.x, my_airplane.y);
	//glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
}

void motion(int x, int y) {
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	//printf("width: %d, height: %d\n", win_width, win_height);

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_car2();
	prepare_hat();
	prepare_cake();
	prepare_sword();
	prepare_slug();
	prepare_slug2();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");
	printf("\nLet's start! Your level is 0.\n");
}

#define N_MESSAGE_LINES 8
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple OpenGL 2D Game";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used:",
		"      ESC: Exit Program",
		"      UP, DOWN, RIGHT, LEFT: Move Airplane",
		"      SPACE: Shoot Slug"
		"    - Hint:",
		"      If you eat a knife, level up.",
		"      When the slug is hit, the level decreases.",
		"      If you hit a slug at level 0, you die."
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	initialize_glew();
	initialize_renderer();
	greetings(program_name, messages, N_MESSAGE_LINES);

	win_width = 600; win_height = 800;
	srand((unsigned int)time(0));

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}

void slug::collision_check()
{
	if (!alive) return;
	if (flag) // my
	{
		if (my_airplane.alive && distance(x, y, my_airplane.x, my_airplane.y) < (BASE_SCALE*my_airplane.scale)*(BASE_SCALE*my_airplane.scale))
		{

			my_airplane.level_down();
			alive = 0;
		}
	}
	else // enemy
	{
		for (int i = 0; i < enemy_vec.size(); i++)
		{
			enemy& e = enemy_vec[i];
			if (e.alive && distance(x, y, e.x, e.y) < ((BASE_SCALE+5)*e.scale*(BASE_SCALE+5)*e.scale))
			{
				e.die();
				alive = 0;
				break;
			}
		}
	}
}

void items::collision_check()
{
	if (alive && distance(x, y, my_airplane.x, my_airplane.y) < (BASE_SCALE + 5)*(BASE_SCALE + 5))
	{
		my_airplane.level_up();
		alive = 0;
	}
}

void enemy::die()
{
	alive = 0;
	items_vec.push_back(items(x, y));
}