/*
 *
 *  Modeled after the GCode parsing code at:
 *  https://github.com/repetier/Repetier-Firmware/blob/master/src/ArduinoAVR/Repetier/gcode.cpp
 */

#include "gcode.h"
#include "commands.h"
#include "string.h"

volatile uint32_t curLineNum = 0;

void GCode::resetLineNum()
{
    curLineNum = 0;
}

uint32_t GCode::getLineNum()
{
    return curLineNum;
}

bool GCode::parseAscii(char *line, bool fromSerial)
{
    params = 0;
	G = 256;
    serialCommand = fromSerial;
    uint8_t checksum = 0;

    char *pos = line;
    char c;

    while ((c = *(pos++)) && !hasFormatError())
    {
        if(c == '(' || c == '%' || c == ';' || c == '\n' || c == '\r')
            break;

        switch(c)
        {
            case 'G':
            case 'g':
            {
                G = parseLongValue(pos) & 0xFFFF;
                params |= 0x02;
                if(G > 255)
                    setFormatError();
                break;
            }
			case 'N':
			case 'n':
			{
				N = parseLongValue(pos);
				params |= 0x400;
				if (curLineNum + 1 == N || curLineNum == N)
				    curLineNum = N;
                else
                    setFormatError();
				break;
			}
            case 'X':
            case 'x':
            {
                X = parseFloatValue(pos);
                params |= 0x04;
                break;
            }
            case 'Y':
            case 'y':
            {
                Y = parseFloatValue(pos);
                params |= 0x08;
                break;
            }
            case 'Z':
            case 'z':
            {
                Z = parseFloatValue(pos);
                params |= 0x10;
                break;
            }
            case '*':
            {
                if (!hasN())
                {
                    setFormatError();
                }
                else
                {
                    checksum = parseLongValue(pos);
                    uint8_t cs = 0;
                    while (line != pos - 1)
                        cs ^= *(line++);

                    if (checksum != cs)
                        setFormatError();
                }
                break;
            }
            default:
                break;
        }
    }

    if (hasN() && checksum == 0)
        setFormatError();

    return !hasFormatError() && (params & 0x0003) && (G < 256);
}

