#ifndef _N_INPUT_PREREQ_H_
#define _N_INPUT_PREREQ_H_

enum EButtonType {
	BTN_UP = 0,
	BTN_DOWN,
	BTN_LEFT,
	BTN_RIGHT,
	BTN_START,
	BTN_BACK,
	BTN_RIGHT_MOUSE,
	BTN_LEFT_MOUSE,
	BTN_ARROW_UP,
	BTN_ARROW_LEFT,
	BTN_ARROW_DOWN,
	BTN_ARROW_RIGHT,
	BTN_A,
	BTN_B,
	BTN_SIZE
};

struct MousePosition{
	MousePosition() : x(0), y(0) {}
	MousePosition(int p_x, int p_y) : x(p_y), y(p_y){}
	int x;
	int y;
};

struct MovementAxis{
	MovementAxis(void) : x(0.0f), z(0.0f) {}
	MovementAxis(float p_x, float p_z) : x(p_x), z(p_z){}
	float x;
	float z;
};

struct CameraAxis{
	CameraAxis(void) : x(0.0f), y(0.0f), z(0.0f){}
	CameraAxis(float p_x, float p_y, float p_z) : x(p_x), y(p_y), z(p_z){}
	float x;
	float y;
	float z;
};

#endif // _N_INPUT_PREREQ_H_