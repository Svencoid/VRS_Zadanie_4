#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define DMA_USART2_BUFFER_SIZE 128

void MX_USART2_UART_Init(void);

void USART2_RegisterCallback(void *callback);
void USART2_PutBuffer(uint8_t *buffer, uint8_t length);
void USART2_CheckDmaReception(void);
uint16_t USART2_GetRxBufferUsage(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
