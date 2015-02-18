//chmoellinger, 15.05.14

// this file should contain basic configuration stuff
// like setting the device/port/pin for subsystems

#ifndef _LOWLEVEL_CONFIG_H_
#define _LOWLEVEL_CONFIG_H_

#include <Arduino.h>

#define LOWLEVELCONFIG_SBUS_DEVICE Serial3

// Whether the gyro data processing should use interrupts
#define LOWLEVELCONFIG_GYRO_USE_INTERRUPT false


// Define if debugging (and catching assertions) is enabled or not
#define LOWLEVELCONFIG_ENABLE_DEBUGGING true

// Define the serial output device dor debugging information
#define LOWLEVELCONFIG_DEBUG_UART Serial

// Set a let which �starts blinking if an assertion was wrong. 
// To disable this behaviour, undef it.
#define LOWLEVELCONFIG_DEBUG_ASSERT_LED 13

#define OUTPUT_PIN_ESC_FRONT_LEFT 23
#define OUTPUT_PIN_ESC_FRONT_RIGHT 22
#define OUTPUT_PIN_ESC_REAR_LEFT 21
#define OUTPUT_PIN_ESC_REAR_RIGHT 20

#define OUTPUT_PIN_NEOPIXELS_FRONT_LEFT 6
#define OUTPUT_PIN_NEOPIXELS_FRONT_RIGHT 11
#define OUTPUT_PIN_NEOPIXELS_REAR_LEFT 5
#define OUTPUT_PIN_NEOPIXELS_REAR_RIGHT 12

#define GYRO_ONOFF_PIN 14
#define GYRO_SUCCESSFULL_INITIALIZED_STATUS_LED 2
#define ARMED_STATUS_LED_PIN 4 // GREEN LED


#define MAX_ANGLE_SOLL_PITCH 45.0f
#define MAX_ANGLE_SOLL_ROLL 45.0f

#endif 