/*
 * stepper.cpp
 * Author : Justin Doss
 *
 * This file implements an abstraction layer to allow simple control of forward and reverse motor stepping.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "stepper.h"

Stepper x_stepper, y_stepper, z_stepper;
Stepper* steppers[4] = { &x_stepper, &y_stepper, &z_stepper };

Stepper* Stepper::getStepper(_axis axis)
{
    return steppers[axis];
}

void Stepper::init(_axis axis)
{
    this->axis = axis;

    switch (axis)
    {
        case X_AXIS:
            this->reversed = X_MOTOR_INVERT;
            INITPIN(X_MOTOR_DIR, OUTPUT, LOW);
            INITPIN(X_MOTOR_STEP, OUTPUT, LOW);
            INITPIN(X_ENDSTOP, INPUT, HIGH);
            break;
        case Y_AXIS:
            this->reversed = Y_MOTOR_INVERT;
            INITPIN(Y_MOTOR_DIR, OUTPUT, LOW);
            INITPIN(Y_MOTOR_STEP, OUTPUT, LOW);
            INITPIN(Y_ENDSTOP, INPUT, HIGH);
            break;
        case Z_AXIS:
            this->reversed = Z_MOTOR_INVERT;
            INITPIN(Z_MOTOR_DIR, OUTPUT, LOW);
            INITPIN(Z_MOTOR_STEP, OUTPUT, LOW);
            INITPIN(Z_ENDSTOP, INPUT, HIGH);
            break;
        default:
            systemFailure("Init Stepper");
            break;
    }
}

void Stepper::step(unsigned char reverse)
{
    // for DRV8825 stepper drivers
    // min step pulse period is 2 µs
    // min delay for dir change is 650 ns

    switch (this->axis)
    {
        case X_AXIS:
            SETPIN(X_MOTOR_DIR, this->reversed != reverse);
            _delay_us(1);
            SETPIN(X_MOTOR_STEP, HIGH);
            _delay_us(2);
            SETPIN(X_MOTOR_STEP, LOW);
            break;
        case Y_AXIS:
            SETPIN(Y_MOTOR_DIR, this->reversed != reverse);
            _delay_us(1);
            SETPIN(Y_MOTOR_STEP, HIGH);
            _delay_us(2);
            SETPIN(Y_MOTOR_STEP, LOW);
            break;
        case Z_AXIS:
            SETPIN(Z_MOTOR_DIR, this->reversed != reverse);
            _delay_us(1);
            SETPIN(Z_MOTOR_STEP, HIGH);
            _delay_us(2);
            SETPIN(Z_MOTOR_STEP, LOW);
            break;
        default:
            systemFailure("Stepper Out");
            break;
    }
}