/***************************************************************************//**
Implementation file file for a simple wait routine (waits in ms).

@author LiTec/PAMG
@date 10.01.2010
@version 1.0

@param compiler switches: none
@param linker switches: none

@b History:
- 1.0 LiTec/PAMG 10.01.2010 initial version.
*******************************************************************************/
/** @addtogroup WAIT_api
@{*/
/** @file Wait.c Main source file for a simple wait routine. */
#include "Wait.h"

#if defined (__AVR_ATmega2560__)
/***************************************************************************//**
Function for a simple wait based on the AVR-Lib (delay)
@note the time is dependent on the processor clock! -> define F_CPU=16000000UL
@param msec time to wait in ms
@retval - none
*******************************************************************************/
void WaitMs ( uint32_t msec ) {
#ifndef DEBUG_WITHOUT_WAIT
  uint32_t i; // counter
  for (i=0; i<msec; i++) {
  	_delay_ms ( 1 );
  }
#endif
}

#elif defined (__SAM3X8E__)
/** Global g_ul_ms_ticks in milliseconds since start of application */
volatile uint32_t g_ul_ms_ticks = 0;

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 *  \brief Handler for System Tick interrupt.
 *
 *  Process System Tick Event
 *  Increments the g_ul_ms_ticks counter.
 */
void SysTick_Handler(void) {
	g_ul_ms_ticks++;
}

/**
 *  Waits for the given number of milliseconds (using the g_ul_ms_ticks generated
 *  by the SAM's microcontrollers's system tick).
 *  \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
void WaitMs(uint32_t ul_dly_ticks) {
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif                                  // device

/**@}*/
