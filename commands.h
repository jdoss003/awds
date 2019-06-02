/*
 * File: commands.h
 * Author : Justin Doss
 *
 * This file processes a GCODE object and implements the functionality for each command.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "gcode.h"

typedef struct ScheduledMove
{
    signed long x_steps;
    unsigned short x_peroid;
    signed long y_steps;
    unsigned short y_peroid;
    signed long z_steps;
    unsigned short z_peroid;
} _scheduledMove;

typedef struct pos
{
    float x_pos;
    float y_pos;
    float z_pos;
} _pos;

void initComScheduler();
void proccess_command(GCode);

#endif //COMMANDS_H
