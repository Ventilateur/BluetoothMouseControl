// math_3D_IMU.h

#ifndef _MATH_3D_IMU_h
#define _MATH_3D_IMU_h

#include "arduino.h"

#ifndef INCLUDE_IN_INO
    #include <FilterTwoPole.h>
#endif // !INCLUDE_IN_INO


#define RAD(val)    ((val / 180.0f) * PI)
#define DEG(val)    ((val * 180.0f) / PI)


inline float deg2rad(float val) { return ((val / 180.0f) * PI); }

inline float rad2deg(float val) { return ((val * 180.0f) / PI); }


template <typename T>
struct Vect2D {
    T x;
    T y;
};

template <typename T>
class Vect3D {
public:
    T x;
    T y;
    T z;
    Vect3D() { x = y = z = 0.0f; }
    Vect3D(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }
    T magnitude() { return sqrt(x*x + y*y + z*z); }
    Vect3D<T> norm() {
        Vect3D ret(x, y, z);
        T m = ret.magnitude();
        ret.x = m; ret.y /= m; ret.z /= m;
        return ret;
    }
};


template <typename T>
class data6Dof {
public:
    data6Dof() {}
    data6Dof(Vect3D<T> gyro, Vect3D<T> accel) {
        this->gyro = gyro;
        this->accel = accel;
    }
    Vect3D<T> gyro;
    Vect3D<T> accel;
};


template <typename T>
class data9Dof {
public:
    data9Dof() {}
    data9Dof(Vect3D<T> gyro, Vect3D<T> accel, Vect3D<T> mag) {
        this->gyro = gyro;
        this->accel = accel;
        this->mag = mag;
    }
    Vect3D<T> gyro;
    Vect3D<T> accel;
    Vect3D<T> mag;
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


class TaitBryan {
public:
	float yaw;
	float pitch;
	float roll;
	TaitBryan();
    TaitBryan(Quaternion q);
	TaitBryan(float y, float p, float r);
    void getFrom3dVect(Vect3D<float> accel, Vect3D<float> gyro, Vect3D<float> magne);
	void getAnglesInDegFromQuaternion(Quaternion q);
    void getAnglesInRadFromQuaternion(Quaternion q);
};

TaitBryan operator-(const TaitBryan& a, const TaitBryan& b);


class ThreeChannelsLPF {
public:
    ThreeChannelsLPF() {};
    void setFilters(OSCILLATOR_TYPE type, float cutoffFreq) {
        lpf1.setAsFilter(type, cutoffFreq);
        lpf2.setAsFilter(type, cutoffFreq);
        lpf3.setAsFilter(type, cutoffFreq);
    }
    Vect3D<float> filter(float ch1, float ch2, float ch3) {
        output.x = lpf1.input(ch1);
        output.y = lpf2.input(ch2);
        output.z = lpf3.input(ch3);
        return getFiltered();
    }
    Vect3D<float> getFiltered() { return output; }
private:
    FilterTwoPole lpf1;
    FilterTwoPole lpf2;
    FilterTwoPole lpf3;
    Vect3D<float> output;
};


#endif