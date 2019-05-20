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
} Camera;

Camera camera_wv;
Camera camera_sv;
enum _CameraType { CAMERA_WORLD_VIEWER, CAMERA_DRIVER } camera_type;

void set_ViewMatrix_for_world_viewer(void) {
	ViewMatrix = glm::mat4(camera_wv.uaxis.x, camera_wv.vaxis.x, camera_wv.naxis.x, 0.0f,
		camera_wv.uaxis.y, camera_wv.vaxis.y, camera_wv.naxis.y, 0.0f,
		camera_wv.uaxis.z, camera_wv.vaxis.z, camera_wv.naxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix = glm::translate(ViewMatrix, -camera_wv.pos);
	ProjectionMatrix = glm::perspective(TO_RADIAN*camera_wv.fovy, camera_wv.aspect_ratio, camera_wv.near_c, camera_wv.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_sub_viewer(void) {
	ViewMatrix = glm::mat4(camera_sv.uaxis.x, camera_sv.vaxis.x, camera_sv.naxis.x, 0.0f,
		camera_sv.uaxis.y, camera_sv.vaxis.y, camera_sv.naxis.y, 0.0f,
		camera_sv.uaxis.z, camera_sv.vaxis.z, camera_sv.naxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix = glm::translate(ViewMatrix, -camera_sv.pos);
	ProjectionMatrix = glm::perspective(TO_RADIAN*camera_sv.fovy, camera_sv.aspect_ratio, camera_sv.near_c, camera_sv.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_car(void) {
	ModelMatrix_CAR_DRIVER = ModelMatrix_CAR_BODY * ModelMatrix_CAR_BODY_to_DRIVER;
	ViewMatrix = glm::affineInverse(ModelMatrix_CAR_DRIVER);
	ProjectionMatrix = glm::perspective(TO_RADIAN*60.0f, (9.0f * WIN_WIDTH) / (16.0f * WIN_HEIGHT), 1.5f, 500.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_tiger(void) {
	ModelMatrix_TIGER_EYE = ModelMatrix_TIGER * ModelMatrix_TIGER_to_EYE;
	ViewMatrix = glm::affineInverse(ModelMatrix_TIGER_EYE);
	ProjectionMatrix = glm::perspective(TO_RADIAN*60.0f, (9.0f * WIN_WIDTH) / (16.0f * WIN_HEIGHT), 0.75f, 500.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewProjection(int i)
{
	switch (i)
	{
	case MAIN:
		set_ViewMatrix_for_world_viewer();
		glViewport(0, 0, (int)(WIN_WIDTH * 2.0 / 3.0), WIN_HEIGHT);
		break;
	case SUB:
		set_ViewMatrix_for_sub_viewer();
		glViewport((int)(WIN_WIDTH * 2.0 / 3.0), (int)(WIN_HEIGHT * 2.0 / 3.0), (int)(WIN_WIDTH * 1.0 / 3.0), (int)(WIN_HEIGHT * 1.0 / 3.0));
		break;
	case CAR:
		set_ViewMatrix_for_car();
		glViewport((int)(WIN_WIDTH * 2.0 / 3.0), (int)(WIN_HEIGHT * 1.0 / 3.0), (int)(WIN_WIDTH * 1.0 / 3.0), (int)(WIN_HEIGHT * 1.0 / 3.0));
		break;
	case TIGER:
		set_ViewMatrix_for_tiger();
		glViewport((int)(WIN_WIDTH * 2.0 / 3.0), 0, (int)(WIN_WIDTH * 1.0 / 3.0), (int)(WIN_HEIGHT * 1.0 / 3.0));
		break;
	}
}

void initialize_camera(void) {
	camera_type = CAMERA_WORLD_VIEWER;

	ViewMatrix = glm::lookAt(glm::vec3(0.0f, 75.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	camera_sv.uaxis = glm::vec3(ViewMatrix[0].x, ViewMatrix[1].x, ViewMatrix[2].x);
	camera_sv.vaxis = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y);
	camera_sv.naxis = glm::vec3(ViewMatrix[0].z, ViewMatrix[1].z, ViewMatrix[2].z);
	camera_sv.pos = -(ViewMatrix[3].x*camera_sv.uaxis + ViewMatrix[3].y*camera_sv.vaxis + ViewMatrix[3].z*camera_sv.naxis);

	camera_sv.move = 0;
	camera_sv.fovy = 30.0f, camera_sv.aspect_ratio = 1.0f; camera_sv.near_c = 5.0f; camera_sv.far_c = 10000.0f;

	ViewMatrix = glm::lookAt(glm::vec3(75.0f, 75.0f, 75.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	camera_wv.uaxis = glm::vec3(ViewMatrix[0].x, ViewMatrix[1].x, ViewMatrix[2].x);
	camera_wv.vaxis = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y);
	camera_wv.naxis = glm::vec3(ViewMatrix[0].z, ViewMatrix[1].z, ViewMatrix[2].z);
	camera_wv.pos = -(ViewMatrix[3].x*camera_wv.uaxis + ViewMatrix[3].y*camera_wv.vaxis + ViewMatrix[3].z*camera_wv.naxis);

	camera_wv.move = 0;
	camera_wv.fovy = 30.0f, camera_wv.aspect_ratio = 1.0f; camera_wv.near_c = 5.0f; camera_wv.far_c = 10000.0f;

	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	// the transformation that moves the driver's camera frame from car body's MC to driver seat
	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	ModelMatrix_TIGER_to_EYE = glm::rotate(glm::mat4(1.0f), -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_TIGER_to_EYE = glm::translate(ModelMatrix_TIGER_to_EYE, glm::vec3(0.0f, -88.0f, 62.0f));
	ModelMatrix_TIGER_to_EYE = glm::rotate(ModelMatrix_TIGER_to_EYE, 180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, -1.0f));
}

#define CAM_TSPEED 0.05f
void renew_cam_position(int del) {
	camera_wv.pos += CAM_TSPEED * del*(-camera_wv.naxis);
}

void renew_sub_naxis(int del)
{
	camera_sv.pos += CAM_TSPEED * del*(-camera_sv.naxis);
}
void renew_sub_uaxis(int del)
{
	camera_sv.pos += CAM_TSPEED * del*(camera_sv.uaxis);
}
void renew_sub_vaxis(int del)
{
	camera_sv.pos += CAM_TSPEED * del*(camera_sv.vaxis);
}


#define CAM_RSPEED 0.1f
void renew_cam_orientation_rotation_around_v_axis(int angle) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, camera_wv.vaxis));
	camera_wv.uaxis = RotationMatrix * camera_wv.uaxis;
	camera_wv.naxis = RotationMatrix * camera_wv.naxis;
}
void renew_sub_rotation_around_v_axis(int angle) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, camera_sv.vaxis));
	camera_sv.uaxis = RotationMatrix * camera_sv.uaxis;
	camera_sv.naxis = RotationMatrix * camera_sv.naxis;
}

void renew_spherical_vertical(int angle)
{
	glm::mat3 RotationMatrix;
	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, glm::vec3(-camera_wv.naxis[2], 0.0f, camera_wv.naxis[0])));
	camera_wv.pos = RotationMatrix * camera_wv.pos;
	camera_wv.naxis = RotationMatrix * camera_wv.naxis;
	camera_wv.uaxis = RotationMatrix * camera_wv.uaxis;
	camera_wv.vaxis = RotationMatrix * camera_wv.vaxis;
}
void renew_spherical_horizontal(int angle)
{
	glm::mat3 RotationMatrix;
	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, glm::vec3(0.0f, 1.0f, 0.0f)));
	camera_wv.pos = RotationMatrix * camera_wv.pos;
	camera_wv.naxis = RotationMatrix * camera_wv.naxis;
	camera_wv.uaxis = RotationMatrix * camera_wv.uaxis;
	camera_wv.vaxis = RotationMatrix * camera_wv.vaxis;
}
/*********************************  END: camera *********************************/