/*
 * main.cpp
 * Author : Justin Doss
 *
 * This file contains the main loop and initialization code for the program.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"
#include <string.h>

#include "commands.h"
#include "mov_controller.h"

_system_state system_state;

GCode command;
char* nextLine;

void* operator new(size_t objsize)
{
	return malloc(objsize);
}

void operator delete(void* obj)
{
	free(obj);
}

void waitingLoop(unsigned char delay)
{
    switch (system_state)
    {
        case SYS_WAITING:
        case SYS_RUNNING:
            break;
        default:
            systemFailure("Bad system state");
            break;
    }
}

_system_state getSysState()
{
    return system_state;
}

void setSystemRunning()
{
    system_state = SYS_RUNNING;
}

void systemFailure(char* msg)
{
    cli();
    TimerOffA();
    TimerOffB();

    system_state = SYS_FAILURE;
    MovController::stopAllMoves();

    while (!USART_hasTransmittedLine(0));
    USART_sendLine("Error: ", 0);
    while (!USART_hasTransmittedLine(0));
    USART_sendLine(msg, 0);
    while (!USART_hasTransmittedLine(0));
    USART_sendLine("\n", 0);
    while (1); // stay here until reset
}

void mainLoop()
{
	static unsigned char t = 0;
	
    switch (system_state)
    {
        case SYS_START:
            system_state = SYS_RUNNING;
			break;
        case SYS_RUNNING:
        {
            if (USART_hasLine(0))
            {
                nextLine = USART_getLine(0);
                if (command.parseAscii(nextLine, 1))
                {
                    proccess_command(command);
                }
                else
                {
                    while (!USART_hasTransmittedLine(0));
                    char msg[] = "rs       \n";
                    utoa(GCode::getLineNum(), msg + 3, 10);
                    USART_sendLine(msg, 0);
                }
            }
            break;
        }
        default:
            systemFailure("Bad system state");
            break;
    }
}

int main()
{
	_delay_ms(100);

    system_state = SYS_START;
    INITADC();

    initComScheduler();

    unsigned char i;
    for (i = 0; i < 3; ++i)
    {
        MovController::getMovController((_axis) i)->init((_axis) i);
    }

    INITPIN(MOTOR_DISABLE, OUTPUT, LOW);

    TimerSetA(TICK_PERIOD_A);
    TimerOnA();
    //TimerSetB(TICK_PERIOD_B);
    //TimerOnB();

    USART_init(0);
	USART_clearBuf(0);
    USART_autoReceive(1, 0);
	
	INITPIN(PB_0, OUTPUT, LOW);

    while (1) { mainLoop(); }
}
