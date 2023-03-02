#ifndef INC_NMEA_H_
#define INC_NMEA_H_

#include "stm32f1xx_hal.h"

#define UART_BUFFER_SIZE 512

typedef struct {
	int hour;
	int min;
	int sec;
}TIME;

typedef struct {
	float latitude;
	char NS;
	float longitude;
	char EW;
}LOCATION;

typedef struct {
	float altitude;
	char unit;
}ALTITUDE;

typedef struct {
	int Day;
	int Mon;
	int Yr;
}DATE;

typedef struct {
	LOCATION lcation;
	TIME tim;
	int isfixValid;
	ALTITUDE alt;
	int numofsat;
}GGASTRUCT;

typedef struct {
	DATE date;
	float speed;
	float course;
	int isValid;
}RMCSTRUCT;

typedef struct {
	GGASTRUCT ggastruct;
	RMCSTRUCT rmcstruct;
}GPSSTRUCT;

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;


int decodeGGA (char *GGAbuffer, GGASTRUCT *gga);
int decodeRMC (char *RMCbuffer, RMCSTRUCT *rmc);
int Uart_read(void);
int IsDataAvailable(void);
int Look_for (char *str, char *buffertolookinto);
int Uart_peek();
int Copy_upto (char *string, char *buffertocopyinto);
int Get_after (char *string, uint8_t numberofchars, char *buffertosave);
int Wait_for (char *string);
void Ringbuf_init(void);
void Uart_write(int c);
void Uart_sendstring(const char *s);
void Uart_printbase (long n, uint8_t base);
void GetDataFromBuffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);
void Uart_flush (void);
void Uart_isr (UART_HandleTypeDef *huart);
void store_char(unsigned char c, ring_buffer *buffer);

#endif /* INC_NMEA_H_ */