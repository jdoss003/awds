/*
 * ESP8266 Module.cpp
 *
 * Created: 5/15/2019 4:20:29 PM
 * Author : Derek
 */ 

#include <avr/io.h>			/* Include avr IO library */
#include <avr/interrupt.h>	/* Include avr interrupt library */
#include <util/delay.h>		/* Include delay library */
#include <string.h>			/* Include string library */
#include <stdio.h>			/* Include standard IO library */
#include <stdlib.h>			/* Include standard library */
#include "usart1284.h"		/* Include USART header file */

//USART Values
#define ESP8266_Baud 115200
#define USART_Channel 0

/*ESP8266 AT Command Values*/
//Connection Mode
#define SINGLE				0
#define MULTIPLE			1

// Application Mode
#define NORMAL				0
#define TRANSPERANT			1

// Application Mode
#define STATION				1
#define ACCESSPOINT			2
#define BOTH_STATION_AND_ACCESPOINT	3

// Define Required fields shown below
#define DOMAIN				"api.thingspeak.com"
#define PORT				"80"
#define API_WRITE_KEY		"ASW3O7IL6AO9U141"
#define CHANNEL_ID			"780705"
#define SSID				"TestWIFI"
#define PASSWORD			"Sloppyd69"

#define DEFAULT_BUFFER_SIZE		160
#define DEFAULT_TIMEOUT			10000

enum ESP8266_RESPONSE_STATUS{
	ESP8266_RESPONSE_WAITING,
	ESP8266_RESPONSE_FINISHED,
	ESP8266_RESPONSE_TIMEOUT,
	ESP8266_RESPONSE_BUFFER_FULL,
	ESP8266_RESPONSE_STARTING,
	ESP8266_RESPONSE_ERROR
};

enum ESP8266_CONNECT_STATUS {
	ESP8266_CONNECTED_TO_AP,
	ESP8266_CREATED_TRANSMISSION,
	ESP8266_TRANSMISSION_DISCONNECTED,
	ESP8266_NOT_CONNECTED_TO_AP,
	ESP8266_CONNECT_UNKNOWN_ERROR
};

enum ESP8266_JOINAP_STATUS {
	ESP8266_WIFI_CONNECTED,
	ESP8266_CONNECTION_TIMEOUT,
	ESP8266_WRONG_PASSWORD,
	ESP8266_NOT_FOUND_TARGET_AP,
	ESP8266_CONNECTION_FAILED,
	ESP8266_JOIN_UNKNOWN_ERROR
};

char RESPONSE[DEFAULT_BUFFER_SIZE];
char json[0] = '\0';

bool Default_ReadResponse(char* expectedResponse)
{
	while(!USART_hasLine(USART_Channel)){}
	char* crlf = USART_getLine(USART_Channel);
	while(!USART_hasLine(USART_Channel)){}
	char* res = USART_getLine(USART_Channel);
	strcat(crlf, res);
	if(strstr(expectedResponse, crlf))
	{
		//TODO save response in global variable
		return true;
	}
	return false;
}

bool JoinAP_ReadResponse(char* expectedResponse)
{
	while(!USART_hasLine(USART_Channel)){}
	char* crlf = USART_getLine(USART_Channel);
	while(!USART_hasLine(USART_Channel)){}
	char* res = USART_getLine(USART_Channel);
	while(!USART_hasLine(USART_Channel)){}
	char* ip = USART_getLine(USART_Channel);
	while(!USART_hasLine(USART_Channel)){}
	char* ok = USART_getLine(USART_Channel);
	strcat(crlf, res);
	strcat(crlf, ip);
	strcat(crlf, ok);
	if(strstr(expectedResponse, crlf))
	{
		//TODO save clrlf in global variables
		
		return true;
	}
	return false;
}

bool JSON_ReadResponse(char* expectedResponse)
{
	while(!USART_hasLine(USART_Channel)){}
	char* crlf = USART_getLine(USART_Channel);
	while(!USART_hasLine(USART_Channel)){}
	char* res = USART_getLine(USART_Channel);
	strcat(crlf, res);
	if(strstr(expectedResponse, crlf))
	{
		//TODO save response in global variable
		
		return true;
	}
	return false;
}

void SendAT(char* ATCommand)
{
	USART_sendLine(ATCommand, USART_Channel);		/* Send AT command to ESP8266 */
	while(!USART_hasTransmittedLine(USART_Channel)){}
	USART_sendLine("\r\n", USART_Channel);
	while(!USART_hasTransmittedLine(USART_Channel)){}
}

bool ESP8266_Begin()
{
	for (uint8_t i=0;i<5;i++)
	{
		SendAT("AT");
		if(Default_ReadResponse("\r\nOK\r\n"))
		{
			return true;
		}
	}
	return false;
}

bool ESP8266_WIFIMode(uint8_t _mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CWMODE=%d", _mode);
	_atCommand[19] = 0;
	SendAT(_atCommand);
	return Default_ReadResponse("\r\nOK\r\n");
}

