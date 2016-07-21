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
#define DEFAULT_FILTER_TYPE             (LOWPASS_BESSEL)
#define DEFAULT_CUTOFF_FREQ             (10.0f)
#define DEFAULT_FILTER_ITER             (5)

// default settings for all buttons
#define DEFAULT_BTN_FUNC_PIN            (20)
#define DEFAULT_BTN_LB_PIN              (6)
#define DEFAULT_BTN_RB_PIN              (2)
#define DEFAULT_BTN_RESET_TIME          (200)
#define DEFAULT_DEBOUNCE_INTERVAL_MS    (50)

// frame constants
#define FRAME_SIZE                      (6)
#define LIMIT_MOVEMENT		            (80)
#define NO_MOVEMENT                     (0)
#define END_FRAME_VAL                   ((char)127)

enum FrameComponent {
    MOUSE_MODE, 
    LB_STATE, 
    RB_STATE, 
    DELTA_X, 
    DELTA_Y, 
    END_FRAME 
};

// default conversion constant
#define K                               (1)

// modes
#define SW_MODE_ANGLE                   (90.0f)
enum MouseModes { MOVE = 'M', STOP = 'S', RESET = 'R' };
enum ButtonModes { PRESSED = 'P', RELEASED = 'R' };


class MouseController {
public:

    struct Frame {
        char data[FRAME_SIZE];
        const int size = FRAME_SIZE;
    };

    MouseController(int btnFunc_pin, int btnLB_pin, int btnRB_pin);
    void checkModes();
    void setModes(MouseModes mouse_mode, ButtonModes left, ButtonModes right);
    Frame getFrame() { return frame; }
    Frame updateFrame(Vect3D<float> gyro, TaitBryan tb_angles, bool dynamic = true);

    // send frame to target, either an usb port or a SoftwareSerial object
    template <typename TYPE> void sendFrame(TYPE target, bool eco = true, const String& ext = "") {
        if (ext.length() <= 0) {
            if (eco) {
                if (!(frame.data[MOUSE_MODE] == MOVE &&
                    fabs(frame.data[DELTA_X]) == NO_MOVEMENT &&
                    fabs(frame.data[DELTA_Y]) == NO_MOVEMENT)) {
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
    }

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
    // used for sending data
    Frame frame;
    MouseModes mouseMode;
    ButtonModes LB;
    ButtonModes RB;
    // private methods
    void _FuncUpdate();
    void _LBUpdate();
    void _RBUpdate();
    void _setFrame(int8_t mouseMode, int8_t LB, int8_t RB, int8_t dx, int8_t dy);
};

#endif

