/*********************************  START: camera *********************************/
#define WIN_WIDTH 1200
#define WIN_HEIGHT 800
#define MAIN 0
#define SUB 1
#define CAR 2
#define TIGER 3

typedef struct _Camera {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
	float fovy, aspect_ratio, near_c, far_c;
	int move;
	int x, y, xx, yy;
} Camera;

Camera camera_wv[4];

void set_ViewProcjetionMatrix(int i)
{
	ProjectionMatrix = glm::perspective(TO_RADIAN*camera_wv[i].fovy, camera_wv[i].aspect_ratio,
		camera_wv[i].near_c, camera_wv[i].far_c);
	switch (i)
	{
	case MAIN:
		ViewMatrix = glm::mat4(
			camera_wv[i].uaxis.x, camera_wv[i].vaxis.x, camera_wv[i].naxis.x, 0.0f,
			camera_wv[i].uaxis.y, camera_wv[i].vaxis.y, camera_wv[i].naxis.y, 0.0f,
			camera_wv[i].uaxis.z, camera_wv[i].vaxis.z, camera_wv[i].naxis.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case SUB:
		ViewMatrix = glm::mat4(
			camera_wv[i].uaxis.x, camera_wv[i].vaxis.x, camera_wv[i].naxis.x, 0.0f,
			camera_wv[i].uaxis.y, camera_wv[i].vaxis.y, camera_wv[i].naxis.y, 0.0f,
			camera_wv[i].uaxis.z, camera_wv[i].vaxis.z, camera_wv[i].naxis.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case CAR:
		ModelMatrix_CAR_DRIVER = ModelMatrix_CAR_BODY * ModelMatrix_CAR_BODY_to_DRIVER;
		ViewMatrix = glm::affineInverse(ModelMatrix_CAR_DRIVER);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case TIGER:
		ModelMatrix_TIGER_EYE = ModelMatrix_TIGER * ModelMatrix_TIGER_to_EYE;
		ViewMatrix = glm::affineInverse(ModelMatrix_TIGER_EYE);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	}
}

void initialize_camera(void) {

	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case MAIN:
			ViewMatrix = glm::lookAt(glm::vec3(75.0f, 75.0f, 75.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			camera_wv[i].uaxis = glm::vec3(ViewMatrix[0].x, ViewMatrix[1].x, ViewMatrix[2].x);
			camera_wv[i].vaxis = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y);
			camera_wv[i].naxis = glm::vec3(ViewMatrix[0].z, ViewMatrix[1].z, ViewMatrix[2].z);
			camera_wv[i].pos = -(ViewMatrix[3].x*camera_wv[i].uaxis + ViewMatrix[3].y*camera_wv[i].vaxis + ViewMatrix[3].z*camera_wv[i].naxis);
			camera_wv[i].move = 0;
			camera_wv[i].fovy = 30.0f, camera_wv[i].aspect_ratio = 1.0f; camera_wv[i].near_c = 5.0f; camera_wv[i].far_c = 10000.0f;

			camera_wv[i].xx = (int)((2.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].yy = WIN_HEIGHT;
			camera_wv[i].x = 0;
			camera_wv[i].y = 0;
			break;
		case SUB:
			ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			camera_wv[i].uaxis = glm::vec3(ViewMatrix[0].x, ViewMatrix[1].x, ViewMatrix[2].x);
			camera_wv[i].vaxis = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y);
			camera_wv[i].naxis = glm::vec3(ViewMatrix[0].z, ViewMatrix[1].z, ViewMatrix[2].z);
			camera_wv[i].pos = -(ViewMatrix[3].x*camera_wv[i].uaxis + ViewMatrix[3].y*camera_wv[i].vaxis + ViewMatrix[3].z*camera_wv[i].naxis);
			camera_wv[i].move = 0;
			camera_wv[i].fovy = 30.0f, camera_wv[i].aspect_ratio = 27.0f / 32.0f; camera_wv[i].near_c = 5.0f; camera_wv[i].far_c = 400.0f;

			camera_wv[i].xx = (int)((1.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].yy = camera_wv[i].xx * camera_wv[i].aspect_ratio * 0.8;
			camera_wv[i].x = (int)((2.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].y = WIN_HEIGHT - camera_wv[i].yy;
			break;
		case CAR:
			camera_wv[i].move = 0;
			camera_wv[i].fovy = 60.0f, camera_wv[i].aspect_ratio = 27.0f / 32.0f; camera_wv[i].near_c = 1.0f; camera_wv[i].far_c = 500.0f;

			camera_wv[i].xx = (int)((1.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].yy = camera_wv[i].xx * camera_wv[i].aspect_ratio * 0.8;
			camera_wv[i].x = (int)((2.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].y = camera_wv[i-1].y - camera_wv[i].yy;
			break;
		case TIGER:
			camera_wv[i].move = 0;
			camera_wv[i].fovy = 60.0f, camera_wv[i].aspect_ratio = 27.0f / 32.0f; camera_wv[i].near_c = 1.0f; camera_wv[i].far_c = 500.0f;

			camera_wv[i].xx = (int)((1.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].yy = camera_wv[i].xx * camera_wv[i].aspect_ratio * 0.8;
			camera_wv[i].x = (int)((2.0f * WIN_WIDTH) / 3.0f);
			camera_wv[i].y = camera_wv[i - 1].y - camera_wv[i].yy;
			break;
		}
	}
	
	ViewMatrix = glm::lookAt(glm::vec3(75.0f, 75.0f, 75.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	// the transformation that moves the driver's camera frame from car body's MC to driver seat
	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelMatrix_TIGER_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -88.0f, 62.0f));
	ModelMatrix_TIGER_to_EYE = glm::rotate(ModelMatrix_TIGER_to_EYE, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

#define CAM_TSPEED 0.05f
void renew_cam_position(int type, int del) {
	camera_wv[type].pos += CAM_TSPEED*del*(-camera_wv[type].naxis);
}

#define CAM_RSPEED 0.1f
void renew_cam_orientation_rotation_around_v_axis(int type, int angle) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, camera_wv[type].vaxis));
	camera_wv[type].uaxis = RotationMatrix* camera_wv[type].uaxis;
	camera_wv[type].naxis = RotationMatrix* camera_wv[type].naxis;
}
/*********************************  END: camera *********************************/