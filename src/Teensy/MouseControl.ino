

#include "debug_helper.h"
#include <MadgwickAHRS.h>
#include <SoftwareSerial.h>
#include <FilterTwoPole.h>
#include <MPU6050.h>
#include "math_3D_IMU.h"
#include <Bounce.h>
#include <I2Cdev.h>
#include <i2c_t3.h>
#include "MPU9150.h"
#include "constants.h"

#define BUILT_IN_LED	13
#define BLUETOOTH		Serial2

// for absolute orientation
Madgwick madgwick;
TaitBryan tb_angles;

Vect3D_float accel, gyro, mag;
int depX, depY, depZ;

uint16_t delt_t = 0;							// used to control display output rate
long dt = 0;

int time = 0;									// store actual time
int pulseLength = 0, tmp = 0;					// for button measurement

// frame to be sent
char frame[6];
MouseModes mouseMode = MOVE;
ButtonModes LB = RELEASED;
ButtonModes RB = RELEASED;
int8_t dx = OFFSET_ASCII;
int8_t dy = OFFSET_ASCII;

MPU9150 mpu;
FilterTwoPole lpfx, lpfy, lpfz, lpfDepX, lpfDepY, lpfRoll, lpfPitch;

Bounce buttonFunc(FUNC_BUTTON, 50);
Bounce buttonLeft(LEFT_BUTTON, 50);
Bounce buttonRight(RIGHT_BUTTON, 50);


void setup() {
	pinMode(BUILT_IN_LED, OUTPUT);
	digitalWrite(BUILT_IN_LED, HIGH);

	// set reset/calibrate button, 
	// 1 button pin goes to FUNC_BUTTON, the other goes to Vcc, because it's on INPUT_PULLDOWN
	pinMode(FUNC_BUTTON, INPUT_PULLDOWN);
	pinMode(LEFT_BUTTON, INPUT_PULLDOWN);
	pinMode(RIGHT_BUTTON, INPUT_PULLDOWN);

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

	// configure low-pass filters
	lpfx.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
	lpfy.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
	lpfz.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
    lpfDepX.setAsFilter(FILTER_TYPE_DEP, CUTOFF_FREQ_DEP);
    lpfDepY.setAsFilter(FILTER_TYPE_DEP, CUTOFF_FREQ_DEP);
    lpfRoll.setAsFilter(FILTER_TYPE_RP, CUTOFF_FREQ_RP);
    lpfPitch.setAsFilter(FILTER_TYPE_RP, CUTOFF_FREQ_RP);

	// print configuration datas
	Serial.println("Full scale gyro mode = " + String(mpu.getFullScaleGyroRange()));
	Serial.println("Full scale accel mode = " + String(mpu.getFullScaleAccelRange()));
}


