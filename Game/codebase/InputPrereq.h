#ifndef _N_INPUT_PREREQ_H_
#define _N_INPUT_PREREQ_H_

enum EButtonType {
	BTN_UP = 0,
	BTN_DOWN,
	BTN_LEFT,
	BTN_RIGHT,
	BTN_START,
	BTN_BACK,
	BTN_W,
	BTN_A,
	BTN_S,
	BTN_D,
	BTN_SIZE
};

struct MousePosition{
	MousePosition() : x(0), y(0), rel_x(0), rel_y(0) {}
	int x;
	int y;
	int rel_x;
	int rel_y;
};

#endif // _N_INPUT_PREREQ_H_