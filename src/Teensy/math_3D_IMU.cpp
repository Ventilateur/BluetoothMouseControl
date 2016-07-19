
#include "math_3D_IMU.h"


// ==================== Quaternion section ===================== //

Quaternion::Quaternion() {
	w = 1.0f; x = 0.0f; y = 0.0f; z = 0.0f;
}

Quaternion::Quaternion(float nw, float nx, float ny, float nz) {
	w = nw; x = nx; y = ny; z = nz;
}

Quaternion Quaternion::conj() {
	return Quaternion(w, -x, -y, -z);
}

float Quaternion::magnitude() {
	return sqrt(w*w + x*x + y*y + z*z);
}

Quaternion Quaternion::norm() {
	Quaternion ret(w, x, y, z);
	float m = ret.magnitude();
	ret.w /= m; ret.x /= m;
	ret.y /= m; ret.z /= m;
	return ret;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
	Quaternion ret;
	ret.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
	ret.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
	ret.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
	ret.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
	return ret;
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
	Quaternion ret;
	ret.w = (q1.w - q2.w);
	ret.x = (q1.x - q2.x);
	ret.y = (q1.y - q2.y);
	ret.z = (q1.z - q2.z);
	return ret;
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
	Quaternion ret;
	ret.w = (q1.w + q2.w);
	ret.x = (q1.x + q2.x);
	ret.y = (q1.y + q2.y);
	ret.z = (q1.z + q2.z);
	return ret;
}


// ==================== 3D Vector section ====================== //

// ======== int_16t ======== //
Vect3D_int16::Vect3D_int16() {
	x = 1; y = 0; z = 0; 
}

Vect3D_int16::Vect3D_int16(int16_t nx, int16_t ny, int16_t nz) {
	x = nx; y = ny; z = nz;
}

float Vect3D_int16::magnitude() {
	return sqrt(x*x + y*y + z*z);
}

Vect3D_int16 Vect3D_int16::norm() {
	Vect3D_int16 ret(x, y, z);
	float m = ret.magnitude();
	ret.x /= m; ret.y /= m; ret.z /= m;
	return ret;
}

// ======== float ======== //
Vect3D_float::Vect3D_float() {
	x = 1.0f; y = 0.0f; z = 0.0f;
}

Vect3D_float::Vect3D_float(float nx, float ny, float nz) {
	x = nx; y = ny; z = nz;
}

float Vect3D_float::magnitude() {
	return sqrt(x*x + y*y + z*z);
}

Vect3D_float Vect3D_float::norm() {
	Vect3D_float ret(x, y, z);
	float m = ret.magnitude();
	ret.x /= m; ret.y /= m; ret.z /= m;
	return ret;
}


// ==================== Tait–Bryan angles section ====================== //

TaitBryan::TaitBryan() {
	yaw = 0.0f; pitch = 0.0f; roll = 0.0f;
}

TaitBryan::TaitBryan(float y, float p, float r) {
	yaw = y; pitch = p; roll = r;
}

void TaitBryan::getFrom3dVect(Vect3D_float accel, Vect3D_float gyro, Vect3D_float magne) {
	roll = (atan2(accel.y, accel.z));
	if (fabs(accel.y * sin(roll) + accel.z * cos(roll)) < 0.001f)
		pitch = accel.x > 0 ? (PI / 2.0f) : (-PI / 2.0f);
	else
		pitch = atan(-accel.x / (accel.y * sin(roll) + accel.z * cos(roll)));
	yaw = (atan2(magne.z * sin(roll) - magne.y * cos(roll), magne.x * cos(pitch) + \
		   magne.y * sin(pitch) * sin(roll) + \
		   magne.z * sin(pitch) * cos(roll)));
	roll = rad2deg(roll);
	pitch = rad2deg(pitch);
	yaw = rad2deg(yaw);
}

void TaitBryan::getAnglesInDegFromQuaternion(Quaternion q) {
	roll = atan2f((q.w * q.x + q.y * q.z), 0.5f - (q.x * q.x + q.y * q.y));
	pitch = asinf(-2.0f * (q.x * q.z - q.w * q.y));
	yaw = atan2f((q.x * q.y + q.w * q.z), 0.5f - (q.y * q.y + q.z * q.z));

	roll = rad2deg(roll);
	pitch = rad2deg(pitch);
	yaw = rad2deg(yaw);
}

void TaitBryan::getAnglesInRadFromQuaternion(Quaternion q) {
    roll = atan2f((q.w * q.x + q.y * q.z), 0.5f - (q.x * q.x + q.y * q.y));
    pitch = asinf(-2.0f * (q.x * q.z - q.w * q.y));
    yaw = atan2f((q.x * q.y + q.w * q.z), 0.5f - (q.y * q.y + q.z * q.z));
}

SphericalCoordinate::SphericalCoordinate() {
	r = 0.0f; teta = 0.0f; phi = 0.0f;
}

SphericalCoordinate::SphericalCoordinate(float r, float teta, float phi) {
	this->r = r; this->teta = teta; this->phi = phi;
}

float SphericalCoordinate::calculateRFromCartesian(float x, float y, float z) {
	return calculateMagnitude3D(x, y , z);
}

float SphericalCoordinate::calculateTetaFromCartesian(float x, float y, float z) {
	return acosf(z / calculateMagnitude3D(x, y, z));
}

float SphericalCoordinate::calculatePhiFromCartesian(float x, float y, float z) {
	return atan2f(y, x);
}
