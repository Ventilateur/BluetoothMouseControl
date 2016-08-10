// mouse_controller.h

#ifndef _MOUSE_CONTROLLER_h
#define _MOUSE_CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#ifndef INCLUDE_IN_INO
    #include <FilterTwoPole.h>
    #include <Bounce.h>
    #include "math_3D_IMU.h" 
#endif // !INCLUDE_IN_INO

// default filters settings
#define FILTER_TYPE             (LOWPASS_BESSEL)
#define CUTOFF_FREQ             (10.0f)
#define FILTER_ITER             (5)

// default settings for all buttons
#define DEFAULT_BTN_FUNC_PIN            (20)
#define DEFAULT_BTN_LB_PIN              (6)
#define DEFAULT_BTN_RB_PIN              (2)
#define DEFAULT_JOYSTICK_HORIZONTAL_PIN (22)
#define DEFAULT_JOYSTICK_VERTICAL_PIN   (23)
#define DEFAULT_BTN_RESET_TIME          (200)
#define DEFAULT_DEBOUNCE_INTERVAL_MS    (50)

// joystick's constants
#define JOYSTICK_UPPER_THRESHOLD		(900)
#define JOYSTICK_LOWER_THRESHOLD		(100)

// frame constants
#define FRAME_SIZE                      (8)	// refers to the number of elements in FrameComponent
#define LIMIT_MOVEMENT		            (80)
#define NO_MOVEMENT                     (0)
#define END_FRAME_VAL                   ((char)127)

// default conversion constant
#define K                               (1)

// modes
#define SW_MODE_ANGLE                   (90.0f)

enum FrameComponent {
    MOUSE_MODE, 
    LB_STATE, 
    RB_STATE,
    UP_DOWN,
    LEFT_RIGHT, 
    DELTA_X, 
    DELTA_Y, 
    END_FRAME 
};

enum MouseModes { MOVE = 'M', STOP = 'S', RESET = 'R' };
enum ButtonModes { PRESSED = 'P', RELEASED = 'R' };
enum JoystickModes { UP = 'U', DOWN = 'D', LEFT = 'L', RIGHT = 'R', CENTER = 'C'};

struct Frame {
	char data[FRAME_SIZE] = 
		{ STOP, RELEASED, RELEASED, CENTER, CENTER, NO_MOVEMENT, NO_MOVEMENT, END_FRAME_VAL };
	const int size = FRAME_SIZE;
	bool operator==(const Frame& another) {
		bool ret = true;
		for (int i = 0; i < FRAME_SIZE; i++) {
			ret = ret && (data[i] == another.data[i]);
		}
		return ret;
	}
};

class MouseController {

public:
    MouseController(int btnFunc_pin, int btnLB_pin, int btnRB_pin);
	void initialize();
    void checkModes();
    Frame getFrame() { return frame; }
    Frame updateFrame(Vect3D<float> gyro, TaitBryan tb_angles, bool dynamic = true);

	// this is a clearer method for updating the frame, call them in order
	Vect2D<int> convert3DTo2DMovement(Vect3D<float> gyro, TaitBryan tb_angles, bool dynamic = true);
	Vect2D<int8_t> calculateMousePos(Vect2D<int> delta);
	Frame updateFrame(Vect2D<int8_t> delta);

	// setup methods - optional
	void setupButtonsDebounce(unsigned long interval_millis);
	void setupFilters(OSCILLATOR_TYPE type, float cutoffFreq);

private:
    // used for storing time
    int dt, time;
    // used filtering data
    FilterTwoPole lpfX, lpfY;
    // used for memorizing the reference of position
    TaitBryan tb_angles_ref;
    // used for reset mouse position
    int btnResetTime;
    // used for measuring button pulse length
    int pulseLength;
    int risingMoment;
    // used for debouncing buttons
    Bounce btnFunc, btnLB, btnRB;
	unsigned long debounceTime;
	// buttons' pins
	int btnFuncPin;
	int btnLBPin;
	int btnRBPin;
	// joystick's pins
	int joystickHorizontalPin;
	int joystickVerticalPin;
    // used for sending data
    Frame frame;
	Frame lastFrame;
    MouseModes mouseMode;
    ButtonModes LB;
    ButtonModes RB;
	JoystickModes upDown;
	JoystickModes leftRight;
    // private methods
    void _FuncUpdate();
    void _LBUpdate();
    void _RBUpdate();
    void _setFrame(int8_t mouseMode, int8_t LB, int8_t RB, 
				   int8_t vertical, int8_t horizontal, int8_t dx, int8_t dy);

public:
	// send frame to target, either an usb port or a SoftwareSerial object
	template <typename TYPE> void sendFrame(TYPE target, bool eco = true, const String& ext = "") {
		if (ext.length() <= 0) {
			if (eco) {
				if (!(frame == lastFrame && 
					frame.data[UP_DOWN] == CENTER && 
					frame.data[LEFT_RIGHT] == CENTER)) {
					target.write(frame.data, frame.size);
					target.flush();
				}
			} else {
				target.write(frame.data, frame.size);
				target.flush();
			}
		} else {
			target.print(ext);
			target.flush();
		}
		for (int i = 0; i < FRAME_SIZE; i++) lastFrame.data[i] = frame.data[i];
	}
};

#endif

