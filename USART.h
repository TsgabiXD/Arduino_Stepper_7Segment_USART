/***************************************************************************//**
Interface file for USART on ATMEL-Controller with one or more uarts
*******************************************************************************/
/** @addtogroup USART_api
@{*/
/** @file USART.h Interface file for USART-Module. */
#ifndef  USART_H
#define  USART_H

#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void    USARTInit (uint8_t uart, uint32_t baud,
                  uint8_t rx_enable, uint8_t rx_int_enable,
                  uint8_t tx_enable, uint8_t tx_int_enable);
void    USARTInitSTDIO (uint8_t uart);           // define STDIO (USART0 - 3)
void    USARTTransmit (uint8_t uart, char data); // simple USART-Transmit
char    USARTReceive  (uint8_t uart );           // simple USART-Receive
char    USARTCharReceived (uint8_t uart );       // test for char received
int16_t USARTPutChar  (char c, FILE *stream);    // put_char for printf
int16_t USARTGetChar (FILE *stream);             // get_char with echo for scanf
int16_t USARTGetCharNoEcho ( );             // get_char without echo
char    _getkey ( void );                        // get_char without echo

#ifdef __cplusplus
} // extern "C"
#endif

#endif // USART_H
/**@}*/
