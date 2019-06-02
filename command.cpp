

#include "command.h"
#include "mov_controller.h"

Command::Command()
{
    this->ready = false;
    this->type = EMPTY;
}

Command::~Command() {}

void Command::start()
{
    switch (this->type)
    {
        case MOVE:
        {
            if (this->mov.x_steps != 0)
            {
                MovController::getMovController(X_AXIS)->moveTo(this->mov.x_steps, this->mov.x_peroid);
            }
            if (this->mov.y_steps != 0)
            {
                MovController::getMovController(Y_AXIS)->moveTo(this->mov.y_steps, this->mov.y_peroid);
            }
            if (this->mov.z_steps != 0)
            {
                MovController::getMovController(Z_AXIS)->moveTo(this->mov.z_steps, this->mov.z_peroid);
            }
            break;
        }
        case SET_POS:
        {
            MovController::getMovController(X_AXIS)->setPosition(this->pos.x_pos);
            MovController::getMovController(Y_AXIS)->setPosition(this->pos.y_pos);
            MovController::getMovController(Z_AXIS)->setPosition(this->pos.z_pos);
            break;
        }
        case GRAB:
        {
            break; // TODO
        }
        case DROP:
        {
            break; // TODO
        }
        default:
            break;
    }
    this->ready = false;
    this->started = true;
}

bool Command::hasStarted()
{
    return this->started;
}

bool Command::isDone()
{
    switch (this->type)
    {
        case MOVE:
            return !MovController::areAnyMotorsMoving();
        case GRAB:
            return false; // TODO
        case DROP:
            return false; // TODO
        case SET_POS:
            return true;
        default:
            return false;
    }
}

bool Command::isReady()
{
    return this->ready;
}

void Command::setReady()
{
    this->ready = true;
}

_commandType Command::getCommandType()
{
    return this->type;
}

void Command::setCommandType(_commandType type)
{
    this->type = type;
    this->started = false;
}

_scheduledMove* Command::getMoveData()
{
    return &this->mov;
}

_pos* Command::getPosData()
{
    return &this->pos;
}
