#include "objects.h"
#include <deque>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define AIRPLANE_ROTATION_RADIUS 200.0f
#define BASE_SCALE 18

using namespace std;

int distance(int x, int y, int xx, int yy)
{
	return (x - xx)*(x - xx) + (y - yy)*(y - yy);
}

struct slug
{
	int x, y;
	int type;
	int speed;
	bool flag;
	bool alive;

	slug(int xx, int yy, int t, int s, bool f)
	{
		x = xx; y = yy;
		type = t;
		speed = s;
		flag = f;
		alive = 1;
	}
	void move()
	{
		int dx, dy;
		switch (type)
		{
		case 0:
			dx = 0; dy = 1;
			break;
		case 1:
			dx = 0; dy = -1;
			break;
		case 2:
		case 3:
		case 4:
			break;
		}
		dx *= speed; dy *= speed;
		x += dx; y += dy;
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
	int x, y;
	int scale;
	int level;
	bool load;
	bool alive;

	airplane(int xx, int yy)
	{
		x = xx; y = yy;
		scale = 1;
		level = 0;
		load = 1;
		alive = 1;
	}
	void move(int dx, int dy)
	{
		x += dx;
		y += dy;
	}
	void level_up()
	{
		if (level < 2) level++;
	}
	void shoot()
	{
		if (load)
		{
			slug_vec.push_back(slug(x, y, 0, 7, 0));
			load = 0;
		}
	}
	void reload()
	{
		load = 1;
	}
	void die()
	{
		alive = 0;
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
	int x, y;
	int dx, dy;
	int rotate;
	int type;
	int scale;
	bool alive;

	enemy(int xx, int yy)
	{
		type = rand() % 3;

		x = xx; y = yy;
		dx = 0; dy = -1;
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
		slug_vec.push_back(slug(x, y, 1, 4, 1));
	}
	void die()
	{
		alive = 0;
	}
	bool boundary_check()
	{
		if (-win_width / 2 < x && x < win_width / 2 && -win_height / 2 < y && y < win_height / 2) return 1;
		else return 0;
	}
};

airplane my_airplane(0, -300);
deque<enemy> enemy_vec;
int clock_count = 0;

void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	/*
	if (airplane_clock <= 360) { // 0 <= airplane_clock <= 719 
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
	}
	else {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(airplane_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(AIRPLANE_ROTATION_RADIUS, 0.0f, 0.0f));

		if (airplane_clock <= 540)
			airplane_s_factor = (airplane_clock - 360.0f) / 180.0f + 1.0f;
		else
			airplane_s_factor = -(airplane_clock - 540.0f) / 180.0f + 2.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));
	}

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane(); */

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
	if (clock_count % 200 == 0)
	{
		for (int i = 0; i < enemy_vec.size(); i++)
			if (enemy_vec[i].alive) enemy_vec[i].shoot();
	}
	if (clock_count % 1000 == 0)
	{
		enemy_vec.push_back(enemy(win_width / 4, win_height / 2));
		enemy_vec.push_back(enemy(-win_width / 4, win_height / 2));
	}

	for (int i = 0; i < slug_vec.size(); i++)
	{
		slug_vec[i].move();
		slug_vec[i].collision_check();
	}
	while (!slug_vec.empty() && !slug_vec.front().boundary_check()) slug_vec.pop_front();
	for (int i = 0; i < enemy_vec.size(); i++) enemy_vec[i].move();
	while (!enemy_vec.empty() && !enemy_vec.front().boundary_check()) enemy_vec.pop_front();

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

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple OpenGL 2D Game";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' & four arrow keys",
		"    - Mouse used: L-click and move"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	win_width = 600; win_height = 800;
	srand((unsigned int)time(0));

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}

void slug::collision_check()
{
	if (flag) // my
	{
		if (distance(x, y, my_airplane.x, my_airplane.y) < (BASE_SCALE*my_airplane.scale)*(BASE_SCALE*my_airplane.scale))
		{
			my_airplane.die();
			alive = 0;
		}
	}
	else // enemy
	{
		for (int i = 0; i < enemy_vec.size(); i++)
		{
			enemy& e = enemy_vec[i];
			if (distance(x, y, e.x, e.y) < (BASE_SCALE - 8)*e.scale*(BASE_SCALE - 8)*e.scale)
			{
				e.die();
				alive = 0;
				break;
			}
		}
	}
}
