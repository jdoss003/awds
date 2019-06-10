

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int jsonEquals(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start)
    {
        return (strncmp(json + tok->start, s, tok->end - tok->start) == 0);
    }
    return 0;
}

/*
 * The returned string is dynamically allocated. You MUST free it after you are done!
 */
char* getValueFromKey(const char* json, const char* key)
{
    jsmn_parser p;
    jsmntok_t t[64];

    jsmn_init(&p);
    int numTokens = jsmn_parse(&p, json, strlen(json), t, sizeof(t) / sizeof(t[0]));
    if (numTokens < 1)//|| t[0].type != JSMN_OBJECT)
    {
		PORTA = 0x01;
        return 0;
    }

    for (int i = 1; i < numTokens; i++)
    {
        if (jsonEquals(json, &t[i], key) && t[i + 1].type == JSMN_STRING)
        {
            int len = t[i + 1].end - t[i + 1].start;
            char* buff = (char*)malloc(sizeof(char) * (len + 1));
            sprintf(buff, "%.*s", len, json + t[i + 1].start);
			
			PORTA = 0x04;
            return buff;
        }
    }
	PORTA = 0x02;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif JSON_UTILS_H
