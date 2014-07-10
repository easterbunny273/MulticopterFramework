//chmoellinger, 15.05.14

// this file should contain basic configuration stuff
// like setting the device/port/pin for subsystems

#ifndef _LOWLEVEL_CONFIG_H_
#define _LOWLEVEL_CONFIG_H_

#include <Arduino.h>

#define LOWLEVELCONFIG_SBUS_DEVICE Serial3

// Whether the gyro data processing should use interrupts
#define LOWLEVELCONFIG_GYRO_USE_INTERRUPT false


#define LOWLEVELCONFIG_ENABLE_DEBUGGING true
#define LOWLEVELCONFIG_DEBUG_DEVICE Serial

#define OUTPUT_PIN_ESC_FRONT_LEFT 23
#define OUTPUT_PIN_ESC_FRONT_RIGHT 22
#define OUTPUT_PIN_ESC_REAR_LEFT 21
#define OUTPUT_PIN_ESC_REAR_RIGHT 20

#define MAX_ANGLE_SOLL_PITCH 45.0f
#define MAX_ANGLE_SOLL_ROLL 45.0f

template<typename T> void debug_print(T message)
{
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_DEVICE.print(message);
#endif
};

template<typename T> void debug_println(T message)
{
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_DEVICE.println(message);
#endif
};

#endif 