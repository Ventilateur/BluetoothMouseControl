// constants.h

#ifndef _CONSTANTS_h
#define _CONSTANTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


// ********************************************************** //
// ********************** MODIFIALBE ZONE ******************* //
// ********************************************************** //

#define BT_BAUDRATE			(57600)
#define FUNC_BUTTON			(20)
#define LEFT_BUTTON			(6)
#define RIGHT_BUTTON		(2)
#define BUTTON_RESET_TIME	(200)

// MPU configuration
#define SAMPLE_RATE			(7)
#define LPF_MODE			(5)
#define GYRO_RANGE_MODE		(0)
#define ACCEL_RANGE_MODE	(0)
#define INT_ENABLE			(true)

// gyroscope's filters configuration
#define FILTER_TYPE_GYRO	(LOWPASS_BESSEL)
#define CUTOFF_FREQ_GYRO	(5.0f)
#define GYRO_FILTER_ITER	(5)

// debug modes
#define DEBUG				(true)
#define LOOP_RATE_DISP		(false)

// other stuffs
#define LOOP_DELAY_MS		(0)
#define SEND_RATE_MS		(5)

// frame constants
#define OFFSET_ASCII		(32)
#define DEL_CHAR			(127)
#define LIMIT_DEPLACE		(80)

// modes
enum MouseModes {MOVE = 'M', STOP = 'S', RESET = 'R'};
enum ButtonModes {PRESSED = 'P', RELEASED = 'R'};

#endif

