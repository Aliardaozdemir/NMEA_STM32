#include "NMEA.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "UartRingbuffer.h"

#define uart &huart1
#define TIMEOUT_DEF 500

int GMT = +530;
int inx = 0;
int hr=0,min=0,day=0,mon=0,yr=0;
int daychange = 0;

int decodeGGA (char *GGAbuffer, GGASTRUCT *gga)
{
	inx = 0;
	char buffer[12];
	int i = 0;
	
	for(;GGAbuffer[inx] != ','; inx++);  
	inx++;
	for(;GGAbuffer[inx] != ','; inx++);  
	inx++;
	for(;GGAbuffer[inx] != ','; inx++); 
	inx++;
	for(;GGAbuffer[inx] != ','; inx++);  
	inx++;
	for(;GGAbuffer[inx] != ','; inx++);  
	inx++;
	for(;GGAbuffer[inx] != ','; inx++);  
	inx++;  
	
	
	if ((GGAbuffer[inx] == '1') || (GGAbuffer[inx] == '2') || (GGAbuffer[inx] == '6'))   
	{
		gga->isfixValid = 1;   
		inx = 0;   
	}
	else
	{
		gga->isfixValid = 0;   
		return 1;  
	}
	for(;GGAbuffer[inx] != ','; inx++);  

	inx++;   
	memset(buffer, '\0', 12);
	
	for(i=0; GGAbuffer[inx] != ',';){
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	
	hr = (atoi(buffer)/10000) + GMT/100;   

	min = ((atoi(buffer)/100)%100) + GMT%100;  
	
	if (min > 59) 
	{
		min = min-60;
		hr++;
	}
	if (hr<0)
	{
		hr=24+hr;
		daychange--;
	}
	if (hr>=24)
	{
		hr=hr-24;
		daychange++;
	}

	
	gga->tim.hour = hr;
	gga->tim.min = min;
	gga->tim.sec = atoi(buffer)%100;


	inx++;   
	memset(buffer, '\0', 12);
	for(i=0; GGAbuffer[inx] != ',';){
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	if (strlen(buffer) < 6) return 2;  
	int16_t num = (atoi(buffer));   
	int j;
	 
	for(j=0;buffer[j] != '.';j++);
	j++;
	int declen = (strlen(buffer))-j;  
	int dec = atoi ((char *) buffer+j);  
	float lat = (num/100.0) + (dec/pow(10, (declen+2)));  
	gga->lcation.latitude = lat;  
	inx++;  
	gga->lcation.NS = GGAbuffer[inx];  



	inx++;  
	inx++;  
	memset(buffer, '\0', 12);
	for(i=0; GGAbuffer[inx] != ',';){
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	num = (atoi(buffer));  
	for(j=0;buffer[j] != '.';j++);  
	j++;
	declen = (strlen(buffer))-j;  
	dec = atoi ((char *) buffer+j);  
	lat = (num/100.0) + (dec/pow(10, (declen+2)));  
	gga->lcation.longitude = lat;  
	inx++;
	gga->lcation.EW = GGAbuffer[inx];  


	
	inx++;   
	inx++;   
	inx++;   

	
	inx++;  
	memset(buffer, '\0', 12);
	for(i=0; GGAbuffer[inx] != ',';){
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	gga->numofsat = atoi(buffer);   


	
	inx++;
	for(j=0;buffer[j] != '.';j++);  


	
	inx++;
	memset(buffer, '\0', 12);
	i=0;
	for(i=0; GGAbuffer[inx] != ',';){
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	num = (atoi(buffer));
	j = 0;
	for(j=0;buffer[j] != '.';j++); 
	j++;
	declen = (strlen(buffer))-j;
	dec = atoi ((char *) buffer+j);
	lat = (num) + (dec/pow(10, (declen)));
	gga->alt.altitude = lat;

	inx++;
	gga->alt.unit = GGAbuffer[inx];

	return 0;

}

int decodeRMC (char *RMCbuffer, RMCSTRUCT *rmc)
{
	inx = 0;
	char buffer[12];
	int i = 0;
	
	for(;RMCbuffer[inx] != ',';inx++);
	inx++;
	for(;RMCbuffer[inx] != ',';inx++);
	inx++;
	
	
	if (RMCbuffer[inx] == 'A')  
	{
		rmc->isValid = 1;
	}
	else
	{
		rmc->isValid =0;
		return 1;
	}
	
	
	inx++;
	inx++;
	for(;RMCbuffer[inx] != ',';inx++);  
	for(;RMCbuffer[inx] != ',';inx++);  
	inx++;
	for(;RMCbuffer[inx] != ',';inx++);  
	inx++;
	for(;RMCbuffer[inx] != ',';inx++); 

	
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	for(;RMCbuffer[inx] != ',';){
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	if (strlen (buffer) > 0){          
		int16_t num = (atoi(buffer));  
		int j = 0;
		for(;RMCbuffer[inx] != ',';j++)
		j++;
		int declen = (strlen(buffer))-j;
		int dec = atoi ((char *) buffer+j);
		float lat = num + (dec/pow(10, (declen)));
		rmc->speed = lat;
	}
	else rmc->speed = 0;

	
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	for(;RMCbuffer[inx] != ',';){
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	if (strlen (buffer) > 0){  
		int16_t num = (atoi(buffer));   
		int j = 0;
		for(;RMCbuffer[inx] != ',';j++)   
		j++;
		int declen = (strlen(buffer))-j;
		int dec = atoi ((char *) buffer+j);
		float lat = num + (dec/pow(10, (declen)));
		rmc->course = lat;
	}
	else
		{
			rmc->course = 0;
		}

	
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	for(;RMCbuffer[inx] != ',';){
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	
	day = atoi(buffer)/10000;  
	mon = (atoi(buffer)/100)%100;  
	yr = atoi(buffer)%100;  

	day = day+daychange;   

	
	rmc->date.Day = day;
	rmc->date.Mon = mon;
	rmc->date.Yr = yr;

	return 0;
}

ring_buffer rx_buffer = { { 0 }, 0, 0};
ring_buffer tx_buffer = { { 0 }, 0, 0};
ring_buffer *_rx_buffer;
ring_buffer *_tx_buffer;

void Ringbuf_init(void)
{
  _rx_buffer = &rx_buffer;
  _tx_buffer = &tx_buffer;
  __HAL_UART_ENABLE_IT(uart, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(uart, UART_IT_RXNE);
}
void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;
  if(i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}
static int check_for (char *str, char *buffertolookinto)
{
	int stringlength = strlen (str);
	int bufferlength = strlen (buffertolookinto);
	int so_far = 0;
	int indx = 0;
repeat:
	while (str[so_far] != buffertolookinto[indx])
		{
			indx++;
			if (indx>stringlength) return 0;
		}
	if (str[so_far] == buffertolookinto[indx])
	{
		while (str[so_far] == buffertolookinto[indx])
		{
			so_far++;
			indx++;
		}
	}
	if (so_far == stringlength);
	else
	{
		so_far =0;
		if (indx >= bufferlength) return -1;
		goto repeat;
	}
	if (so_far == stringlength) return 1;
	else return -1;
}
int Uart_read(void)
{
  if(_rx_buffer->head == _rx_buffer->tail)
  {
    return -1;
  }
  else
  {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % UART_BUFFER_SIZE;
    return c;
  }
}
void Uart_write(int c)
{
	if (c>=0)
	{
		int i = (_tx_buffer->head + 1) % UART_BUFFER_SIZE;
		while (i == _tx_buffer->tail);

		_tx_buffer->buffer[_tx_buffer->head] = (uint8_t)c;
		_tx_buffer->head = i;

		__HAL_UART_ENABLE_IT(uart, UART_IT_TXE); 
	}
}
int IsDataAvailable(void)
{
  return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % UART_BUFFER_SIZE;
}
void Uart_sendstring (const char *s)
{
	while(*s) Uart_write(*s++);
}
void GetDataFromBuffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto)
{
	int startStringLength = strlen (startString);
	int endStringLength   = strlen (endString);
	int so_far = 0;
	int indx = 0;
	int startposition = 0;
	int endposition = 0;
repeat1:
	while (startString[so_far] != buffertocopyfrom[indx]) indx++;
	if (startString[so_far] == buffertocopyfrom[indx])
	{
		while (startString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}
	if (so_far == startStringLength) startposition = indx;
	else
	{
		so_far =0;
		goto repeat1;
	}
	so_far = 0;
repeat2:
	while (endString[so_far] != buffertocopyfrom[indx]) indx++;
	if (endString[so_far] == buffertocopyfrom[indx])
	{
		while (endString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}
	if (so_far == endStringLength) endposition = indx-endStringLength;
	else
	{
		so_far =0;
		goto repeat2;
	}
	so_far = 0;
	indx=0;
	for (int i=startposition; i<endposition; i++)
	{
		buffertocopyinto[indx] = buffertocopyfrom[i];
		indx++;
	}
}
void Uart_flush (void)
{
	memset(_rx_buffer->buffer,'\0', UART_BUFFER_SIZE);
	_rx_buffer->head = 0;
	_rx_buffer->tail = 0;
}
int Uart_peek()
{
  if(_rx_buffer->head == _rx_buffer->tail)
  {
    return -1;
  }
  else
  {
    return _rx_buffer->buffer[_rx_buffer->tail];
  }
}
int Copy_upto (char *string, char *buffertocopyinto)
{
	int so_far =0;
	int len = strlen (string);
	int indx = 0;

again:
	while (Uart_peek() != string[so_far])
		{
			buffertocopyinto[indx] = _rx_buffer->buffer[_rx_buffer->tail];
			_rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % UART_BUFFER_SIZE;
			indx++;
			while (!IsDataAvailable());
		}
	while (Uart_peek() == string [so_far])
	{
		so_far++;
		buffertocopyinto[indx++] = Uart_read();
		if (so_far == len) return 1;
		timeout = TIMEOUT_DEF;
		while ((!IsDataAvailable())&&timeout);
		if (timeout == 0) return 0;
	}
	if (so_far != len)
	{
		so_far = 0;
		goto again;
	}
	if (so_far == len) return 1;
	else return 0;
}

int Get_after (char *string, uint8_t numberofchars, char *buffertosave)
{
	for (int indx=0; indx<numberofchars; indx++)
	{
		timeout = TIMEOUT_DEF;
		while ((!IsDataAvailable())&&timeout);  
		if (timeout == 0) return 0;  
		buffertosave[indx] = Uart_read();  
	}
	return 1;
}
int Wait_for (char *string)
{
	int so_far =0;
	int len = strlen (string);

again:
	timeout = TIMEOUT_DEF;
	while ((!IsDataAvailable())&&timeout);  
	if (timeout == 0) return 0;
	while (Uart_peek() != string[so_far])  
	{
		if (_rx_buffer->tail != _rx_buffer->head)
		{
			_rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % UART_BUFFER_SIZE;  
		}
		else
		{
			return 0;
		}
	}
	while (Uart_peek() == string [so_far]) 
	{
		so_far++;
		_rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % UART_BUFFER_SIZE;  
		if (so_far == len) return 1;
		timeout = TIMEOUT_DEF;
		while ((!IsDataAvailable())&&timeout);
		if (timeout == 0) return 0;
	}
	if (so_far != len)
	{
		so_far = 0;
		goto again;
	}
	if (so_far == len) return 1;
	else return 0;
}

void Uart_isr (UART_HandleTypeDef *huart)
{
	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
 
		huart->Instance->SR;                       
        unsigned char c = huart->Instance->DR;     
        store_char (c, _rx_buffer); 
        return;
    }
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
    	if(tx_buffer.head == tx_buffer.tail)
    	    {
    	      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
    	    }
    	 else
    	    {
    	      unsigned char c = tx_buffer.buffer[tx_buffer.tail];
    	      tx_buffer.tail = (tx_buffer.tail + 1) % UART_BUFFER_SIZE;
    	      huart->Instance->SR;
    	      huart->Instance->DR = c;

    	    }
    	return;
    }
}

