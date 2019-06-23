#include "objects.h"
#include <deque>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define BASE_SCALE 10
#define KING_IS_COMING 5
#define KING_HP 20
#define BOMB_TIME 1000

using namespace std;

float distance(float x, float y, float xx, float yy)
{
	return (x - xx)*(x - xx) + (y - yy)*(y - yy);
}

#define KING_TYPE_SLUG -2
#define BASE_TYPE_SLUG -1
struct slug
{
	float x, y;
	float kx, ky;
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
		kx = 0; ky = 0;
		type = t;
		speed = s;
		count = 0;
		flag = f;
		alive = 1;
		dir = 1;
		dir_count = 0;
	}
	slug(float xx, float yy, float s, float ddx, float ddy, void* aux)
	{
		x = xx; y = yy;
		kx = ddx; ky = ddy;
		type = KING_TYPE_SLUG;
		speed = s;
		count = 0;
		flag = 1;
		alive = 1;
		dir = 1;
		dir_count = 0;
	}
	void move()
	{
		float dx, dy;
		switch (type)
		{
		case KING_TYPE_SLUG:
			dx = kx; dy = ky;
			break;
		case BASE_TYPE_SLUG:
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
	void level_down();
	void shoot()
	{
		if (load)
		{
			switch (level)
			{
			case 0:
				slug_vec.push_back(slug(x, y, BASE_TYPE_SLUG, 6, 0));
				break;
			case 1:
				slug_vec.push_back(slug(x, y, BASE_TYPE_SLUG, 8, 0));
				slug_vec.push_back(slug(x, y, 5, 8, 0));
				slug_vec.push_back(slug(x, y, 6, 8, 0));
				break;
			case 2:
				slug_vec.push_back(slug(x, y, BASE_TYPE_SLUG, 10, 0));
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
	float count;
	int dir_count;
	int dir;
	int type;
	float scale;
	bool alive;

	enemy(float xx, float yy)
	{
		type = rand() % 4;

		x = xx; y = yy;
		dx = 0; dy = -0.5;
		rotate = 0;
		count = 0;
		dir_count = 0;
		dir = 1;
		scale = 1.5;
		alive = 1;
	}
	void move()
	{
		float dx, dy;
		switch (type)
		{
		case COCKTAIL:
			dx = 0.25 * dir; dy = -0.5;
			break;
		case SHIRT:
			dx = 0.25; dy = -0.5;
			break;
		case HOUSE:
			dx = (float)sin(count) / 3; dy = -0.5;
			break;
		case CAR:
			dx = -(float)sin(count) / 3; dy = -0.5;
			break;
		}
		x += dx; y += dy;
		count += 0.01;
		dir_count++;
		if (dir_count % 70 == 0) dir *= -1;
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

struct enemy_king
{
	float x, y;
	float dx, dy;
	float scale;
	int hp;
	bool alive;

	enemy_king()
	{
		dx = 0; dy = -0.3;
		scale = 5;
		hp = KING_HP;
		alive = 1;
	}
	void set_xy(float xx, float yy)
	{
		x = xx; y = yy;
	}
	void damage();
	void move()
	{
		x += dx; y += dy;
		if (y < win_height / 3) y -= dy;
	}
	void shoot()
	{
		float a, b;
		slug_vec.push_back(slug(x, y, BASE_TYPE_SLUG, 8, 1));
		slug_vec.push_back(slug(x, y, 3, 8, 1));
		slug_vec.push_back(slug(x, y, 4, 8, 1));
		for (float i = -0.6, a = 1; i < 0; i += 0.4 * a, a += 1)
		{
			for (float j = -1, b = 1; j < 0; j += 0.3 * b, b += 1)
				slug_vec.push_back(slug(x, y, 8, i, j, NULL));
			for (float j = 1, b = 1; j >= 0; j -= 0.3 * b, b += 1)
				slug_vec.push_back(slug(x, y, 8, i, j, NULL));
		}
		for (float i = 0.6, a = 1; i >= 0; i -= 0.4 * a, a += 1)
		{
			for (float j = -1, b = 1; j < 0; j += 0.3 * b, b += 1)
				slug_vec.push_back(slug(x, y, 8, i, j, NULL));
			for (float j = 1, b = 1; j >= 0; j -= 0.3 * b, b += 1)
				slug_vec.push_back(slug(x, y, 8, i, j, NULL));
		}
		for(int i=0; i<8; i++)
			slug_vec.push_back(slug(x, y, rand() % 9, 8, 1));
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

struct effect
{
	float x, y;
	float r[3];
	bool alive;
	effect(float xx, float yy)
	{
		x = xx; y = yy;
		r[0] = 0;
		r[1] = -5;
		r[2] = -10;
		alive = 1;
	}
	void move()
	{
		for (int i = 0; i < 3; i++) r[i] += 0.5;
		if (r[2] > 30) alive = 0;
	}
};

airplane my_airplane(0, -300);
enemy_king king;
deque<enemy> enemy_vec;
deque<items> items_vec;
deque<effect> effect_vec;
int clock_count = 0;
int kill = 0;
// for effect
int ax[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int bx[8] = {0, -1, 0, 1, 0, 1, 0, -1};
int ay[8] = {0, 1, 1, 1, 0, -1, -1, -1};
int by[8] = {0, -1, 0, -1, 0, 1, 0, 1};
bool king_stage = 0;

int bomb_on = 0;
int bomb[3] = { 0 };
int bomb_clock = 0;

void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	// bombs
	if (bomb[0])
	{
		for (int i = 1; i <= 3; i++)
		{
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(70 * i, 70 * i, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(i, i, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_hat();
		}
	}
	if (bomb[1])
	{
		int dir;
		if (bomb_clock % 2 == 0) dir = -1;
		else dir = 1;

		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(bomb_clock % 500 - 250, bomb_clock % 500 - 250, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-30, 30, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3((float)bomb_clock/200.0, (float)bomb_clock/200.0, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car2();
	}
	if (bomb[2])
	{
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(70, 70, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(60, 70, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(50, 70, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, bomb_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(40, 70, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2, 2, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cake();
	}

	for (int i = 0; i < slug_vec.size(); i++)
	{
		slug& s = slug_vec[i];
		if (s.alive)
		{
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(s.x, s.y, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5, 1.5, 1.0f));
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

	for (int i = 0; i < effect_vec.size(); i++)
	{
		effect& e = effect_vec[i];
		if (e.alive)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 8; k++)
				{
					ModelMatrix = glm::mat4(1.0f);
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(ax[k]*e.r[j]+bx[k] + e.x, ay[k]*e.r[j]+by[k] + e.y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, e.r[j]*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3 * (j+1), 0.3 * (j+1), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cake();
				}
			}
		}
	}

	if (king_stage && king.alive)
	{
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(king.x, king.y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(king.scale, king.scale, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_ufo();
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

bool game_over = 0;
void timer(int value) {
	clock_count %= 6000;
	if (clock_count % 10 == 0) my_airplane.reload();
	// enemy shoot
	if (clock_count % 100 == 0)
	{
		for (int i = 0; i < enemy_vec.size(); i++)
			if (enemy_vec[i].alive) enemy_vec[i].shoot();
	}
	// gen enemy
	if (kill < KING_IS_COMING)
	{
		if (clock_count % 600 == 0)
			enemy_vec.push_back(enemy(win_width / (rand() % 2 + 3), win_height / 2));
		if (clock_count % 600 == 300)
			enemy_vec.push_back(enemy(-win_width / (rand() % 2 + 3), win_height / 2));
	}
	else // gen enemy king
	{
		if (king_stage)
		{
			if(king.alive)
			{
				king.move();
				if (clock_count % 80 == 0) king.shoot();
				if (clock_count % 300 == 0)
				{
					enemy_vec.push_back(enemy(win_width / 2 - 30, win_height / 2));
					enemy_vec.push_back(enemy(-win_width / 2 + 30, win_height / 2));
				}
			}
		}
		else
		{
			printf("**************************************************************\n");
			printf("The enemy king is coming!!\n");
			printf("**************************************************************\n");
			king.set_xy(0, win_height / 2 + 30);
			king_stage = 1;
		}
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

	// effect
	for (int i = 0; i < effect_vec.size(); i++) effect_vec[i].move();
	while (!effect_vec.empty() && !effect_vec.front().alive) effect_vec.pop_front();

	glutPostRedisplay();
	clock_count++;
	for (int i = 0; i < 3; i++)
		if (bomb[i] > 0) bomb[i]--;
	if (bomb[0] == bomb[1] && bomb[1] == bomb[2] && bomb[2] == 0) bomb_on = 0;
	bomb_clock = (bomb_clock + 1) % 960;
	while (bomb_on && !slug_vec.empty()) slug_vec.pop_front();

	if (!my_airplane.alive && !game_over)
	{
		printf("**************************************************************\n");
		printf("GAME OVER!!\n");
		printf("**************************************************************\n\n");
		printf("\nPress the ESC key...\n");
		game_over = 1;
	}
	if (!king.alive && !game_over)
	{
		while (!enemy_vec.empty()) enemy_vec.pop_front();
		printf("**************************************************************\n");
		printf("GAME CLEAR!!\n");
		printf("**************************************************************\n\n");
		printf("\nPress the ESC key...\n");
		game_over = 1;
	}
	glutTimerFunc(10, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 32: // SPACE key
		
		if(my_airplane.alive) my_airplane.shoot();
		//printf("*********************************************\n");
		//for (int i = 0; i < slug_vec.size(); i++)
		//	printf("%d: [%d %d]\n", i, slug_vec[i].x, slug_vec[i].y);
		break;
	case '1':
		if(!bomb[0] && !bomb_on) bomb[0] = BOMB_TIME, bomb_on = 1;
		break;
	case '2':
		if(!bomb[1] && !bomb_on) bomb[1] = BOMB_TIME, bomb_on = 1;
		break;
	case '3':
		if(!bomb[2] && !bomb_on) bomb[2] = BOMB_TIME, bomb_on = 1;
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
	glutPostRedisplay();
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
	prepare_ufo();
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
	printf("Let's start!\n\nlevel up!! [0]\n");
}

#define N_MESSAGE_LINES 15
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple OpenGL 2D Game";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used:\n",
		"      * ESC: 프로그램 종료",
		"      * UP/DOWN/LEFT/RIGHT: 비행기 이동",
		"      * SPACE: 슬러그 발사",
		"      * '1': 1번 폭탄 발사",
		"      * '2': 2번 폭탄 발사",		
		"      * '3': 3번 폭탄 발사\n",
		"    - Hint:\n",
		"      * 적군을 죽이면, 일정확률로 강화무기가 나온다.",
		"      * 강화무기를 먹으면, 레벨이 올라간다.",
		"      * 레벨이 올라가면, 슬러그의 형태가 바뀌고 1몫이 추가된다.",
		"      * 최대 레벨은 2 이다.",
		"      * 일정수 이상의 적군을 죽이면, 최종보스가 등장한다.",
		"      * 최종보스는 여러대를 맞추어야 죽는다.",
		"      * 최종보스를 잡으면 게임이 종료된다."
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 800);
	glutInitWindowPosition(800, 0);
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
		if (king_stage && king.alive &&
			distance(x, y, king.x, king.y) < (BASE_SCALE*king.scale*BASE_SCALE*king.scale + 50))
		{
			king.damage();
			alive = 0;
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
	kill++;
	printf("Killed %d enemies!!\n", kill);
	if(rand() % 2 == 0) items_vec.push_back(items(x, y));
	effect_vec.push_back(effect(x, y));
}

void airplane::level_down()
{
	if (level == 0)
	{
		effect_vec.push_back(effect(x, y));
		alive = 0;
	}
	else
	{
		level--;
		printf("level down!! [%d]\n", level);
	}
}

void enemy_king::damage()
{
	if (hp > 0)
	{
		hp--;
		printf("HP of the king: %d!!\n", hp);
	}
	else
	{
		alive = 0;
		effect_vec.push_back(effect(x, y));
	}
}