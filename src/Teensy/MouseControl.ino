
#define INCLUDE_IN_INO

#ifdef INCLUDE_IN_INO
    #include <FilterTwoPole.h>
    #include <Bounce.h>
    #include "math_3D_IMU.h"
    #include "constants.h"  
#endif // INCLUDE_IN_INO
  
#include <FilterTwoPole.h>
#include <MadgwickAHRS.h>
#include <I2Cdev.h>
#include <i2c_t3.h>
#include <MPU6050.h>
#include "mouse_controller.h"
#include "debug_helper.h"
#include "MPU9150.h"
#include "constants.h"


MouseController mouse(FUNC_BUTTON, LEFT_BUTTON, RIGHT_BUTTON);
ThreeChannelsLPF lpf;
ThreeChannelsLPF gyroLpf, accelLpf;
MPU9150 mpu;
Madgwick madgwick;
data6Dof<float> data;
int dt, time, blinkTime = 0;
bool ledOn = false;

void getMotion();
TaitBryan getOrientation(Vect3D<float> gyro, Vect3D<float> accel);

void setup() {
    Serial.begin(BT_BAUDRATE);
    BLUETOOTH.begin(BT_BAUDRATE);
    setupIMU();
    lpf.setFilters(LOWPASS_BESSEL, 5.0f);
    gyroLpf.setFilters(LOWPASS_BESSEL, 5.0f);
    accelLpf.setFilters(LOWPASS_BESSEL, 5.0f);
    dt = 0;
    time = millis();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    dt = millis() - time;
    time = millis();
    mouse.checkModes();
    getMotion();
    TaitBryan tb = getOrientation(data.gyro, data.accel);
    Vect2D<int> d1 = mouse.convert3DTo2DMovement(data.gyro, tb, true);
    Vect2D<int8_t> d2 = mouse.calculateMousePos(d1);
    mouse.updateFrame(d2); 
    mouse.sendFrame(Serial);
    //println(DEG(tb.roll), DEG(tb.pitch));
    //println(d2.x, d2.y);
    if (millis() - blinkTime > 500) {
        ledOn = !ledOn;
        blinkTime = millis();
    }
    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
    delay(10);
}

void setupIMU() {
    Wire.begin();
    mpu.initialize();
    mpu.setRate(SAMPLE_RATE);
    mpu.setDLPFMode(LPF_MODE);
    mpu.setFullScaleGyroRange(GYRO_RANGE_MODE);
    mpu.setFullScaleAccelRange(ACCEL_RANGE_MODE);
    mpu.setIntDataReadyEnabled(INT_ENABLE);
}

void getMotion() {
    if (mpu.getIntDataReadyStatus() == 1) {
        mpu.getGyroScaled(&data.gyro.x, &data.gyro.y, &data.gyro.z);
        mpu.getAccelScaled(&data.accel.x, &data.accel.y, &data.accel.z);
    }
    for (int i = 0; i < 5; i++) {
        data.gyro = gyroLpf.filter(data.gyro.x, data.gyro.y, data.gyro.z);
        data.accel = accelLpf.filter(data.accel.x, data.accel.y, data.accel.z);
    }
}

TaitBryan getOrientation(Vect3D<float> gyro, Vect3D<float> accel) {
    static int last = 0;
    int dt = millis() - last;
    last = millis();
    madgwick.update(freq(dt), BETA,
                    radians(gyro.x), radians(gyro.y), radians(gyro.z),
                    accel.x, accel.y, accel.z, 1.0f, 1.0f, 1.0f);
    TaitBryan tb_angles(Quaternion(madgwick.q0, madgwick.q1, madgwick.q2, madgwick.q3));
    Vect3D<float> ret;
    for (int i = 0; i < 5; i++) ret = lpf.filter(tb_angles.roll, tb_angles.pitch, 0.0f);
    tb_angles.roll = ret.x; tb_angles.pitch = ret.y;
    return tb_angles;
}