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
#define USE_BT				(true)
#define BT_TX_PIN			(10)
#define BT_RX_PIN			(9)
#define BUTTON_PIN			(20)

// MPU configuration
#define SAMPLE_RATE			(7)
#define LPF_MODE			(5)
#define GYRO_RANGE_MODE		(2)
#define ACCEL_RANGE_MODE	(2)
#define INT_ENABLE			(true)
#define BUTTON_RESET_TIME	(200)
#define CUTOFF_FREQ_12		(5.0f)
#define CUTOFF_FREQ_34		(2.0f)
#define ITERATION_12		(5)
#define ITERATION_34		(5)
#define FILTER_TYPE_12		(LOWPASS_BESSEL)
#define FILTER_TYPE_34		(LOWPASS_BESSEL)

// debug modes
#define CALIB_DISP			(false)
#define LOOP_RATE_DISP		(false)
#define GYRO_DISP			(false)
#define ACCEL_DISP			(false)
#define MAG_DISP			(false)
#define YPR_DISP			(true)
#define SEND_DATA			(true)

// other stuffs
#define LOOP_DELAY_MS		(0)
#define SEND_RATE_MS		(5)

// frame constants
#define OFFSET_ASCII		(32)
#define DEL_CHAR			(127)
#define LIMIT_DEPLACE		(80)
#define DELTA_MULTIPLIER	(100.0)

// modes
enum MouseModes {MOVE = 'M', STOP = 'S', RESET = 'R'};

#endif

