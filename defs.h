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
//#define F_CPU 20000000UL

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum axis { X_AXIS, Y_AXIS, Z_AXIS } _axis;
typedef enum system_states { SYS_START, SYS_WAITING, SYS_RUNNING, SYS_FAILURE } _system_state;

void systemFailure(char* msg);
_system_state getSysState();

#ifdef __cplusplus
}
#endif

// in milliseconds
#define TICK_PERIOD_A 1
#define TICK_PERIOD_B 1
#define TIMER_A_ISR tickTasks
#define TIMER_B_ISR grap_tick

#include "pin_io.h"
#include "timer.h"
#include "usart1284.h"
#include "task.h"
#include "grappler.h"

#define MOTOR_DISABLE PD_7

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

#define X_HOME_OFFSET 1
#define Y_HOME_OFFSET 1
#define Z_HOME_OFFSET 1

#define X_MAX_POS 925
#define Y_MAX_POS 670
#define Z_MAX_POS 145

#endif //DEFS_H_