/*
 *
 *  Modeled after the GCode parsing code at:
 *  https://github.com/repetier/Repetier-Firmware/blob/master/src/ArduinoAVR/Repetier/gcode.h
 */

#ifndef GCODE_H
#define GCODE_H

#include "defs.h"

class GCode
{
    public:
        ~GCode()
        {
            delete text;
        }

        inline bool hasG()
        {
            return (bool)(params & 0x02);
        }

        inline uint16_t getG()
        {
            return G;
        }

        inline bool hasX()
        {
            return (bool)(params & 0x04);
        }

        inline float getX()
        {
            return X;
        }

        inline bool hasY()
        {
            return (bool)(params & 0x08);
        }

        inline float getY()
        {
            return Y;
        }

        inline bool hasZ()
        {
            return (bool)(params & 0x10);
        }

        inline float getZ()
        {
            return Z;
        }

        inline bool hasNoXYZ()
        {
            return (bool)(params & 0x1C);
        }
		
		inline bool hasN()
		{
			return (bool)(params & 0x400);
		}

		inline int32_t getN()
		{
			return N;
		}

        inline bool hasString()
        {
            return (bool)(params & 0x400);
        }

        inline char* getString()
        {
            return text;
        }

        inline bool hasFormatError()
        {
            return (bool)(params & 0x8000);
        }

        bool parseAscii(char *line, bool fromSerial);

        static void resetLineNum();
        static uint32_t getLineNum();

    private:

        inline void setFormatError()
        {
            params |= 0x8000;
        }

        inline float parseFloatValue(char *s)
        {
            char *endPtr;
            while(*s == ' ') s++;
            float f = strtod(s, &endPtr);
            return s == endPtr ? 0.0 : f;
        }

        inline long parseLongValue(char *s)
        {
            char *endPtr;
            while(*s == ' ') s++;
            long l = strtol(s, &endPtr, 10);
            return s == endPtr ? 0 : l;
        }

        uint16_t params;
        uint16_t G;
        float X;
        float Y;
        float Z;
		int32_t N;

        char *text;

        bool serialCommand;
};

#endif //GCODE_H
