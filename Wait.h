/***************************************************************************//**
Interface file file for a simple wait routine (waits in ms).
*******************************************************************************/
/** @addtogroup WAIT_api
@{*/
/** @file Wait.h Interface file for a simple wait routine. */
#ifndef _WAIT_H
#define _WAIT_H

#include <stdint.h>

#if defined (__AVR_ATmega2560__)
  #include <util/delay.h>
#elif defined (__SAM3X8E__)
  #include "asf.h"
#endif


#ifdef __cplusplus
extern "C"{
#endif

void WaitMs ( uint32_t msec );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _WAIT_H
/**@}*/
