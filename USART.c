/***************************************************************************//**
Implementation file for USART on ATMEL-Controller with one or more uarts

@author LiTec/PAMG
@date 17.10.2012
@version 1.2

@param compiler switches: none
@param linker switches: none

@b History:
- 1.2 LiTec/PAMG 17.10.2012 Enhancement: routines for scanf and _getkey.
- 1.1 LiTec/PAMG 20.09.2012 Enhancement of baudrates for ATMEGA2560.
- 1.0 LiTec/PAMG 02.01.2012 initial version.
*******************************************************************************/
/** @addtogroup USART_api
@{*/
/** @file USART.c Main source file for USART-Module. */
#include "USART.h"

#if defined (SCANF_ECHO)
static FILE mystdinout = FDEV_SETUP_STREAM(USARTPutChar, USARTGetChar, _FDEV_SETUP_RW);
#else
static FILE mystdinout = FDEV_SETUP_STREAM (USARTPutChar, USARTGetCharNoEcho, _FDEV_SETUP_RW);
#endif
uint8_t usart_stdio = 0;                // use USART0 as STDIO

/***************************************************************************//**
Function to set USART for STDIO
@param uart select one of the controllers USART (0 .. n see datasheet)
@retval - none
*******************************************************************************/
void USARTInitSTDIO (uint8_t uart) {

  if ( uart < 4) {
    usart_stdio = uart;
  } else {
    usart_stdio = 0;
  }
}

