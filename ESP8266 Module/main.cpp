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
#include "json_utils.h"		/* Include JSON parser functs */

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

enum ESP8266_CONNECT_STATUS {
	ESP8266_CONNECTED_TO_AP,
	ESP8266_CREATED_TRANSMISSION,
	ESP8266_TRANSMISSION_DISCONNECTED,
	ESP8266_NOT_CONNECTED_TO_AP,
	ESP8266_CONNECT_UNKNOWN_ERROR
};


//Receive line sent from WIFI that was automatically received through USART.
bool Default_ReadResponse(char* expectedResponse)
{
	PORTA = 0x01;
	//Receive echo
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	PORTA = 0x01;
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	PORTA - 0x02;
	//Receive OK
	while(!USART_hasLine(USART_Channel)){}
	char* ok = USART_getLine(USART_Channel);
	unsigned char* dispstr = (unsigned char*) ok;
	
	LCD_DisplayString(1, dispstr);
	
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
	unsigned char* dispstr = (unsigned char*) ok;
	
	LCD_DisplayString(1, dispstr);
	
	//Check if received expected response
	if(strstr(ok, expectedResponse))
	{
		return status;
	}
	char* failed = "failed";
	return failed;
}

//Receive line sent from WIFI that was automatically received through USART. Catches echo.
bool JoinAP_ReadResponse(char* connected, char* gotIP)
{
	//Receive crlf
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	while(!USART_hasLine(USART_Channel)){}
	char* status1 = USART_getLine(USART_Channel);
	
	if(strstr(status1, "DISCONNECT")){
		//Receive WIFI CONNECTED
		while(!USART_hasLine(USART_Channel)){}
		char* status2 = USART_getLine(USART_Channel);
		
		//Receive WIFI GOT IP
		while(!USART_hasLine(USART_Channel)){}
		char* ip = USART_getLine(USART_Channel);
		
		//Receive crlf
		while(!USART_hasLine(USART_Channel)){}
		USART_getLine(USART_Channel);
		
		unsigned char* dispstr1 = (unsigned char*) status2;
		LCD_DisplayString(1, dispstr1);
		
		//Check if received expected response
		if(strstr(status2, connected) && strstr(ip, gotIP))
		{
			return true;
		}
		return false;
	}
	else if(strstr(status1, "ERROR")){
		return false;
	}
	else{
		PORTA = 0x01;
		//Receive WIFI GOT IP
		while(!USART_hasLine(USART_Channel)){}
		char* ip = USART_getLine(USART_Channel);
			
		PORTA = 0x02;
		//Receive crlf
		while(!USART_hasLine(USART_Channel)){}
		USART_getLine(USART_Channel);
		
		unsigned char* dispstr2 = (unsigned char*) status1;
		LCD_DisplayString(1, dispstr2);
		
		//Check if received expected response
		if(strstr(status1, connected) && strstr(ip, gotIP))
		{
			return true;
		}
		return false;
	}
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
	unsigned char* dispstr = (unsigned char*) ok;
	
	LCD_DisplayString(1, dispstr);
	
	//Check if received expected response
	if(strstr(ok, expectedResponse))
	{
		return true;
	}
	return false;
}

char* JSON_ReadResponse()
{
	//Receive received notice
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive SEND OK
	while(!USART_hasLine(USART_Channel)){}
	USART_getLine(USART_Channel);
	
	//Receive JSON*/
	while(!USART_hasLine(USART_Channel)){}
	char* json = USART_getLine(USART_Channel);
	
	
	while(json && *json != '['){
		while(!USART_hasLine(USART_Channel)){}
		json = USART_getLine(USART_Channel);
	}

	//Check if received expected response
	return json;
}

//Send AT command to ESP8622 WIFI Module.
void SendAT(char* atCommand)
{	
	PORTA = 0x01;
	USART_clearBuf(USART_Channel);
	USART_sendLine(atCommand, USART_Channel);		/* Send AT command to ESP8266 */
	//while(!USART_hasTransmittedLine(USART_Channel)){}
	PORTA= 0x00;
}

