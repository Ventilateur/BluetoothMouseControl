

#include <SoftwareSerial.h>
#include <FilterTwoPole.h>
#include <MadgwickAHRS.h>
#include <MPU6050.h>
#include "math_3D_IMU.h"
#include <Bounce.h>
#include <I2Cdev.h>
#include <i2c_t3.h>
#include "MPU9150.h"
#include "constants.h"

#define BUILT_IN_LED	13
#define BLUETOOTH Serial2

Vect3D_int16 aRaw, gRaw, mRaw;
Vect3D_float accel, gyro, mag;
YPR anglesPrev, angles;
int deltaRoll, deltaPitch;
Quaternion quat;
char frame[3];

uint16_t delt_t = 0;							// used to control display output rate
long dt = 0;
float dR = 0.0f, dP = 0.0f;

int time = 0;									// store actual time
int pulseLength = 0, tmp = 0;					// for button measurement

bool onButton = false;
MouseModes mouseMode = MOVE;

MPU9150 mpu;
Madgwick filter;
FilterTwoPole lpf1, lpf2, lpf3, lpf4;
Bounce button(BUTTON_PIN, 10);


void setup() {
	pinMode(BUILT_IN_LED, OUTPUT);
	digitalWrite(BUILT_IN_LED, HIGH);

	// set reset/calibrate button, 
	// 1 button pin goes to BUTTON_PIN, the other goes to Vcc, because it's on INPUT_PULLDOWN
	pinMode(BUTTON_PIN, INPUT_PULLDOWN);

	// setup bluetooth connection
	Serial.begin(BT_BAUDRATE);
	BLUETOOTH.begin(BT_BAUDRATE);

	// configure the MPU-9150
	Wire.begin();
	mpu.initialize();
	mpu.setRate(SAMPLE_RATE); 
	mpu.setDLPFMode(LPF_MODE); 
	mpu.setFullScaleGyroRange(GYRO_RANGE_MODE); 
	mpu.setFullScaleAccelRange(ACCEL_RANGE_MODE); 
	mpu.setIntDataReadyEnabled(INT_ENABLE); 

	// configure low pass filter for 3 channel yaw, pitch and roll
	lpf1.setAsFilter(FILTER_TYPE_12, CUTOFF_FREQ_12);
	lpf2.setAsFilter(FILTER_TYPE_12, CUTOFF_FREQ_12);
	lpf3.setAsFilter(FILTER_TYPE_34, CUTOFF_FREQ_34);
	lpf4.setAsFilter(FILTER_TYPE_34, CUTOFF_FREQ_34);
}


void loop() {

	// calculate loop rate
	delt_t = millis() - time;
	time = millis();
	#if LOOP_RATE_DISP == true
		Serial.print("Loop rate ms = " + String(delt_t) + "\r\n");
		Serial.print("          Hz = " + String(1000.0f / delt_t) + "\r\n");
	#endif

	buttonCheck();

	// get raw value from the MPU
	if (mpu.getIntDataReadyStatus() == 1) {
		mpu.getAccelScaled(&accel.x, &accel.y, &accel.z);
		mpu.getGyroScaled(&gyro.x, &gyro.y, &gyro.z);
	}

	// calculate angles
	filter.update(1000.0f / delt_t, deg2rad(gyro.x), deg2rad(gyro.y), deg2rad(gyro.z), accel.x, accel.y, accel.z, mag.y, mag.x, -mag.z);
	quat.w = filter.q0; quat.x = filter.q1; quat.y = filter.q2; quat.z = filter.q3;
	angles.getFromQuaternion(quat);

	// low filter the results
	for (int i = 1; i < ITERATION_12; i++) {
		angles.pitch = lpf1.input(angles.pitch);
		angles.roll = lpf2.input(angles.roll);
	}

	// switch mouse mode
	switch (mouseMode) {
	case MOVE:
		if (millis() - dt > SEND_RATE_MS) {
			dt = millis();
			calculateMousePos();	// calculate position and set frame
			BLUETOOTH.println(F(frame));
			BLUETOOTH.flush();
		}
		break;
	case STOP:
		setFrameVal(STOP, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.println(F(frame));
		BLUETOOTH.flush();
		break;
	case RESET:
		setFrameVal(RESET, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.println(F(frame));
		BLUETOOTH.flush();
		mouseMode = MOVE;
		break;
	default:
		setFrameVal(STOP, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.println(F(frame));
		BLUETOOTH.flush();
		break;
	}
	
	// send data to default serial port (debug only)
	Serial.print("Roll, Pitch = " + String(deltaRoll) + "," + deltaPitch + "\r\n");
	Serial.flush();

	// update values
	anglesPrev.pitch = angles.pitch;
	anglesPrev.roll = angles.roll;

	delay(LOOP_DELAY_MS);
}


void calculateMousePos() {
	// calculate relative mouvement
	dP = angles.pitch - anglesPrev.pitch;
	dR = angles.roll - anglesPrev.roll;
	for (int i = 1; i < ITERATION_34; i++) {
		dR = lpf3.input(dR);
		dP = lpf4.input(dP);
	}
	deltaRoll = (int)((dR)* DELTA_MULTIPLIER);
	deltaPitch = (int)((dP)* DELTA_MULTIPLIER);

	// limit deplacement interval
	if (deltaRoll > LIMIT_DEPLACE) deltaRoll = LIMIT_DEPLACE;
	else if (deltaRoll < (-1) * LIMIT_DEPLACE) deltaRoll = (-1) * LIMIT_DEPLACE;
	if (deltaPitch > LIMIT_DEPLACE) deltaPitch = LIMIT_DEPLACE;
	else if (deltaPitch < (-1) * LIMIT_DEPLACE) deltaPitch = (-1) * LIMIT_DEPLACE;


	// construct a communication frame, avoiding command characters (00-32 and 127)
	int8_t f1 = (int8_t)deltaRoll >= 0 ? (int8_t)deltaRoll + OFFSET_ASCII : (int8_t)deltaRoll;
	if (f1 == DEL_CHAR) f1 = DEL_CHAR - 1;
	int8_t f2 = (int8_t)deltaPitch >= 0 ? (int8_t)deltaPitch + OFFSET_ASCII : (int8_t)deltaPitch;
	if (f2 == DEL_CHAR) f2 = DEL_CHAR - 1;
	setFrameVal(MOVE, f1, f2);
}


void buttonCheck() {

	// check button changing state
	if (button.update()) {
		Serial.println("maaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		if (button.risingEdge()) {
			// memorize time at rising edge and stop the mouse
			tmp = millis();							
			mouseMode = STOP;
		}
		if (button.fallingEdge()) {					
			// calculate pulse length at falling edge
			pulseLength = millis() - tmp;
			// mode 1: button click - reset position by sending command 'R'
			if (pulseLength <= BUTTON_RESET_TIME) {
				mouseMode = RESET;
			// mode 2: button pressed - NaN, to be configured
			} else {
				mouseMode = MOVE;
			}
		}
		pulseLength = 0;	// reset pulse length for the next event
	}
}


void setFrameVal(int8_t mode, int8_t r, int8_t p) {
	frame[0] = mode;
	frame[1] = r;
	frame[2] = p;
}