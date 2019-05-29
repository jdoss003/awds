/*
 * ESP8266 Module.cpp
 *
 * Created: 5/15/2019 4:20:29 PM
 * Author : Derek
 *
 * To understand ESP8266 module please find it's data sheet and AT command sheet
 */ 

#define F_CPU 20000000UL

#include <avr/io.h>			/* Include avr IO library */
#include <avr/interrupt.h>	/* Include avr interrupt library */
#include <util/delay.h>		/* Include delay library */
#include <string.h>			/* Include string library */
#include <stdio.h>			/* Include standard IO library */
#include <stdlib.h>			/* Include standard library */
#include "usart1284.h"		/* Include USART header file */
#include "io.h"				/* Include LCD functions */

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
#define PASSWORD			"AWDScs179"

#define DEFAULT_BUFFER_SIZE		160
#define DEFAULT_TIMEOUT			50000

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

//Receive line sent from WIFI that was automatically received through USART.
bool Default_ReadResponse(char* expectedResponse)
{
	//Receive echo
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive OK
	while(!USART_hasLine(USART_Channel)){}
	char* ok = USART_getLine(USART_Channel);;
	
	//Check if received expected response
	if(strstr(ok, expectedResponse))
	{
		return true;
	}
	return false;
}

//Receive line sent from WIFI that was automatically received through USART. Catches echo.
char* Connected_ReadResponse(char* expectedResponse)
{
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive STATUS:#
	while(!USART_hasLine(USART_Channel)){}
	char* status = USART_getLine(USART_Channel);
	
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive OK
	while(!USART_hasLine(USART_Channel)){}
	char* ok = USART_getLine(USART_Channel);
	
	//Check if received expected response
	if(strstr(ok, expectedResponse))
	{
		return status;
	}
	char* failed = "failed";
	return failed;
}

//Receive line sent from WIFI that was automatically received through USART. Catches echo.
char* JoinAP_ReadResponse(char* connected, char* gotIP)
{
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive WIFI CONNECTED
	while(!USART_hasLine(USART_Channel)){}
	char* status = USART_getLine(USART_Channel);
	
	//Receive WIFI GOT IP
	while(!USART_hasLine(USART_Channel)){}
	char* ip = USART_getLine(USART_Channel);
	
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Check if received expected response
	if(strstr(status, connected) && strstr(ip, gotIP))
	{
		return status;
	}
	return status;
}

//Receive line sent from WIFI that was automatically received through USART. Catches echo.
bool Start_ReadResponse(char* expectedResponse)
{
	//Receive Echo
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive CONNECT
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive OK
	while(!USART_hasLine(USART_Channel)){}
	char* ok = USART_getLine(USART_Channel);
	
	//Check if received expected response
	if(strstr(ok, expectedResponse))
	{
		return true;
	}
	return false;
}

char* JSON_ReadResponse(char* expectedResponse)
{
	//Receive received notice
	while(!USART_hasLine(USART_Channel)){}
	char* rec = USART_getLine(USART_Channel);
	unsigned char* dispstr1 = (unsigned char*) rec;
	PORTA =0x01;
	
	//Receive SEND OK
	while(!USART_hasLine(USART_Channel)){}
	char* sok = USART_getLine(USART_Channel);
	unsigned char* dispstr2 = (unsigned char*) sok;
	PORTA =0x02;
	
	//Receive JSON
	while(!USART_hasLine(USART_Channel)){}
	char* json1 = USART_getLine(USART_Channel);
	unsigned char* dispstr3 = (unsigned char*) json1;
	PORTA = 0x04;
	
	//Receive JSON
	while(!USART_hasLine(USART_Channel)){}
	char* json2 = USART_getLine(USART_Channel);
	unsigned char* dispstr4 = (unsigned char*) json2;
	PORTA = 0x04;
	
	//Check if received expected response
	LCD_DisplayString(1, dispstr4);
	return json2;
}

//Send AT command to ESP8622 WIFI Module.
void SendAT(char* atCommand)
{	PORTA = 0x01;
	USART_clearBuf(USART_Channel);
	USART_sendLine(atCommand, USART_Channel);		/* Send AT command to ESP8266 */
	while(!USART_hasTransmittedLine(USART_Channel)){}
	PORTA= 0x00;
}