/***************************************************************************//**
Function for USART initialization
@param uart select one of the controllers USART (0 .. n see datasheet)
@param baud defines the baud-rate (depends on the CPU-clk)
@param rx_enable enables the USARTx-receiver
@param rx_int_enable enables the USARTx-RX-interrupt
@param tx_enable enables the USARTx-transmitter
@param tx_int_enable enables the USARTx-TX-interrupt
@retval - none
*******************************************************************************/
void USARTInit (uint8_t uart, uint32_t baud,
                uint8_t rx_enable, uint8_t rx_int_enable,
                uint8_t tx_enable, uint8_t tx_int_enable ) {

#ifndef F_CPU
  #warning "F_CPU undefined --> used in module USART.C"
#endif
  uint8_t ubrrh = 0;
  uint8_t ubrrl = 0;
  uint8_t doubleTransSpeed = 0;

  stdout = stdin = &mystdinout;

  switch ( F_CPU ) {
   case 4000000L :
     switch ( baud ) {
      case 19200 :
        ubrrl =12;                      // 19200 at 0.2% error see datasheet
        break;
      case 2400 :
        ubrrl =103;                     // 2400 at 0.2% error see datasheet
        break;
      default: // 9600
        ubrrl =25;                      // 9600 at 0.2% error see datasheet
        break;
     }
     break;

   case 7372800L :
     switch ( baud ) {
      case 115200 :
        ubrrl = 3;                      // 115200 at 0.0% error see datasheet
        break;
      case 19200 :
        ubrrl =23;                      // 19200 at 0.0% error see datasheet
        break;
      default: // 9600
        ubrrl =47;                      // 9600 at 0.0% error see datasheet
        break;
     }
     break;

   case 8000000L :
     switch ( baud ) {
      case 19200 :
        ubrrl =25;                       // 19200 at 0.2% error see datasheet
        break;
      case 2400 :
        ubrrl =207;                      // 2400 at 0.2% error see datasheet
        break;
      default: // 9600
        ubrrl =51;                       // 9600 at 0.2% error see datasheet
        break;
     }
     break;

   case 16000000L :
     switch ( baud ) {
      case 115200 : // 115200bps at 2.1% error see datasheet
        doubleTransSpeed = 1; ubrrl = 16; break;
      case 57600 : // 57600bps at -0.8% error see datasheet UBRR = 34
        doubleTransSpeed = 1; ubrrl = 34; break;
      case 38400 : // 38400bps at 0.2% error see datasheet UBRR = 51
        doubleTransSpeed = 1; ubrrl = 51; break;
      case 19200 : // 19200bps at 0.2% error see datasheet
        ubrrl = 51; break;
      case 4800 : // 4800bps at -0.1% error see datasheet UBRR = 416
        doubleTransSpeed = 1; ubrrl = 160; ubrrh = 1; break;
      case 2400 : // 2400bps at 0.0% error see datasheet UBRR = 832
        doubleTransSpeed = 1; ubrrl = 64; ubrrh = 3; break;
      case 1200 : // 2400bps at 0.0% error see datasheet UBRR = 1666
        doubleTransSpeed = 1; ubrrl = 130;  ubrrh = 6; break;
      default: // 9600bps at 0.2% error see datasheet
        ubrrl = 103; break;
     }
     break;

   default:   // missing or unknown F_CPU
     return;
     break;
  }

#if defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny26__)
  UCSRA = 0x00;                      // !double Transmission Speed
  UBRRH = ubrrh; UBRRL = ubrrl;
  /* enable receiver and transmitter */
  UCSRB = ((rx_enable & 0x01) << RXEN) | ((rx_int_enable & 0x01) << RXCIE) |
          ((tx_enable & 0x01) << TXEN) | ((tx_int_enable & 0x01) << TXCIE);
  /* set frame format: 8data, 1stop bit, no parity */
  UCSRC = (1<<UCSZ1) | (1<<UCSZ0);
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
  UCSRA = 0x00;                      // !double Transmission Speed
  UBRRH = ubrrh; UBRRL = ubrrl;
  /* enable receiver and transmitter */
  UCSRB = ((rx_enable & 0x01) << RXEN) | ((rx_int_enable & 0x01) << RXCIE) |
          ((tx_enable & 0x01) << TXEN) | ((tx_int_enable & 0x01) << TXCIE);
  /* set frame format: 8data, 1stop bit, no parity */
  UCSRC = (1<<URSEL) /*| (1<<USBS)*/ |(1<<UCSZ1) | (1<<UCSZ0);
#elif defined (__AVR_ATmega2560__)
  /* enable receiver and transmitter */
  /* set frame format: 8data, 1stop bit, no parity */
  switch ( uart ) {
    case 0 :
      if ( doubleTransSpeed ) UCSR0A |= (1<<U2X0); // double Transmission Speed
      else                    UCSR0A = 0x00;
      UBRR0H = ubrrh; UBRR0L = ubrrl;
      UCSR0B = ((rx_enable & 0x01)<<RXEN0) | ((rx_int_enable & 0x01)<<RXCIE0) |
               ((tx_enable & 0x01)<<TXEN0) | ((tx_int_enable & 0x01)<<TXCIE0);
      UCSR0C=0x06;       // 8 Databit see datasheet
      break;
    case 1 :
      if ( doubleTransSpeed ) UCSR1A |= (1<<U2X1); // double Transmission Speed
      else                    UCSR1A = 0x00;
      UBRR1H = ubrrh; UBRR1L = ubrrl;
      UCSR1B = ((rx_enable & 0x01)<<RXEN1) | ((rx_int_enable & 0x01)<<RXCIE1) |
               ((tx_enable & 0x01)<<TXEN1) | ((tx_int_enable & 0x01)<<TXCIE1);
      UCSR1C=0x06;       // 8 Databit see datasheet
      break;
    case 2 :
      if ( doubleTransSpeed ) UCSR2A |= (1<<U2X2); // double Transmission Speed
      else                    UCSR2A = 0x00;
      UBRR2H = ubrrh; UBRR2L = ubrrl;
      UCSR2B = ((rx_enable & 0x01)<<RXEN2) | ((rx_int_enable & 0x01)<<RXCIE2) |
               ((tx_enable & 0x01)<<TXEN2) | ((tx_int_enable & 0x01)<<TXCIE2);
      UCSR2C=0x06;       // 8 Databit see datasheet
      break;
    case 3 :
      if ( doubleTransSpeed ) UCSR3A |= (1<<U2X3); // double Transmission Speed
      else                    UCSR3A = 0x00;
      UBRR3H = ubrrh; UBRR3L = ubrrl;
      UCSR3B = ((rx_enable & 0x01)<<RXEN3) | ((rx_int_enable & 0x01)<<RXCIE3) |
               ((tx_enable & 0x01)<<TXEN3) | ((tx_int_enable & 0x01)<<TXCIE3);
      UCSR3C=0x06;       // 8 Databit see datasheet
      break;
    default: break;
  }
#else
  #warning "MCU unknown"
#endif
} // USART_init

/***************************************************************************//**
Function for sending a character via USART
@param uart select one of the controllers USART (0 .. n see datasheet)
@param data data to be transmitted
@retval - none
*******************************************************************************/
void USARTTransmit (uint8_t uart, char data ) {
#if defined (__AVR_ATmega2560__)
  switch ( uart ) {
   case 0 : while ( !(UCSR0A & (1<<UDRE0))) { }  // wait for old data sended
            UDR0 = data;  break;
   case 1 : while ( !(UCSR1A & (1<<UDRE1))) { }  // wait for old data sended
            UDR1 = data;  break;
   case 2 : while ( !(UCSR2A & (1<<UDRE2))) { }  // wait for old data sended
            UDR2 = data; break;
   case 3 : while ( !(UCSR3A & (1<<UDRE3))) { }  // wait for old data sended
            UDR3 = data;  break;
   default: break;
  }
#else // only one uart
  while ( !(UCSRA & (1<<UDRE))) { } // wait for old data sended
  UDR = data;
#endif
} // USARTTransmit