//Send AT command and receive OK. Checks if operational.
bool ESP8266_Begin()
{
	char* command = "AT\r\n";
	char* response = "K";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command CWMODE. Sets up WIFI mode. 
bool ESP8266_WIFIMode()
{
	char* command = "AT+CWMODE_CUR=3\r\n";//Set up WIFI module as both Access point and STA
	char* response = "K";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command AT+CIPMUX. Sets up Enable for multiple connections. In this case sending 0 makes it a single connection.
bool ESP8266_ConnectionMode()
{
	char* command = "AT+CIPMUX=0\r\n";//Set up WIFI module for singular connection
	char* response = "K";
	SendAT(command);
	return Default_ReadResponse(response);
}

//Send AT Command AT+CIPMODE. Sets up transfer mode. In this case sending 0 makes it a a normal mode where the WIF module must reconnect constantly to TCP.
bool ESP8266_ApplicationMode()
{
	char* command = "AT+CIPMODE=0\r\n";//Set up WIFI module for singular connection
	char* response = "K";
	SendAT(command);
	return Default_ReadResponse(response);
}

uint8_t ESP8266_connected()
{
	char* command = "AT+CIPSTATUS\r\n";//Set up WIFI module for singular connection
	char* response = "K";
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

bool ESP8266_JoinAccessPoint()
{
	char* command = "AT+CWJAP_CUR=\"AWDS\",\"automaticwarehousedeliverysystem\"\r\n";
	char* response1 = "CONNECTED";
	char* response2 = "GOT IP";
	SendAT(command);
	return JoinAP_ReadResponse(response1, response2);
}

bool ESP8266_Start()
{
	char* command = "AT+CIPSTART=\"TCP\",\"192.168.43.51\",1000\r\n";
	char* response = "K";
	SendAT(command);
	return Start_ReadResponse(response);
}

char* ESP8266_Send(char* Data)
{
	char command[20];
	memset(command, 0, 20);
	sprintf(command, "AT+CIPSEND=%d\r\n", (strlen(Data)));
	command[19] = 0;
	char* response1 = "K";
	SendAT(command);
	
	if(Default_ReadResponse(response1))
	{
		_delay_ms(10);
		SendAT(Data);
		return JSON_ReadResponse();
	}
	else
	{
		return "ERR";
	}
}

bool WIFI_init(){
		
		sei();
		
		if (!ESP8266_WIFIMode())
		{
			return false;
		}
		
		_delay_ms(100);
		
		if (!ESP8266_ConnectionMode())
		{
			return false;
		}
		
		_delay_ms(100);
		
		if (!ESP8266_ApplicationMode())
		{
			return false;
		}
		
		_delay_ms(100);
		
		if (!ESP8266_JoinAccessPoint())
		{
			return false;
		}
		return true;
}

char* getOperation(){
	char* httpReq = "GET /packages/001\r\n\r\n";
	uint8_t val = ESP8266_connected();
	_delay_ms(100);
	
	if(val == ESP8266_CONNECTED_TO_AP)
	{
		if(ESP8266_Start())
		{
			_delay_ms(100);
			char* ans = ESP8266_Send(httpReq);
			return ans;
		}
		else
		{
			return "[]";
		}
	}
	else if(val == ESP8266_CREATED_TRANSMISSION)
	{
		char* ans = ESP8266_Send(httpReq);
		return ans;
	}
	else if(val == ESP8266_TRANSMISSION_DISCONNECTED)
	{
		if(ESP8266_Start())
		{
			_delay_ms(100);
			char* ans = ESP8266_Send(httpReq);
			return ans;
		}
		else
		{
			return "[]";
		}
	}
	else if(val == ESP8266_NOT_CONNECTED_TO_AP)
	{
		if (ESP8266_JoinAccessPoint())
		{
			_delay_ms(10);
			if(ESP8266_Start())
			{
				_delay_ms(10);
				char* ans = ESP8266_Send(httpReq);
				return ans;
			}
			else
			{
				return "[]";
			}
		}
		return "[]";
	}
	else
	{
		return "[]";
	}
}

int main(void)
{
	DDRA = 0x0F; PORTA = 0x00; //LED debug lights
	DDRC = 0xFF; PORTC = 0x00; //LCD Data Bus
	DDRD = 0xC0; PORTD = 0x00; //LCD Control Bus
	
	//Initialize Dependencies
	_delay_ms(1000);
	LCD_init();
	USART_initBaud(USART_Channel, ESP8266_Baud);/* Initiate USART with 115200 baud rate */
	USART_autoRecieve(1, USART_Channel); /* Initiate auto receiving of ESP8266 info*/		
	_delay_ms(1000);
	
	//INIT WIFI
	if(!WIFI_init())
	{
		PORTA = 0x0C;
	}
	else
	{
		char* operation = getOperation();
		char* op2 = operation;
		
		while(*op2++ != ']'){
			
		}
		*(op2-1) = 0;
		
		char* key = "GCODE";
		 char* res = getValueFromKey(operation+1, key);
		 
		 if(res == 0){
			 unsigned char* op = (unsigned char*) "JSON PARSE FAILED";
			 LCD_DisplayString(1, op);
		 }
		 else{
			 PORTA = 0x0F;
			 unsigned char* displayres = (unsigned char*) op2;
			 LCD_DisplayString(1, displayres);
			 free(res);
		 }
	}
	while(1){}
}


