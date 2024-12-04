/*
 * UartRingbuffer.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 */

#include "UartRingbuffer_multi.h"
#include <string.h>

/*  Define the device uart and pc uart below according to your setup  */
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

#define device_uart &huart3
#define pc_uart &huart2

/* put the following in the ISR
extern void Uart_isr (UART_HandleTypeDef *huart);
*/

ring_buffer rx_buffer1 = { { 0 }, 0, 0};
ring_buffer tx_buffer1 = { { 0 }, 0, 0};
ring_buffer rx_buffer2 = { { 0 }, 0, 0};
ring_buffer tx_buffer2 = { { 0 }, 0, 0};

ring_buffer *_rx_buffer1;
ring_buffer *_tx_buffer1;
ring_buffer *_rx_buffer2;
ring_buffer *_tx_buffer2;

void store_char (unsigned char c, ring_buffer *buffer);

void Ringbuf_init(void)
{
  _rx_buffer1 = &rx_buffer1;
  _tx_buffer1 = &tx_buffer1;
  _rx_buffer2 = &rx_buffer2;
  _tx_buffer2 = &tx_buffer2;

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(device_uart, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_ERR);

  /* Enable the UART Data Register not empty Interrupt */
  __HAL_UART_ENABLE_IT(device_uart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_RXNE);
}

void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

  // If the next position is the same as the tail, buffer is full, so don't store.
  if(i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}