/***************************************************************************//**
Function for receiving a character via USART
@param uart select one of the controllers USART (0 .. n see datasheet)
@retval received_character
*******************************************************************************/
char USARTReceive (uint8_t uart ) {
#if defined (__AVR_ATmega2560__)
  switch ( uart ) {
   case 0 : while ( !(UCSR0A & (1<<RXC0))) { } // wait for data
            return UDR0;  break;
   case 1 : while ( !(UCSR1A & (1<<RXC1))) { } // wait for data
            return UDR1;  break;
   case 2 : while ( !(UCSR2A & (1<<RXC2))) { } // wait for data
            return UDR2;  break;
   case 3 : while ( !(UCSR3A & (1<<RXC3))) { } // wait for data
            return UDR3;  break;
   default: return 0; break;
  }
#else                                   // only one uart
  while ( !(UCSRA & (1<<RXC))) { }      // wait for data
  return UDR;
#endif
} // USARTReceive

/***************************************************************************//**
Function for testing if char received
@param uart select one of the controllers USART (0 .. n see datasheet)
@retval 1 .. char received, 0 .. no char received
*******************************************************************************/
char USARTCharReceived (uint8_t uart ) {
	#if defined (__AVR_ATmega2560__)
	switch ( uart ) {
	case 0 : return (UCSR0A & (1<<RXC0)); break;
	case 1 : return (UCSR1A & (1<<RXC1)); break;
	case 2 : return (UCSR2A & (1<<RXC2)); break;
	case 3 : return (UCSR3A & (1<<RXC3)); break;
    default: return 0; break;
  }
#else                                   // only one uart
  return (UCSRA & (1<<RXC));
#endif
} // USARTReceive

/***************************************************************************//**
Function for sending data via standard USART by printf
@param c character to be sent
@param *stream outputstream
@retval c (sended char), expected by printf
*******************************************************************************/
int16_t USARTPutChar(char c, FILE *stream) {
#if defined (__AVR_ATmega2560__)        // stdio sends by USART0
  switch ( usart_stdio) {
    case 3 :
      if (c == '\n') USARTPutChar('\r', stream);
      loop_until_bit_is_set(UCSR3A, UDRE3);
      UDR3 = c;
      break;
    case 2 :
      if (c == '\n') USARTPutChar('\r', stream);
      loop_until_bit_is_set(UCSR2A, UDRE2);
      UDR2 = c;
      break;
    case 1 :
      if (c == '\n') USARTPutChar('\r', stream);
      loop_until_bit_is_set(UCSR1A, UDRE1);
      UDR1 = c;
      break;
    default:
      if (c == '\n') USARTPutChar('\r', stream);
      loop_until_bit_is_set(UCSR0A, UDRE0);
      UDR0 = c;
      break;
  }
  return c;
#else
  if (c == '\n') USARTPutChar('\r', stream);
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
  return c;
#endif
} // USARTPutChar

/***************************************************************************//**
Function for receiving data from standard USART by scanf with echo
@param *stream outputstream for echo
@retval received character
*******************************************************************************/
int16_t USARTGetChar(FILE *stream) {
#if defined (__AVR_ATmega2560__)        // stdio sends by USART0
  switch ( usart_stdio) {
    case 3 :
      while (!(UCSR3A & (1<<RXC3)));
      return (int16_t)(USARTPutChar(UDR3, stream)); // echo;
    break;
    case 2 :
      while (!(UCSR2A & (1<<RXC2)));
      return (int16_t)(USARTPutChar(UDR2, stream)); // echo;
    break;
    case 1 :
      while (!(UCSR1A & (1<<RXC1)));
      return (int16_t)(USARTPutChar(UDR1, stream)); // echo;
    break;
    default:
      while (!(UCSR0A & (1<<RXC0)));
      return (int16_t)(USARTPutChar(UDR0, stream)); // echo;
      break;
  }
#endif
} // USARTGetChar

/***************************************************************************//**
Function for receiving data from standard USART by scanf without echo
@retval received character
*******************************************************************************/
int16_t USARTGetCharNoEcho(  ) {
#if defined (__AVR_ATmega2560__)        // stdio sends by USART0
  switch ( usart_stdio) {
    case 3 :
      while (!(UCSR3A & (1<<RXC3)));
      return UDR3;
    break;
    case 2 :
      while (!(UCSR2A & (1<<RXC2)));
      return UDR2;
    break;
    case 1 :
      while (!(UCSR1A & (1<<RXC1)));
      return UDR1;
    break;
    default:
      while (!(UCSR0A & (1<<RXC0)));
      return UDR0;
      break;
  }
#endif
} // USARTGetChar

/***************************************************************************//**
_getkey for compatibility KEIL
*******************************************************************************/
char _getkey ( void ) {
#if defined (__AVR_ATmega2560__)        // stdio sends by USART0
switch ( usart_stdio) {
	case 3 :
	while (!(UCSR3A & (1<<RXC3)));
	return UDR3;
	break;
	case 2 :
	while (!(UCSR2A & (1<<RXC2)));
	return UDR2;
	break;
	case 1 :
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1;
	break;
	default:
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
	break;
  }
  #endif
}
/**@}*/
