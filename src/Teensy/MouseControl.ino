

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

Vect3D_float accel, gyro, mag;
int depX, depY, depZ;

uint16_t delt_t = 0;							// used to control display output rate
long dt = 0;
float dR = 0.0f, dP = 0.0f;

int time = 0;									// store actual time
int pulseLength = 0, tmp = 0;					// for button measurement

// frame to be sent
char frame[6];
MouseModes mouseMode = MOVE;
ButtonModes LB = RELEASED;
ButtonModes RB = RELEASED;
int8_t r = OFFSET_ASCII;
int8_t p = OFFSET_ASCII;

MPU9150 mpu;
FilterTwoPole lpfx, lpfy, lpfz;

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

	// configure filter for gyro's datas
	lpfx.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
	lpfy.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
	lpfz.setAsFilter(FILTER_TYPE_GYRO, CUTOFF_FREQ_GYRO);
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

	// gyro filtering
	for (int i = 0; i < GYRO_FILTER_ITER; i++) {
		gyro.x = lpfx.input(gyro.x);
		gyro.y = lpfy.input(gyro.y);
		gyro.z = lpfz.input(gyro.z);
	}

	// switch mouse mode
	switch (mouseMode) {
	case MOVE:
		if (millis() - dt > SEND_RATE_MS) {
			dt = millis();
			calculateMousePos();	
			setFrameVal(MOVE, LB, RB, r, p);
			BLUETOOTH.print(F(frame));
			BLUETOOTH.flush();
		}
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
	Serial.println("XYZ = " + String(gyro.x) + " , " + gyro.y + " , " + gyro.z);
	Serial.flush();

	delay(LOOP_DELAY_MS);
}


void calculateMousePos() {
	// horizontal mouse movement (axe X) coressponds to the rotation around axe Z of the gyro
	// vertical mouse movement (axe Y) coressponds to the rotation around axe Y of the gyro
	depX = (int)gyro.z;
	depY = (int)gyro.y;

	// limit deplacement interval
	if (depX > LIMIT_DEPLACE) depX = LIMIT_DEPLACE;
	else if (depX < (-1) * LIMIT_DEPLACE) depX = (-1) * LIMIT_DEPLACE;
	if (depY > LIMIT_DEPLACE) depY = LIMIT_DEPLACE;
	else if (depY < (-1) * LIMIT_DEPLACE) depY = (-1) * LIMIT_DEPLACE;

	// update r and p values, avoiding command characters (00-32 and 127)
	r = (int8_t)depX >= 0 ? (int8_t)depX + OFFSET_ASCII : (int8_t)depX;
	if (r == DEL_CHAR) r = DEL_CHAR - 1;
	p = (int8_t)depY >= 0 ? (int8_t)depY + OFFSET_ASCII : (int8_t)depY;
	if (p == DEL_CHAR) p = DEL_CHAR - 1;
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


void setFrameVal(int8_t mouseMode, int8_t LB, int8_t RB, int8_t r, int8_t p) {
	frame[0] = mouseMode;
	frame[1] = LB;
	frame[2] = RB;
	frame[3] = r;
	frame[4] = p;
	frame[5] = '\n';
}