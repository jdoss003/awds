
#include "defs.h"

#define MAX_ANGLE 50

typedef enum grap_state { OPEN, CLOSE } grapState;
_task grap_task;
volatile unsigned char anglePos = 0;

void grap_init()
{
    INITPIN(PB_6, OUTPUT, LOW);

    grap_task.state = CLOSE;
    grap_task.period = 8;
}

void grap_open()
{
	grap_task.state = OPEN;
}

void grap_close()
{
	grap_task.state = CLOSE;
}

char grap_isOpen()
{
	return anglePos == MAX_ANGLE;
}

char grap_isClosed()
{
	return anglePos == 0;
}

grapState grap_doThing()
{
	if(grap_task.state == CLOSE)
	{
		SETPIN(PB_6, HIGH);
		_delay_ms(0.4);
		SETPIN(PB_6, LOW);
		if (anglePos > 0) --anglePos;
	}
	else
	{
		SETPIN(PB_6, HIGH);
		_delay_ms(1.6);
		SETPIN(PB_6, LOW);
		if (anglePos < MAX_ANGLE) ++anglePos;
	}

    return grap_task.state;
}

void grap_tick()
{
	if (getSysState() == 3) { return; }
    // Task is ready to tick
    if (grap_task.elapsedTime >= grap_task.period)
    {
        // Setting next state for task
        grap_doThing();
        // Reset the elapsed time for next tick.
        grap_task.elapsedTime = 0;
    }
    grap_task.elapsedTime += 1;
}