int Look_for (char *str, char *buffertolookinto)
{
  int stringlength = strlen (str);
  int bufferlength = strlen (buffertolookinto);
  int so_far = 0;
  int indx = 0;

repeat:
  while (str[so_far] != buffertolookinto[indx]) indx++;
  if (str[so_far] == buffertolookinto[indx]){
    while (str[so_far] == buffertolookinto[indx])
    {
      so_far++;
      indx++;
    }
  } else {
    so_far = 0;
    if (indx >= bufferlength) return -1;
    goto repeat;
  }

  if (so_far == stringlength) return 1;
  else return -1;
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
    so_far = 0;
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
    so_far = 0;
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





void Uart_flush (UART_HandleTypeDef *uart)
{
  if (uart == device_uart)
  {
    memset(_rx_buffer1->buffer,'\0', UART_BUFFER_SIZE);
    _rx_buffer1->head = 0;
  }
  if (uart == pc_uart)
  {
    memset(_rx_buffer2->buffer,'\0', UART_BUFFER_SIZE);
    _rx_buffer2->head = 0;
  }
}

int Uart_peek(UART_HandleTypeDef *uart)
{
  if (uart == device_uart)
  {
    if(_rx_buffer1->head == _rx_buffer1->tail)
    {
      return -1;
    }
    else
    {
      return _rx_buffer1->buffer[_rx_buffer1->tail];
    }
  }

  else if (uart == pc_uart)
  {
    if(_rx_buffer2->head == _rx_buffer2->tail)
    {
      return -1;
    }
    else
    {
      return _rx_buffer2->buffer[_rx_buffer2->tail];
    }
  }

  return -1;
}

int Uart_read(UART_HandleTypeDef *uart)
{
  if (uart == device_uart)
  {
    if(_rx_buffer1->head == _rx_buffer1->tail)
    {
      return -1;
    }
    else
    {
      unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
      _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
      return c;
    }
  }

  else if (uart == pc_uart)
  {
    if(_rx_buffer2->head == _rx_buffer2->tail)
    {
      return -1;
    }
    else
    {
      unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
      _rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
      return c;
    }
  }

  else return -1;
}

void Uart_write(int c, UART_HandleTypeDef *uart)
{
  if (c>=0)
  {
    if (uart == device_uart){
      int i = (_tx_buffer1->head + 1) % UART_BUFFER_SIZE;

      while (i == _tx_buffer1->tail);

      _tx_buffer1->buffer[_tx_buffer1->head] = (uint8_t)c;
      _tx_buffer1->head = i;

      __HAL_UART_ENABLE_IT(device_uart, UART_IT_TXE); // Enable UART transmission interrupt
    }

    else if (uart == pc_uart){
      int i = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

      while (i == _tx_buffer2->tail);

      _tx_buffer2->buffer[_tx_buffer2->head] = (uint8_t)c;
      _tx_buffer2->head = i;

      __HAL_UART_ENABLE_IT(pc_uart, UART_IT_TXE); // Enable UART transmission interrupt
    }
  }
}

int IsDataAvailable(UART_HandleTypeDef *uart)
{
  if (uart == device_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;
  else if (uart == pc_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
  return -1;
}

int Get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart)
{
  while (Wait_for(string, uart) != 1);
  for (int indx=0; indx<numberofchars; indx++)
  {
    while (!(IsDataAvailable(uart)));
    buffertosave[indx] = Uart_read(uart);
  }
  return 1;
}

void Uart_sendstring (const char *s, UART_HandleTypeDef *uart)
{
  while(*s!='\0') Uart_write(*s++, uart);
}

void Uart_printbase (long n, uint8_t base, UART_HandleTypeDef *uart)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while (n);

  Uart_sendstring(s, uart);
}

char uart_buffer[50];  // Bộ đệm nhận UART
char command_buffer[50];  // Bộ đệm lệnh đã nhận
// Cập nhật trạng thái đèn giao thông
int receive_uart_command(void) {
    // Đọc dữ liệu UART nếu có sẵn
    HAL_UART_Receive(&huart2, (uint8_t *)uart_buffer, sizeof(uart_buffer), 100);

    // Kiểm tra và xử lý lệnh
    if (strstr(uart_buffer, "!INCREMENT RED#")) {
        return INCREMENT_RED_TIME;
    } else if (strstr(uart_buffer, "!DECREMENT RED#")) {
        return DECREMENT_RED_TIME;
    } else if (strstr(uart_buffer, "!INCREMENT GREEN#")) {
        return INCREMENT_GREEN_TIME;
    } else if (strstr(uart_buffer, "!DECREMENT GREEN#")) {
        return DECREMENT_GREEN_TIME;
    } else if (strstr(uart_buffer, "!INCREMENT YELLOW#")) {
        return INCREMENT_YELLOW_TIME;
    } else if (strstr(uart_buffer, "!DECREMENT YELLOW#")) {
        return DECREMENT_YELLOW_TIME;
    } else if (strstr(uart_buffer, "!SWITCH#")) {
        return SWITCH;
    } else if (strstr(uart_buffer, "!RESET#")) {
        return RESETALL;
    } else if (strstr(uart_buffer, "!SWITCH TO TUNING#")) {
        return SWITCH_TO_TUNING_MODE;
    }

    // Không có lệnh hợp lệ
    return 0;
}

unsigned int timeout;



int Copy_upto(char *string, char *buffertocopyinto, UART_HandleTypeDef *uart)
{
    int so_far = 0;
    int len = strlen(string);
    int indx = 0;

again:
    while (Uart_peek(uart) != string[so_far]) {
        buffertocopyinto[indx] = _rx_buffer1->buffer[_rx_buffer1->tail];  // or _rx_buffer2 if using the second UART
        _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
        indx++;
        while (!IsDataAvailable(uart));
    }
    while (Uart_peek(uart) == string[so_far]) {
        so_far++;
        buffertocopyinto[indx++] = Uart_read(uart);
        if (so_far == len) return 1;
        timeout = 100;
        while ((!IsDataAvailable(uart)) && timeout);
        if (timeout == 0) return 0;
    }

    if (so_far != len) {
        so_far = 0;
        goto again;
    }

    if (so_far == len) return 1;
    else return 0;
}



void ProcessIncomingCommand() {
    char command[50];
    int idx = 0;

    while (IsDataAvailable(pc_uart)) {
        char c = Uart_read(pc_uart); // Đọc từng ký tự từ UART

        if (c == '\n' || c == '\r') { // Nếu gặp ký tự kết thúc lệnh
            command[idx] = '\0';      // Kết thúc chuỗi
            receive_uart_command();   // Thực thi lệnh
            idx = 0;                  // Xóa bộ đệm lệnh
        } else {
            command[idx++] = c;       // Lưu ký tự vào chuỗi lệnh
        }
    }
}

int Wait_for(char *string, UART_HandleTypeDef *uart)
{
    int stringLength = strlen(string);
    int matchCount = 0;

    while (1) {
        // Kiểm tra xem có dữ liệu nào trong buffer không
        if (IsDataAvailable(uart)) {
            // Đọc ký tự từ buffer UART
            char c = Uart_read(uart);

            // So sánh ký tự vừa đọc với từng ký tự trong chuỗi cần tìm
            if (c == string[matchCount]) {
                matchCount++;

                // Nếu đã tìm đủ tất cả các ký tự trong chuỗi
                if (matchCount == stringLength) {
                    return 1;  // Chuỗi đã xuất hiện trong buffer
                }
            } else {
                matchCount = 0;  // Nếu ký tự không khớp, quay lại bắt đầu chuỗi
            }
        }
    }

    return 0;  // Không tìm thấy chuỗi trong buffer
}


// Hàm xử lý interrupt UART
void Uart_isr (UART_HandleTypeDef *huart) {
    uint32_t isrflags = READ_REG(huart->Instance->SR);
    uint32_t cr1its = READ_REG(huart->Instance->CR1);

    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
        unsigned char c = huart->Instance->DR;
        if (huart == device_uart) {
            store_char(c, _rx_buffer1);
        } else if (huart == pc_uart) {
            store_char(c, _rx_buffer2);
        }
    }

    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET)) {
        if (huart == device_uart) {
            if (_tx_buffer1->head != _tx_buffer1->tail) {
                unsigned char c = _tx_buffer1->buffer[_tx_buffer1->tail];
                _tx_buffer1->tail = (_tx_buffer1->tail + 1) % UART_BUFFER_SIZE;
                huart->Instance->DR = c;
            }
        } else if (huart == pc_uart) {
            if (_tx_buffer2->head != _tx_buffer2->tail) {
                unsigned char c = _tx_buffer2->buffer[_tx_buffer2->tail];
                _tx_buffer2->tail = (_tx_buffer2->tail + 1) % UART_BUFFER_SIZE;
                huart->Instance->DR = c;
            }
        }
    }
}

