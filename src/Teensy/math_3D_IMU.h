// math_3D_IMU.h

#ifndef _MATH_3D_IMU_h
#define _MATH_3D_IMU_h

#include "arduino.h"

inline float deg2rad(float val) { return ((val / 180.0f) * PI); }

inline float rad2deg(float val) { return ((val * 180.0f) / PI); }

inline float calculateMagnitude3D(float x, float y, float z) {
    return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

class SphericalCoordinate {
public:
	SphericalCoordinate();
	SphericalCoordinate(float r, float teta, float phi);
	float calculateRFromCartesian(float x, float y, float z);
	float calculateTetaFromCartesian(float x, float y, float z);
	float calculatePhiFromCartesian(float x, float y, float z);

	// getters + setters
	float getR() { return r; }
	float getTeta() { return teta; }
	float getPhi() { return phi; }
	void setR(float val) { r = val; }
	void setTeta(float val) { teta = val; }
	void setPhi(float val) { phi = val; }

private:
	float r;
	float teta;
	float phi;
};

class Quaternion {
public:
	float w;
	float x;
	float y;
	float z;

	Quaternion();
	Quaternion(float nw, float nx, float ny, float nz);
	Quaternion conj();
	float magnitude();
	Quaternion norm();
};

Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);

class Vect3D_int16 {
public:
	int16_t x;
	int16_t y;
	int16_t z;
	Vect3D_int16();
	Vect3D_int16(int16_t nx, int16_t ny, int16_t nz);
	float magnitude();
	Vect3D_int16 norm();
};

class Vect3D_float {
public:
	float x;
	float y;
	float z;
	Vect3D_float();
	Vect3D_float(float nx, float ny, float nz);
	float magnitude();
	Vect3D_float norm();
};

class TaitBryan {
public:
	float yaw;
	float pitch;
	float roll;
	TaitBryan();
	TaitBryan(float y, float p, float r);
	void getFrom3dVect(Vect3D_float accel, Vect3D_float gyro, Vect3D_float magne);
	void getAnglesInDegFromQuaternion(Quaternion q);
    void getAnglesInRadFromQuaternion(Quaternion q);
};

#endif

