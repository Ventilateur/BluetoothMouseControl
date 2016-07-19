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

// general configurations 
#define DYNAMIQUE_ANGLES    (true)
#define ECO_MODE            (true)
#define BT_BAUDRATE			(57600)
#define K                   (1)

// debug modes
#define DEBUG				(false)
#define LOOP_RATE_DISP		(false)

// buttons' constants
#define FUNC_BUTTON			(20)
#define LEFT_BUTTON			(6)
#define RIGHT_BUTTON		(2)
#define BUTTON_RESET_TIME	(200)
#define BUTTON_CALIB_TIME   (2500)

// MPU configuration
#define SAMPLE_RATE			(7)
#define LPF_MODE			(5)
#define GYRO_RANGE_MODE		(2)
#define ACCEL_RANGE_MODE	(2)
#define INT_ENABLE			(true)

// gyroscope's filters configuration
#define FILTER_TYPE_GYRO	(LOWPASS_BESSEL)
#define CUTOFF_FREQ_GYRO	(10.0f)
#define GYRO_FILTER_ITER	(5)

// depX's and depY's filters configuration 
#define FILTER_TYPE_DEP 	(LOWPASS_BESSEL)
#define CUTOFF_FREQ_DEP 	(10.0f)
#define DEP_FILTER_ITER 	(5)

// roll's and pitch's filters configuration 
#define FILTER_TYPE_RP   	(LOWPASS_BESSEL)
#define CUTOFF_FREQ_RP   	(10.0f)
#define RP_FILTER_ITER  	(5)

// other stuffs
#define LOOP_DELAY_MS		(0)
#define SEND_RATE_MS		(0)

// frame constants
#define OFFSET_ASCII		(32)
#define DEL_CHAR			(127)
#define LIMIT_MOVEMENT		(80)

// modes
enum MouseModes {MOVE = 'M', STOP = 'S', RESET = 'R'};
enum ButtonModes {PRESSED = 'P', RELEASED = 'R'};

#endif

