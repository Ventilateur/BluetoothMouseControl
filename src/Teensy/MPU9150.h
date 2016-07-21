// MPU9150.h

#ifndef _MPU9150_h
#define _MPU9150_h

#ifndef INCLUDE_IN_INO
    #include <I2Cdev.h>
    #include <i2c_t3.h>
    #include <MPU6050.h>  
#endif // !INCLUDE_IN_INO


// magneto parameters

#define SET		1

#if SET == 1
	// Set 1 - Ramonville
	#define MAX_X				( 26.07)
	#define MIN_X				(-35.45)

	#define MAX_Y				( 53.61)
	#define MIN_Y				( -9.38)

	#define MAX_Z				( -4.69)
	#define MIN_Z				(-64.75)

	#define SCALE_X				(  0.98f)
	#define SCALE_Y				(  0.96f)
	#define SCALE_Z				(  1.07f)

	#define OFFSET_X			( -4.69)
	#define OFFSET_Y			( 22.115)
	#define OFFSET_Z			(-34.72)

#elif SET == 2
	// Set 2 - Artilect
	#define MAX_X				( 26.37)
	#define MIN_X				(-31.05)

	#define MAX_Y				( 67.97)
	#define MIN_Y				( 10.25)

	#define MAX_Z				( 1.46)
	#define MIN_Z				(-53.61)

	#define SCALE_X				(  0.99f)
	#define SCALE_Y				(  0.98f)
	#define SCALE_Z				(  1.03f)

	#define OFFSET_X			( -2.34)
	#define OFFSET_Y			( 39.11)
	#define OFFSET_Z			(-26.07)
#endif 

// gyro parameters
#define G_OFFSET_X			(-0.15f)
#define G_OFFSET_Y			( 0.70f)
#define G_OFFSET_Z			( 1.65f)


// ********************************************************** //
// ******************* DON'T TOUCH THIS ZONE **************** //
// ********************************************************** //

// binding gyro mode
#define GYRO_FCR_250		(250)	// GYRO_RANGE_MODE = 0
#define GYRO_FCR_500		(500)	// GYRO_RANGE_MODE = 1 
#define GYRO_FCR_1000		(1000)  // GYRO_RANGE_MODE = 2
#define GYRO_FCR_2000		(2000)  // GYRO_RANGE_MODE = 3

// binding accel mode
#define ACCEL_FCR_2			(2)		// ACCEL_RANGE_MODE = 0
#define ACCEL_FCR_4			(4)		// ACCEL_RANGE_MODE = 1
#define ACCEL_FCR_8			(8)		// ACCEL_RANGE_MODE = 2
#define ACCEL_FCR_16		(16)	// ACCEL_RANGE_MODE = 3

// sensor constants
#define MAX_16_BITS_VAL		(32768)
#define MAX_13_BITS_VAL		(4096)
#define MAG_FSR				(1200)



class MPU9150 : public MPU6050 {
protected:
	float avg_x, avg_y, avg_z, avg_xyz;
	void calculateMag();
public:
	float max_x, max_y, max_z;
	float min_x, min_y, min_z;
	float offset_x, offset_y, offset_z;
	float scale_x, scale_y, scale_z;
	MPU9150();
	void calibrate();
	void resetMag();
	void resetForCalib();
	float normalize(int16_t val, int16_t range);
	float normalizeMag(int16_t val);
	void getAccelScaled(float *ax, float *ay, float *az);
	void getGyroScaled(float *gx, float *gy, float *gz);
	void getMagnetoScaled(float *mx, float *my, float *mz);
	void correctMag(float *mx, float *my, float *mz);
};


#endif