//Send AT command and receive OK. Checks if operational.
bool ESP8266_Begin()
{
	char* command = "AT\r\n";
	char* response = "OK";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command CWMODE. Sets up WIFI mode. 
bool ESP8266_WIFIMode()
{
	char* command = "AT+CWMODE_CUR=3\r\n";//Set up WIFI module as both Access point and STA
	char* response = "OK";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command AT+CIPMUX. Sets up Enable for multiple connections. In this case sending 0 makes it a single connection.
bool ESP8266_ConnectionMode()
{
	char* command = "AT+CIPMUX=0\r\n";//Set up WIFI module for singular connection
	char* response = "OK";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command AT+CIPMODE. Sets up transfer mode. In this case sending 0 makes it a a normal mode where the WIF module must reconnect constantly to TCP.
bool ESP8266_ApplicationMode()
{
	char* command = "AT+CIPMODE=0\r\n";//Set up WIFI module for singular connection
	char* response = "OK";
	SendAT(command);
	return Default_ReadResponse(response);
}

uint8_t ESP8266_connected()
{
	char* command = "AT+CIPSTATUS\r\n";//Set up WIFI module for singular connection
	char* response = "OK";
	SendAT(command);
	char* status = Connected_ReadResponse(response);
	
	if(strstr(status, "STATUS:2"))
		return ESP8266_CONNECTED_TO_AP;
	else if(strstr(status, "STATUS:3"))
		return ESP8266_CREATED_TRANSMISSION;
	else if(strstr(status, "STATUS:4"))
		return ESP8266_TRANSMISSION_DISCONNECTED;
	else if(strstr(status, "STATUS:5"))
		return ESP8266_NOT_CONNECTED_TO_AP;
	else
		return ESP8266_CONNECT_UNKNOWN_ERROR;
}

uint8_t ESP8266_JoinAccessPoint()
{
	char* command = "AT+CWJAP_CUR=\"TestWIFI\",\"AWDScs179\"\r\n";
	char* response1 = "CONNECTED";
	char* response2 = "GOT IP";
	SendAT(command);
	char* status = JoinAP_ReadResponse(response1, response2);
	
	if(strstr(status, "CONNECTED"))
		return ESP8266_WIFI_CONNECTED;
	else
	{
		if(strstr(status, "1"))
			return ESP8266_CONNECTION_TIMEOUT;
		else if(strstr(status, "2"))
			return ESP8266_WRONG_PASSWORD;
		else if(strstr(status, "3"))
			return ESP8266_NOT_FOUND_TARGET_AP;
		else if(strstr(status, "+CWJAP:4"))
			return ESP8266_CONNECTION_FAILED;
		else
			return ESP8266_JOIN_UNKNOWN_ERROR;
	}
}

bool ESP8266_Start(uint8_t _ConnectionNumber, char* Domain, char* Port)
{
	char* command = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n";
	char* response = "OK";
	SendAT(command);
	return Start_ReadResponse(response);
}

char* ESP8266_Send(char* Data)
{
	char command[20];
	memset(command, 0, 20);
	sprintf(command, "AT+CIPSEND=%d\r\n", (strlen(Data)));
	command[19] = 0;
	char* response1 = "OK";
	SendAT(command);
	
	if(Default_ReadResponse(response1))
	{
		char* response2 = "SEND OK";
		SendAT(Data);
		return JSON_ReadResponse(response2);
	}
	else
	{
		PORTA = 0x0C;
		return "WHOOPS";
	}
}

int main(void)
{
	DDRA = 0x0F; PORTA = 0x00; //LED debug lights
	DDRC = 0xFF; PORTC = 0x00; //LCD Data Bus
	DDRD = 0xC0; PORTD = 0x00; //LCD Control Bus
	
	_delay_ms(1000);
	LCD_init();
	USART_initBaud(USART_Channel, ESP8266_Baud);/* Initiate USART with 115200 baud rate */
	USART_autoRecieve(1, USART_Channel); /* Initiate auto receiving of ESP8266 info*/
	
	sei();
	
	if (!ESP8266_WIFIMode())
	{	
		PORTA = 0x0C;
		while(1){}
	}
	
	_delay_ms(1000);
	
	if (!ESP8266_ConnectionMode())
	{
		PORTA = 0x0C;
		while(1){}
	}
	
	_delay_ms(1000);
	
	if (!ESP8266_ApplicationMode())
	{
		PORTA = 0x0C;
		while(1){}
	}
	
	_delay_ms(1000);
	
	if (!ESP8266_connected() == ESP8266_NOT_CONNECTED_TO_AP)
	{
		PORTA = 0x0C;
		while(1){}
	}
	
	_delay_ms(1000);
	
	if (!ESP8266_JoinAccessPoint() == ESP8266_WIFI_CONNECTED)
	{
		PORTA = 0x0C;
		while(1){}
	}
	
	_delay_ms(1000);
		
	if(ESP8266_Start(0, DOMAIN, PORT))
	{
		char* buffer = "GET /channels/780705/fields/1.json\r\n\r\n";
		char* ans = ESP8266_Send(buffer);
		PORTA = 0x0F;
	}
	else
	{
		PORTA = 0x0C;
	}

	while(1){}
}


