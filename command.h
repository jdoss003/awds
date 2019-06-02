

#ifndef COMMAND_H
#define COMMAND_H

#include "commands.h"

typedef enum commandType { MOVE, SET_POS, GRAB, DROP, EMPTY } _commandType;

class Command
{
    public:
        Command();
        ~Command();
        void start();
        bool hasStarted();
        bool isDone();
        bool isReady();
        void setReady();
        _commandType getCommandType();
        void setCommandType(_commandType);
        _scheduledMove* getMoveData();
        _pos* getPosData();

    private:
        bool ready;
        bool started;
        _commandType type;
        _scheduledMove mov;
        _pos pos;
};

#endif //COMMAND_H
