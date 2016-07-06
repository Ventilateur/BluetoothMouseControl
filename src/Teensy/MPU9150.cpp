
#include "MPU9150.h"
#include "math_3D_IMU.h"

MPU9150::MPU9150() : MPU6050() {
	max_x = MAX_X, max_y = MAX_Y, max_z = MAX_Z;
	min_x = MIN_X, min_y = MIN_Y, min_z = MIN_Z;
	offset_x = (MAX_X + MIN_X) / 2.0f;
	offset_y = (MAX_Y + MIN_Y) / 2.0f;
	offset_z = (MAX_Z + MIN_Z) / 2.0f;
	scale_x = SCALE_X, scale_y = SCALE_Y, scale_z = SCALE_Z;
}

void MPU9150::calibrate() {
	Vect3D_float mag;
	if (getIntDataReadyStatus() == 1) {
		getMagnetoScaled(&mag.x, &mag.y, &mag.z);
		if (min_x >= mag.x) min_x = mag.x; if (max_x <  mag.x) max_x = mag.x;
		if (min_y >= mag.y) min_y = mag.y; if (max_y <  mag.y) max_y = mag.y;
		if (min_z >= mag.z) min_z = mag.z; if (max_z <  mag.z) max_z = mag.z;
		calculateMag();
	}
}


void MPU9150::calculateMag() {
	// correct hard iron errors
	offset_x = (max_x + min_x) / 2.0f;
	offset_y = (max_y + min_y) / 2.0f;
	offset_z = (max_z + min_z) / 2.0f;

	// correct soft iron errors
	avg_x = (max_x - min_x) / 2.0f;
	avg_y = (max_y - min_y) / 2.0f;
	avg_z = (max_z - min_z) / 2.0f;
	avg_xyz = (avg_x + avg_y + avg_z) / 3.0f;

	(avg_x != 0.0f) ? (scale_x = avg_xyz / avg_x) : (scale_x = 1.0f);
	(avg_y != 0.0f) ? (scale_y = avg_xyz / avg_y) : (scale_y = 1.0f);
	(avg_z != 0.0f) ? (scale_z = avg_xyz / avg_z) : (scale_z = 1.0f);
}


void MPU9150::resetMag() {
	max_x = MAX_X; min_x = MIN_X;
	max_y = MAX_Y; min_y = MIN_Y;
	max_z = MAX_Z; min_z = MIN_Z;
}


void MPU9150::resetForCalib() {
	max_x = -2000.0f; max_y = -2000.0f; max_z = -2000.0f;
	min_x = 2000.0f; min_y = 2000.0f; min_z = 2000.0f;
	offset_x = 0.0f; offset_y = 0.0f; offset_z = 0.0f;
	scale_x = 1.0f; scale_y = 1.0f; scale_z = 1.0f;
}


float MPU9150::normalize(int16_t val, int16_t range) {
	return ((float)(val*range) / (float)MAX_16_BITS_VAL);
}


float MPU9150::normalizeMag(int16_t val) {
	return ((float)(val*MAG_FSR) / (float)MAX_13_BITS_VAL);
}


void MPU9150::getAccelScaled(float *ax, float *ay, float *az) {
	int16_t a1, a2, a3;
	getAcceleration(&a1, &a2, &a3);
	*ax = normalize(a1, ACCEL_RANGE);
	*ay = normalize(a2, ACCEL_RANGE);
	*az = normalize(a3, ACCEL_RANGE);
}


void MPU9150::getGyroScaled(float *gx, float *gy, float *gz) {
	int16_t g1, g2, g3;
	getRotation(&g1, &g2, &g3);
	*gx = normalize(g1, GYRO_RANGE) - G_OFFSET_X;
	*gy = normalize(g2, GYRO_RANGE) - G_OFFSET_Y;
	*gz = normalize(g3, GYRO_RANGE) - G_OFFSET_Z;
}


void MPU9150::getMagnetoScaled(float *mx, float *my, float *mz) {
	int16_t m1, m2, m3;
	getMag(&m1, &m2, &m3);
	*mx = normalizeMag(m1);
	*my = normalizeMag(m2);
	*mz = normalizeMag(m3);
}


void MPU9150::correctMag(float *mx, float *my, float *mz) {
	calculateMag();
	*mx = (*mx - offset_x) * scale_x;
	*my = (*my - offset_y) * scale_y;
	*mz = (*mz - offset_z) * scale_z;
}
