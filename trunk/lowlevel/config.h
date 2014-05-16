//chmoellinger, 15.05.14

// this file should contain basic configuration stuff
// like setting the device/port/pin for subsystems

#ifndef _LOWLEVEL_CONFIG_H_
#define _LOWLEVEL_CONFIG_H_

#include <Arduino.h>

#define LOWLEVELCONFIG_SBUS_DEVICE Serial3

#define LOWLEVELCONFIG_DEBUG
#define LOWLEVELCONFIG_DEBUG_DEVICE Serial

template<typename T> void debug_print(T message)
{
#ifdef LOWLEVELCONFIG_DEBUG
	LOWLEVELCONFIG_DEBUG_DEVICE.print(message);
#endif
};

template<typename T> void debug_println(T message)
{
#ifdef LOWLEVELCONFIG_DEBUG
	LOWLEVELCONFIG_DEBUG_DEVICE.println(message);
#endif
};

#endif 