bool ESP8266_ConnectionMode(uint8_t Mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPMUX=%d", Mode);
	_atCommand[19] = 0;
	SendAT(_atCommand);
	return Default_ReadResponse("\r\nOK\r\n");
}

bool ESP8266_ApplicationMode(uint8_t Mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPMODE=%d", Mode);
	_atCommand[19] = 0;
	SendAT(_atCommand);
	return Default_ReadResponse("\r\nOK\r\n");
}

uint8_t ESP8266_JoinAccessPoint(char* _SSID, char* _PASSWORD)
{
	char _atCommand[60];
	memset(_atCommand, 0, 60);
	sprintf(_atCommand, "AT+CWJAP=\"%s\",\"%s\"", _SSID, _PASSWORD);
	_atCommand[59] = 0;
	SendAT(_atCommand);
	if(JoinAP_ReadResponse("\r\nWIFI CONNECTED\r\n"))
		return ESP8266_WIFI_CONNECTED;
	else{
		if(strstr(RESPONSE, "+CWJAP:1"))
			return ESP8266_CONNECTION_TIMEOUT;
		else if(strstr(RESPONSE, "+CWJAP:2"))
			return ESP8266_WRONG_PASSWORD;
		else if(strstr(RESPONSE, "+CWJAP:3"))
			return ESP8266_NOT_FOUND_TARGET_AP;
		else if(strstr(RESPONSE, "+CWJAP:4"))
			return ESP8266_CONNECTION_FAILED;
		else
			return ESP8266_JOIN_UNKNOWN_ERROR;
	}
}

uint8_t ESP8266_connected()
{
	SendAT("AT+CIPSTATUS");
	Default_ReadResponse("\r\nOK\r\n");
	if(strstr(RESPONSE, "STATUS:2"))
		return ESP8266_CONNECTED_TO_AP;
	else if(strstr(RESPONSE, "STATUS:3"))
		return ESP8266_CREATED_TRANSMISSION;
	else if(strstr(RESPONSE, "STATUS:4"))
		return ESP8266_TRANSMISSION_DISCONNECTED;
	else if(strstr(RESPONSE, "STATUS:5"))
		return ESP8266_NOT_CONNECTED_TO_AP;
	else
		return ESP8266_CONNECT_UNKNOWN_ERROR;
}

uint8_t ESP8266_Start(uint8_t _ConnectionNumber, char* Domain, char* Port)
{
	bool _startResponse;
	char _atCommand[60];
	memset(_atCommand, 0, 60);
	_atCommand[59] = 0;

	SendAT("AT+CIPMUX?");
	if(Default_ReadResponse("CIPMUX:0"))
		sprintf(_atCommand, "AT+CIPSTART=\"TCP\",\"%s\",%s", Domain, Port);

	SendAT(_atCommand);
	if(!Default_ReadResponse("CONNECT\r\n");)
	{
		return ESP8266_RESPONSE_ERROR;
	}
	return ESP8266_RESPONSE_FINISHED;
}

uint8_t ESP8266_Send(char* Data)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPSEND=%d", (strlen(Data)+2));
	_atCommand[19] = 0;
	SendAT(_atCommand);
	if(!Default_ReadResponse("\r\nOK\r\n>"))
	{
		SendAT(Data);
		if(!JSON_ReadResponse("\r\nSEND OK\r\n"))
		{
			return ESP8266_RESPONSE_ERROR;
		}
		return ESP8266_RESPONSE_FINISHED;
	}
	return ESP8266_RESPONSE_ERROR;
}

int main(void)
{
	char _buffer[150];
	uint8_t Connect_Status;
	USART_initBaud(USART_Channel, ESP8266_Baud);/* Initiate USART with 115200 baud rate */
	
	sei();									/* Start global interrupt */

	while(!ESP8266_Begin())
	{
		ESP8266_WIFIMode(BOTH_STATION_AND_ACCESPOINT);/* 3 = Both (AP and STA) */
		ESP8266_ConnectionMode(SINGLE);		/* 0 = Single; 1 = Multi */
		ESP8266_ApplicationMode(NORMAL);	/* 0 = Normal Mode; 1 = Transperant Mode */
		if(ESP8266_connected() == ESP8266_NOT_CONNECTED_TO_AP)
			ESP8266_JoinAccessPoint(SSID, PASSWORD);
		ESP8266_Start(0, DOMAIN, PORT);
	}
	while(1)
	{
		Connect_Status = ESP8266_connected();
		if(Connect_Status == ESP8266_NOT_CONNECTED_TO_AP)
			ESP8266_JoinAccessPoint(SSID, PASSWORD);
		if(Connect_Status == ESP8266_TRANSMISSION_DISCONNECTED)
			ESP8266_Start(0, DOMAIN, PORT);
			
		memset(_buffer, 0, 150);
		sprintf(_buffer, "GET /channels/%s/fields/1.json", CHANNEL_ID);
		ESP8266_Send(_buffer);
		_delay_ms(600);	
	}
	cli();									/* End Global Interrupt */
}


