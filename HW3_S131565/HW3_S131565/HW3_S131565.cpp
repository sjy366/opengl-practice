#define _CRT_SECURE_NO_WARNINGS

#define TO_RADIAN 0.01745329252f 
#define TO_DEGREE 57.295779513f

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// #include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> //inverse, affineInverse, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; // computed only once in initialize_camera()
glm::mat4 ModelMatrix_TIGER, ModelMatrix_TIGER_EYE;
glm::mat4 ModelMatrix_TIGER_to_EYE; // computed only once in initialize_camera()

#include "Camera.h"
#include "Geometry.h"
#include <math.h>

/*********************************  START: callbacks *********************************/
int camera_toggle[4] = { 1, 0, 0, 0 };
int flag_draw_world_objects = 1;
int angle = 0;
int spider_angle = 0, wolf_angle = 0;
int spider_move = 1;
int ironman_angle = 0, ironman_speed = 1;
bool bike_action = 0, spider_action = 0;
float bike_position = 0, bike_up_speed = 0;
float spider_position = 0, spider_up_speed = 0;
float gravity = 0.98;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ModelMatrix_CAR_BODY = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(20.0f, 4.89f, 0.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelMatrix_TIGER = glm::rotate(glm::mat4(1.0f), -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_TIGER = glm::translate(ModelMatrix_TIGER, glm::vec3(30.0f, (sin(rotation_angle_tiger * 10) + 1) * 3.0f, 0.0f));
	ModelMatrix_TIGER = glm::rotate(ModelMatrix_TIGER, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_TIGER = glm::scale(ModelMatrix_TIGER, glm::vec3(0.07f, 0.07f, 0.07f));

	for (int i = 0; i < 4; i++)
		if(camera_toggle[i])
		{
			set_ViewProjection(i);

			ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(-50.0f, 0.0f, -50.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix,
				90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			draw_object(OBJECT_SQUARE16, 20 / 255.0f, 90 / 255.0f, 50 / 255.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //  draw the floor.
			
			ModelMatrix_CAR_BODY = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(20.0f, 4.89f, 0.0f));
			ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_BODY;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car_dummy(); // draw car

			ModelViewProjectionMatrix = glm::scale(ViewProjectionMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glLineWidth(2.0f);
			draw_axes();
			glLineWidth(1.0f); // draw axes

			ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(30.0f, (sin(rotation_angle_tiger * 10) + 1) * 3.0f, 0.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.07f, 0.07f, 0.07f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_tiger(); // draw tiger

			ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -ironman_angle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(15.0f, 20.0f, 0.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 45.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 45.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_ironman();

			ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -spider_angle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(30.0f, spider_position, 0.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(7.0f, -7.0f, 7.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_spider();

			ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -wolf_angle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(45.0f, 0.0f, 0.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_wolf();

			ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(0.0f, bike_position, 35.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_bike();

			ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, 215.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(-10.0f, 30.0f, -10.0f));
			ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 30.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_dragon();
		}
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': // move or stop spider
		if (spider_move) spider_move = 0;
		else spider_move = 1;
		break;
	case 'w':
		if (!spider_action)
		{
			spider_action = 1;
			spider_up_speed = 4;
		}
		break;
	case 'e':
		ironman_speed += 1;
		break;
	case 'r':
		ironman_speed -= 1;
		break;
	case 't': // move bike
		if (!bike_action)
		{
			bike_action = 1;
			bike_up_speed = 7;
		}
		break;
	case 'a':
		renew_sub_naxis(-5);
		break;
	case 's':
		renew_sub_naxis(5);
		break;
	case 'd':
		renew_sub_uaxis(-5);
		break;
	case 'f':
		renew_sub_uaxis(5);
		break;
	case 'g':
		renew_sub_vaxis(-5);
		break;
	case 'h':
		renew_sub_vaxis(5);
		break;
	case 'j':
		renew_sub_rotation_around_v_axis(-5);
		break;
	case 'k':
		renew_sub_rotation_around_v_axis(5);
		break;
	case 'l':
		renew_sub_naxis(-5);
		break;
	case ';':
		renew_sub_naxis(5);
		break;
	case '1': // sub
		if (camera_toggle[1]) camera_toggle[1] = 0;
		else camera_toggle[1] = 1;
		break;
	case '2': // car
		if (camera_toggle[2]) camera_toggle[2] = 0;
		else camera_toggle[2] = 1;
		break;
	case '3': // tiger
		if (camera_toggle[3]) camera_toggle[3] = 0;
		else camera_toggle[3] = 1;
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void special(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		renew_spherical_horizontal(-5);
		break;
	case GLUT_KEY_RIGHT:
		renew_spherical_horizontal(5);
		break;
	case GLUT_KEY_DOWN:
		renew_spherical_vertical(-5);
		break;
	case GLUT_KEY_UP:
		renew_spherical_vertical(5);
		break;
	}
}

void timer_scene(int timestamp_scene) {
	if (bike_action)
	{
		bike_up_speed -= gravity;
		bike_position += bike_up_speed;
		if (bike_position < 0.01)
		{
			bike_position -= bike_up_speed;
			bike_action = 0;
		}
	}
	if (spider_action)
	{
		spider_up_speed -= gravity;
		spider_position += spider_up_speed;
		if (spider_position < 0.01)
		{
			spider_position -= spider_up_speed;
			spider_action = 0;
		}
	}
	angle = (timestamp_scene % 360)*TO_RADIAN;
	if (spider_move) spider_angle = (spider_angle + 2) % 360;
	ironman_angle = (ironman_angle + ironman_speed) % 360;
	wolf_angle = (wolf_angle + 3) % 360;
	rotation_angle_car = (timestamp_scene % 360)*TO_RADIAN;
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	rotation_angle_tiger = (timestamp_scene % 720)*TO_RADIAN;
	cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf = timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;

	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

int prevx, prevy;

void motion(int x, int y) {
	int key = glutGetModifiers();
	if (!camera_wv.move | (camera_type != CAMERA_WORLD_VIEWER) | (key != GLUT_ACTIVE_SHIFT))
		return;

	renew_cam_position(prevx - x);
	//renew_cam_orientation_rotation_around_v_axis(prevx - x);

	prevx = x; prevy = y;

	set_ViewMatrix_for_world_viewer();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			camera_wv.move = 1;
			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP) camera_wv.move = 0;
	}
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);

	camera_wv.aspect_ratio = (float)width / height;

	ProjectionMatrix = glm::perspective(TO_RADIAN*camera_wv.fovy, camera_wv.aspect_ratio, camera_wv.near_c, camera_wv.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void cleanup(void) {
	free_axes();
	free_path();

	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
	free_geom_obj(GEOM_OBJ_ID_CAR_WHEEL);
	free_geom_obj(GEOM_OBJ_ID_CAR_NUT);
	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
	free_geom_obj(GEOM_OBJ_ID_COW);
	free_geom_obj(GEOM_OBJ_ID_TEAPOT);
	free_geom_obj(GEOM_OBJ_ID_BOX);

	glDeleteVertexArrays(N_OBJECTS, object_VAO);
	glDeleteBuffers(N_OBJECTS, object_VBO);
}
/*********************************  END: callbacks *********************************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	ModelMatrix_CAR_BODY = glm::mat4(1.0f);
	ModelMatrix_CAR_WHEEL = glm::mat4(1.0f);
	ModelMatrix_CAR_NUT = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_path();
	prepare_square();
	prepare_tiger();
	prepare_cow();
	prepare_ironman();
	prepare_bike();
	prepare_spider();
	prepare_wolf();
	prepare_dragon();

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, "Data/car_body_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, "Data/car_wheel_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, "Data/car_nut_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_COW, "Data/cow_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_TEAPOT, "Data/teapot_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_BOX, "Data/box_triangles_v.txt", GEOM_OBJ_TYPE_V);
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
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

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "HW3_S131565";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: q w e r t a s d f g h j k l ; up down left right shift ESC",
											"    - Mouse used: L-Click and move" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutMainLoop();
}
