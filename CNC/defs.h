/*
 * File: defs.h
 * Author : Justin Doss
 *
 * This file contains all the definitions (configs) for the project.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef DEFS_H_
#define DEFS_H_

#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum axis { X_AXIS, Y_AXIS, Z_AXIS } _axis;
typedef enum system_states { SYS_START, SYS_WAITING, SYS_RUNNING, SYS_FAILURE } _system_state;

void waitingLoop(unsigned char delay);
void systemFailure(char* msg);
_system_state getSysState();
void setSystemRunning(unsigned char isSerial);

#ifdef __cplusplus
}
#endif

// in milliseconds
#define TICK_PERIOD_A 1
//#define TICK_PERIOD_B 2000
#define TIMER_A_ISR tickTasks
//#define TIMER_B_ISR updateDisplay

#include "pin_io.h"
#include "timer.h"
#include "usart1284.h"
#include "task.h"

#define MOTOR_DISABLE PA_7

#define X_MOTOR_DIR PC_0
#define X_MOTOR_STEP PC_1
#define X_MOTOR_INVERT 0

#define Y_MOTOR_DIR PC_2
#define Y_MOTOR_STEP PC_3
#define Y_MOTOR_INVERT 1

#define Z_MOTOR_DIR PC_4
#define Z_MOTOR_STEP PC_5
#define Z_MOTOR_INVERT 0

#define X_ENDSTOP PA_2
#define Y_ENDSTOP PA_3
#define Z_ENDSTOP PA_4

#define X_STEPS_MM 100
#define Y_STEPS_MM 100
#define Z_STEPS_MM 400

#define X_HOME_OFFSET 36
#define Y_HOME_OFFSET 7
#define Z_HOME_OFFSET 0.8f

#define X_MAX_POS 220
#define Y_MAX_POS 220
#define Z_MAX_POS 240

#endif //DEFS_H_