// 
// 
// 

#include "mouse_controller.h"

MouseController::MouseController(int btnFunc_pin, int btnLB_pin, int btnRB_pin) : 
    btnFunc(btnFunc_pin, 0), btnLB(btnLB_pin, 0), btnRB(btnRB_pin, 0) {

    // buttons are set to default
    btnResetTime = DEFAULT_BTN_RESET_TIME;
	joystickHorizontalPin = DEFAULT_JOYSTICK_HORIZONTAL_PIN;
	joystickVerticalPin = DEFAULT_JOYSTICK_VERTICAL_PIN;
	btnFuncPin = btnFunc_pin;
	btnLBPin = btnLB_pin;
	btnRBPin = btnRB_pin;
	debounceTime = DEFAULT_DEBOUNCE_INTERVAL_MS;

	initialize();

    // filters are set to default
    setupFilters(FILTER_TYPE, CUTOFF_FREQ);

    // initialize other attributes
    mouseMode = MOVE;
    LB = RB = RELEASED;
	upDown = leftRight = CENTER;
    pulseLength = risingMoment = 0;
    dt = 0;
    time = millis();
}

void MouseController::initialize() {
	setupButtonsDebounce(debounceTime);
	pinMode(btnFuncPin, INPUT_PULLUP);
	pinMode(btnLBPin, INPUT_PULLDOWN);
	pinMode(btnRBPin, INPUT_PULLDOWN);
	pinMode(joystickHorizontalPin, INPUT);
	pinMode(joystickVerticalPin, INPUT);
}

void MouseController::checkModes() {
    // check function button changing state
    if (btnFunc.update()) _FuncUpdate();
    // check left button state
    if (btnLB.update()) _LBUpdate();
    // check right button state
    if (btnRB.update()) _RBUpdate();
	// check joystick's horizontal movement
	float horizontal = analogRead(joystickHorizontalPin);
	if (horizontal >= JOYSTICK_UPPER_THRESHOLD) leftRight = RIGHT;
	else if (horizontal <= JOYSTICK_LOWER_THRESHOLD) leftRight = LEFT;
	else leftRight = CENTER;
	// check joystick's vertical movement
	float vertical = analogRead(joystickVerticalPin);
	if (vertical >= JOYSTICK_UPPER_THRESHOLD) upDown = UP;
	else if (vertical <= JOYSTICK_LOWER_THRESHOLD) upDown = DOWN;
	else upDown = CENTER;
}

Frame MouseController::updateFrame(Vect2D<int8_t> delta) {
	// check buttons' changements
	checkModes();
    // switch mouse mode
    switch (mouseMode) {
    case MOVE:
        _setFrame(MOVE, LB, RB, upDown, leftRight, delta.x, delta.y);
        break;
    case STOP:
        _setFrame(STOP, LB, RB, upDown, leftRight, NO_MOVEMENT, NO_MOVEMENT);
        break;
    case RESET:
        _setFrame(RESET, RELEASED, RELEASED, CENTER, CENTER, NO_MOVEMENT, NO_MOVEMENT);
        mouseMode = MOVE;
        break;
    default:
        _setFrame(STOP, RELEASED, RELEASED, CENTER, CENTER, NO_MOVEMENT, NO_MOVEMENT);
        break;
    }
    return frame;
}

Frame MouseController::updateFrame(Vect3D<float> gyro, TaitBryan tb_angles, bool dynamic) {
	// compute mouse position
    Vect2D<int8_t> delta = calculateMousePos(convert3DTo2DMovement(gyro, tb_angles, dynamic));
	return updateFrame(delta);
}

void MouseController::_setFrame(int8_t mouseMode, int8_t lb, int8_t rb, 
                                int8_t vertical, int8_t horizontal, int8_t dx, int8_t dy) {
    frame.data[MOUSE_MODE]  = mouseMode;
    frame.data[LB_STATE]    = lb;
    frame.data[RB_STATE]    = rb;
    frame.data[UP_DOWN]     = vertical;
    frame.data[LEFT_RIGHT]  = horizontal;
    frame.data[DELTA_X]     = dx;
    frame.data[DELTA_Y]     = dy;
    frame.data[END_FRAME]   = END_FRAME_VAL;
}


Vect2D<int> MouseController::convert3DTo2DMovement(Vect3D<float> gyro, TaitBryan tb_angles, bool dynamic) {
    Vect2D<int> delta;
    if (dynamic) {
        delta.x = (int)(K * (gyro.z * cosf(tb_angles.roll) +
                             gyro.y * sinf(tb_angles.roll) / cosf(tb_angles.pitch)));
        delta.y = (int)(K * (gyro.y * cosf(tb_angles.roll) -
                             gyro.z * sinf(tb_angles.roll)));
    } else {
        delta.x = (int)(K * gyro.z);
        delta.y = (int)(K * gyro.y);
    }
    for (int i = 0; i < FILTER_ITER; i++) {
        delta.x = lpfX.input(delta.x);
        delta.y = lpfY.input(delta.y);
    }
    return delta;
}

Vect2D<int8_t> MouseController::calculateMousePos(Vect2D<int> delta) {
    // delimit movement interval
    if (delta.x > LIMIT_MOVEMENT) delta.x = LIMIT_MOVEMENT;
    else if (delta.x < (-1) * LIMIT_MOVEMENT) delta.x = (-1) * LIMIT_MOVEMENT;
    if (delta.y > LIMIT_MOVEMENT) delta.y = LIMIT_MOVEMENT;
    else if (delta.y < (-1) * LIMIT_MOVEMENT) delta.y = (-1) * LIMIT_MOVEMENT;

    // update x and y values, avoiding command characters (00-32 and 127)
    Vect2D<int8_t> ret;
    ret.x = (int8_t)delta.x;
    ret.y = (int8_t)delta.y;
    return ret;
}

void MouseController::setupButtonsDebounce(unsigned long interval_millis) {
    btnFunc.interval(interval_millis);
    btnLB.interval(interval_millis);
    btnRB.interval(interval_millis);
}

void MouseController::setupFilters(OSCILLATOR_TYPE type, float cutoffFreq) {
    lpfX.setAsFilter(type, cutoffFreq);
    lpfY.setAsFilter(type, cutoffFreq);
}

void MouseController::_FuncUpdate() {
    // rising edge event (or falling edge, depends on button)
    if (btnFunc.fallingEdge()) {
        // memorize time at rising edge and stop the mouse
        risingMoment = millis();
        mouseMode = STOP;
    }

    // falling edge event (or rising edge, depends on button)
    if (btnFunc.risingEdge()) {
        // calculate pulse length at falling edge
        pulseLength = millis() - risingMoment;

        // mode 1: button click - reset position by sending command 'R'
        if (pulseLength <= DEFAULT_BTN_RESET_TIME) mouseMode = RESET;
        // mode 2: button pressed - NaN, to be configured
        else mouseMode = MOVE;
    }

    // reset pulse length for the next event
    // attention: this is performed only when an update event
    pulseLength = 0;
}

void MouseController::_LBUpdate() {
    if (btnLB.risingEdge()) LB = PRESSED;
    if (btnLB.fallingEdge()) LB = RELEASED;
}

void MouseController::_RBUpdate() {
    if (btnRB.risingEdge()) RB = PRESSED;
    if (btnRB.fallingEdge()) RB = RELEASED;
}



