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
#include "command.h"
#include "commands.h"
#include "mov_controller.h"
#include <string.h>
#include <math.h>

const char* OK = "ok\n";
const char* BUSY = "busy\n";
const char* DONE = "done\n";

_task com_scheduler;

volatile unsigned char relPositioning = 0;
Command currentBlocking;

void scheduleCom(_task *task)
{
	if (currentBlocking.isReady() && currentBlocking.getCommandType() != EMPTY)
    {
        currentBlocking.start();
    }
	else if (currentBlocking.hasStarted() && currentBlocking.isDone())
	{
		while (!USART_hasTransmittedLine(0));
		USART_sendLine(DONE, 0);
		currentBlocking.setCommandType(EMPTY);
	}
}

void initComScheduler()
{
    com_scheduler.state = 0;
    com_scheduler.period = 1;
    com_scheduler.elapsedTime = 0;
    com_scheduler.TickFct = &scheduleCom;
    addTask(&com_scheduler);
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
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                currentBlocking.setCommandType(MOVE);
                _scheduledMove* mov = currentBlocking.getMoveData();
                mov->x_steps = 0;
                mov->y_steps = 0;
                mov->z_steps = 0;

                if (!relPositioning)
                {
                    if (cmd.hasX())
                    {
                        mov->x_steps = (long)((cmd.getX() - MovController::getMovController(X_AXIS)->getPosition()) * MovController::getStepsPerMM(X_AXIS));
                        if (cmd.getX() < 0)
                        {
                            systemFailure("Neg position in abs positioning mode!");
                        }
                    }
                    if (cmd.hasY())
                    {
                        mov->y_steps = (long)((cmd.getY() - MovController::getMovController(Y_AXIS)->getPosition()) * MovController::getStepsPerMM(Y_AXIS));
                        if (cmd.getY() < 0)
                        {
                            systemFailure("Neg position in abs positioning mode!");
                        }
                    }
                    if (cmd.hasZ())
                    {
                        mov->z_steps = (long)((cmd.getZ() - MovController::getMovController(Z_AXIS)->getPosition()) * MovController::getStepsPerMM(Z_AXIS));
                        if (cmd.getZ() < 0)
                        {
                            systemFailure("Neg position in abs positioning mode!");
                        }
                    }
                }
                else
                {
                    if (cmd.hasX())
                        mov->x_steps = (long)(cmd.getX() * MovController::getStepsPerMM(X_AXIS));
                    if (cmd.hasY())
                        mov->y_steps = (long)(cmd.getY() * MovController::getStepsPerMM(Y_AXIS));
                    if (cmd.hasZ())
                        mov->z_steps = (long)(cmd.getZ() * MovController::getStepsPerMM(Z_AXIS));
                }

                if (mov->x_steps >= mov->y_steps && mov->x_steps > mov->z_steps)
                {
                    mov->x_peroid = cmd.getG() ? 1 : 5;
                    if (mov->y_steps != 0)
                        mov->y_peroid = (unsigned short)floorf((mov->x_peroid * fabs(mov->x_steps / (float)mov->y_steps)) + 0.5f);
                    if (mov->z_steps != 0)
                        mov->z_peroid = (unsigned short)floorf((mov->x_peroid * fabs(mov->x_steps / (float)mov->z_steps)) + 0.5f);
                }
                else if (mov->y_steps >= mov->z_steps)
                {
                    mov->y_peroid = cmd.getG() ? 1 : 5;
                    if (mov->x_steps != 0)
                        mov->x_peroid = (unsigned short)floorf((mov->y_peroid * fabs(mov->y_steps / (float)mov->x_steps)) + 0.5f);
                    if (mov->z_steps != 0)
                        mov->z_peroid = (unsigned short)floorf((mov->y_peroid * fabs(mov->y_steps / (float)mov->z_steps)) + 0.5f);
                }
                else
                {
                    mov->z_peroid = cmd.getG() ? 1 : 5;
                    if (mov->x_steps != 0)
                        mov->x_peroid = (unsigned short)floorf((mov->z_peroid * fabs(mov->z_steps / (float)mov->x_steps)) + 0.5f);
                    if (mov->y_steps != 0)
                        mov->y_peroid = (unsigned short)floorf((mov->z_peroid * fabs(mov->z_steps / (float)mov->y_steps)) + 0.5f);
                }
                currentBlocking.setReady();
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(OK, 0);
            }
            else
            {
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(BUSY, 0);
            }
            break;
        case 28: // go home command
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                currentBlocking.setCommandType(MOVE);
                if (!cmd.hasX() && !cmd.hasY() && !cmd.hasZ())
                    MovController::goHomeAll();
                if (cmd.hasX())
                    MovController::getMovController(X_AXIS)->goHome();
                if (cmd.hasY())
                    MovController::getMovController(Y_AXIS)->goHome();
                if (cmd.hasZ())
                    MovController::getMovController(Z_AXIS)->goHome();
                currentBlocking.setReady();
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(OK, 0);
            }
            else
            {
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(BUSY, 0);
            }
            break;
        case 50: // grab object
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                currentBlocking.setCommandType(GRAB);
                currentBlocking.setReady();
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(OK, 0);
            }
            else
            {
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(BUSY, 0);
            }
            break;
        case 52: // drop object
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                currentBlocking.setCommandType(DROP);
                currentBlocking.setReady();
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(OK, 0);
            }
            else
            {
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(BUSY, 0);
            }
            break;
        case 90: // set abs positioning; no movement takes place
            relPositioning = 0;
            while (!USART_hasTransmittedLine(0));
            USART_sendLine(OK, 0);
            break;
        case 91: // set rel positioning; no movement takes place
            relPositioning = 1;
            while (!USART_hasTransmittedLine(0));
            USART_sendLine(OK, 0);
            break;
        case 92: // set position; no movement takes place
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                currentBlocking.setCommandType(SET_POS);
                _pos* pos = currentBlocking.getPosData();

                pos->x_pos = MovController::getMovController(X_AXIS)->getPosition();
                pos->y_pos = MovController::getMovController(Y_AXIS)->getPosition();
                pos->z_pos = MovController::getMovController(Z_AXIS)->getPosition();

                if (cmd.hasX())
                    pos->x_pos = cmd.getX();
                if (cmd.hasY())
                    pos->y_pos = cmd.getY();
                if (cmd.hasZ())
                    pos->z_pos = cmd.getZ();

                currentBlocking.setReady();
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(OK, 0);
            }
            else
            {
                while (!USART_hasTransmittedLine(0));
                USART_sendLine(BUSY, 0);
            }
            break;
        case 240: // emergency stop
        {
            systemFailure("Received E-STOP command!");
            break;
        }
        case 250: // cubby status
        {
            while (!USART_hasTransmittedLine(0));
            if (false) // TODO check if objected detected by LIDAR
            {
                USART_sendLine("full\n", 0);
            }
            else
            {
                USART_sendLine("empty\n", 0);
            }
            break;
        }
        case 254: // machine status
        {
            while (!USART_hasTransmittedLine(0));
            if (currentBlocking.isDone() || currentBlocking.getCommandType() == EMPTY)
            {
                USART_sendLine("ready\n", 0);
            }
            else
            {
                USART_sendLine(BUSY, 0);
            }
            break;
        }
        case 255: // machine info
        {
            while (!USART_hasTransmittedLine(0));
            char* rsp = "ok FIRMWARE_NAME:avrAWDS FIRMWARE_VERSION:0.0.1 MACHINE_TYPE:AWDS\n";
            USART_sendLine(rsp, 0);
            break;
        }
        default:
            invalidCommandFailure();
            break;
    }
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