void loop() {

	// calculate loop rate
	delt_t = millis() - time;
	time = millis();

#if LOOP_RATE_DISP == true
	Serial.println("Loop rate ms = " + String(delt_t));
	Serial.println("          Hz = " + String(1000.0f / delt_t));
#endif

	buttonCheck();

	// get raw value from the MPU
	if (mpu.getIntDataReadyStatus() == 1) {
		mpu.getGyroScaled(&gyro.x, &gyro.y, &gyro.z);
        getAbsoluteOrientation();
	}

	// gyro filtering
	for (int i = 0; i < GYRO_FILTER_ITER; i++) {
		gyro.x = lpfx.input(gyro.x);
		gyro.y = lpfy.input(gyro.y);
		gyro.z = lpfz.input(gyro.z);
	}

	// switch mouse mode
	switch (mouseMode) {
	case MOVE:
		calculateMousePos();	
		setFrameVal(MOVE, LB, RB, dx, dy);
#if ECO_MODE == true
        if (depX != 0 || depY != 0) {
            BLUETOOTH.print(F(frame));
            BLUETOOTH.flush();
        }
#else
        BLUETOOTH.print(F(frame));
        BLUETOOTH.flush();
#endif
		break;
	case STOP:
		setFrameVal(STOP, LB, RB, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.print(F(frame));
		BLUETOOTH.flush();
		break;
	case RESET:
		setFrameVal(RESET, RELEASED, RELEASED, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.print(F(frame));
		BLUETOOTH.flush();
		mouseMode = MOVE;
		break;
	default:
		setFrameVal(STOP, RELEASED, RELEASED, OFFSET_ASCII, OFFSET_ASCII);
		BLUETOOTH.print(F(frame));
		BLUETOOTH.flush();
		break;
	}
	
	// send data to default serial port (debug only)
#if DEBUG == true
    // println(tb_angles.pitch, tb_angles.roll, 0.0f);
    // float xx = gyro.z * cosf(tb_angles.roll) + gyro.y * sinf(tb_angles.roll) + gyro.x * sinf(tb_angles.pitch);
    // float yy = gyro.y * cosf(tb_angles.roll) - gyro.z * sinf(tb_angles.roll);
    // println(gyro.x, gyro.y, gyro.z);
    // Serial.println(calculateMagnitude3D(gyro.x, gyro.y, gyro.z));
    // Serial.println(gyro.y * cosf(tb_angles.roll) - gyro.z * cosf(tb_angles.roll));
    // print(gyro.z, calculateMagnitude3D(gyro.x, gyro.y, gyro.z), xx); comma;
    // println(gyro.y, calculateMagnitude3D(gyro.x, gyro.y, gyro.z), yy);
    println(depX, depY, (int)gyro.z);
    sflush;
#endif

	delay(LOOP_DELAY_MS);
}


void calculateMousePos() {

    // convert 3D IMU movement to 2D movement (depX and depY)
    convertTo2DMovement(gyro, tb_angles);

	// delimit movement interval
	if (depX > LIMIT_MOVEMENT) depX = LIMIT_MOVEMENT;
	else if (depX < (-1) * LIMIT_MOVEMENT) depX = (-1) * LIMIT_MOVEMENT;
	if (depY > LIMIT_MOVEMENT) depY = LIMIT_MOVEMENT;
	else if (depY < (-1) * LIMIT_MOVEMENT) depY = (-1) * LIMIT_MOVEMENT;

	// update x and y values, avoiding command characters (00-32 and 127)
	dx = (int8_t)depX >= 0 ? (int8_t)depX + OFFSET_ASCII : (int8_t)depX;
	if (dx == DEL_CHAR) dx = DEL_CHAR - 1;
	dy = (int8_t)depY >= 0 ? (int8_t)depY + OFFSET_ASCII : (int8_t)depY;
	if (dy == DEL_CHAR) dy = DEL_CHAR - 1;
}


void buttonCheck() {

	// check function button changing state
	if (buttonFunc.update()) {
		if (buttonFunc.risingEdge()) {
			// memorize time at rising edge and stop the mouse
			tmp = millis();							
			mouseMode = STOP;
		}
		if (buttonFunc.fallingEdge()) {					
			// calculate pulse length at falling edge
			pulseLength = millis() - tmp;
			// mode 1: button click - reset position by sending command 'R'
			if (pulseLength <= BUTTON_RESET_TIME) {
				mouseMode = RESET;
			// mode 2: button pressed - NaN, to be configured
			} else if (pulseLength <= BUTTON_CALIB_TIME) {
				mouseMode = MOVE;
            } else {
                mouseMode = MOVE;
            }
		}
		pulseLength = 0;	// reset pulse length for the next event
	}

	// check left button state
	if (buttonLeft.update()) {
		if (buttonLeft.risingEdge()) LB = PRESSED;
		if (buttonLeft.fallingEdge()) LB = RELEASED;
	}

	// check right button state
	if (buttonRight.update()) {
		if (buttonRight.risingEdge()) RB = PRESSED;
		if (buttonRight.fallingEdge()) RB = RELEASED;
	}
}


void setFrameVal(int8_t mouseMode, int8_t LB, int8_t RB, int8_t dx, int8_t dy) {
	frame[0] = mouseMode;
	frame[1] = LB;
	frame[2] = RB;
	frame[3] = dx;
	frame[4] = dy;
	frame[5] = '\n';
}


void getAbsoluteOrientation() {
    mpu.getAccelScaled(&accel.x, &accel.y, &accel.z);
    mpu.getMagnetoScaled(&mag.x, &mag.y, &mag.z);
    madgwick.update(1000.0f / delt_t, 5.0f, deg2rad(gyro.x), deg2rad(gyro.y), deg2rad(gyro.z),
                    accel.x, accel.y, accel.z, mag.x, mag.y, -mag.z);
    Quaternion quat(madgwick.q0, madgwick.q1, madgwick.q2, madgwick.q3);
    tb_angles.getAnglesInRadFromQuaternion(quat);
    for (int i = 0; i < RP_FILTER_ITER; i++) {
        tb_angles.roll = lpfRoll.input(tb_angles.roll);
        tb_angles.pitch = lpfPitch.input(tb_angles.pitch);
    }
}

void convertTo2DMovement(Vect3D_float gyro, TaitBryan tb_angles) {
#if DYNAMIQUE_ANGLES == true
    depX = K * (int)(gyro.z * cosf(tb_angles.roll) + gyro.y * sinf(tb_angles.roll) + gyro.x * sinf(tb_angles.pitch));
    depY = K * (int)(gyro.y * cosf(tb_angles.roll) - gyro.z * sinf(tb_angles.roll));
#else
    depX = K * (int)gyro.z;
    depY = K * (int)gyro.y;
#endif
    for (int i = 0; i < GYRO_FILTER_ITER; i++) {
        depX = lpfDepX.input(depX);
        depY = lpfDepY.input(depY);
    }
}