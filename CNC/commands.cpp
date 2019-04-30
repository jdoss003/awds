/*
 * File: commands.cpp
 * Author : Justin Doss
 *
 * This file processes a GCODE object and implements the functionality for each command.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"
#include "commands.h"
#include "mov_controller.h"
#include <string.h>
#include <math.h>

_task mov_scheduler;

enum ScheduleState { SCHED_WAIT, SCHED_READY, SCHED_DONE};

volatile unsigned char relPositioning = 0;

struct ScheduledMov
{
    signed short x_steps;
    unsigned short x_peroid;
    signed short y_steps;
    unsigned short y_peroid;
    signed short z_steps;
    unsigned short z_peroid;
} mov;

void scheduleMove(_task *task)
{
    if (!MovController::areAnyMotorsMoving() && task->state == SCHED_READY)
    {
        if (mov.x_steps != 0)
            MovController::getMovController(X_AXIS)->moveTo(mov.x_steps, mov.x_peroid);
        if (mov.y_steps != 0)
            MovController::getMovController(Y_AXIS)->moveTo(mov.y_steps, mov.y_peroid);
        if (mov.z_steps != 0)
            MovController::getMovController(Z_AXIS)->moveTo(mov.z_steps, mov.z_peroid);
        task->state = SCHED_DONE;
    }
}

void initMovScheduler()
{
    mov_scheduler.state = SCHED_WAIT;
    mov_scheduler.period = 1;
    mov_scheduler.elapsedTime = 0;
    mov_scheduler.TickFct = &scheduleMove;
    addTask(&mov_scheduler);
}

void invalidCommandFailure()
{
    systemFailure("Invalid command...");
}

void do_g_command(GCode cmd)
{
    switch (cmd.getG())
    {
        case 0: // move command
        case 1:
            while (mov_scheduler.state == SCHED_READY || MovController::areAnyMotorsMoving()) { waitingLoop(1); }

            mov.x_steps = 0;
            mov.y_steps = 0;
            mov.z_steps = 0;

            if (!relPositioning)
            {
                if (cmd.hasX())
                    mov.x_steps = ((cmd.getX() - MovController::getMovController(X_AXIS)->getPosition()) * MovController::getStepsPerMM(X_AXIS));
                if (cmd.hasY())
                    mov.y_steps = ((cmd.getY() - MovController::getMovController(Y_AXIS)->getPosition()) * MovController::getStepsPerMM(Y_AXIS));
                if (cmd.hasZ())
                    mov.z_steps = ((cmd.getZ() - MovController::getMovController(Z_AXIS)->getPosition()) * MovController::getStepsPerMM(Z_AXIS));
            }
            else
            {
                if (cmd.hasX())
                    mov.x_steps = (cmd.getX() * MovController::getStepsPerMM(X_AXIS));
                if (cmd.hasY())
                    mov.y_steps = (cmd.getY() * MovController::getStepsPerMM(Y_AXIS));
                if (cmd.hasZ())
                    mov.z_steps = (cmd.getZ() * MovController::getStepsPerMM(Z_AXIS));
            }

            if (mov.x_steps >= mov.y_steps && mov.x_steps > mov.z_steps)
            {
                mov.x_peroid = 1;
                if (mov.y_steps != 0)
                    mov.y_peroid = (unsigned short)floorf((mov.x_peroid * fabs(mov.x_steps / (float)mov.y_steps)) + 0.5f);
                if (mov.z_steps != 0)
                    mov.z_peroid = (unsigned short)floorf((mov.x_peroid * fabs(mov.x_steps / (float)mov.z_steps)) + 0.5f);
            }
            else if (mov.y_steps >= mov.z_steps)
            {
                mov.y_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = (unsigned short)floorf((mov.y_peroid * fabs(mov.y_steps / (float)mov.x_steps)) + 0.5f);
                if (mov.z_steps != 0)
                    mov.z_peroid = (unsigned short)floorf((mov.y_peroid * fabs(mov.y_steps / (float)mov.z_steps)) + 0.5f);
            }
            else
            {
                mov.z_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = (unsigned short)floorf((mov.z_peroid * fabs(mov.z_steps / (float)mov.x_steps)) + 0.5f);
                if (mov.y_steps != 0)
                    mov.y_peroid = (unsigned short)floorf((mov.z_peroid * fabs(mov.z_steps / (float)mov.y_steps)) + 0.5f);
            }

            mov_scheduler.state = SCHED_READY;
            break;

        case 28: // go home command
            while (MovController::areAnyMotorsMoving()) { waitingLoop(1); }
            if (!cmd.hasX() && !cmd.hasY() && !cmd.hasZ())
                MovController::goHomeAll();
            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->goHome();
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->goHome();
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->goHome();
            break;
        case 90: // set abs positioning; no movement takes place
            relPositioning = 0;
            break;
        case 91: // set rel positioning; no movement takes place
            relPositioning = 1;
            break;
        case 92: // set position; no movement takes place
            while (MovController::areAnyMotorsMoving()) { waitingLoop(1); }
            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->setPosition(cmd.getX());
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->setPosition(cmd.getY());
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->setPosition(cmd.getZ());
            break;
        case 115:
        {
            char* rsp = "ok FIRMWARE_NAME:avrAWDS MACHINE_TYPE:AWDS \n";
            USART_sendLine(rsp, 0);
            //while (!USART_hasTransmittedLine(0));
            break;
        }
        default:
            invalidCommandFailure();
            break;
    }
	
	/*if (cmd.fromSerial())
	{
		USART_sendLine("ok\n", 0);
		while (!USART_hasTransmittedLine(0));
	}*/
}

void proccess_command(GCode cmd)
{
    if (cmd.hasFormatError())
        return;//invalidCommandFailure();

    if (cmd.hasG())
        do_g_command(cmd);
    else
        invalidCommandFailure();
}