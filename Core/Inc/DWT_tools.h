/*
 * DWT_tools.h
 *
 *  Created on: 25.03.2018
 *      Author: Pawel Rogalinski
 */

#ifndef DWT_TOOLS_H_
#define DWT_TOOLS_H_

#ifdef __cplusplus
extern "C"
{
#endif

	/*
 * DWT (Data watchpoint trigger) jest to sprz�towy licznik zegarowy,
 * kt�ry zlicza ilo�� cykli wykonanych przez CPU.
 * Mo�na go u�y� do bardzo dok�adnego pomiaru czasu wykonywania
 * fragment�w kodu lub do generowania op�nie� na poziomie mikrosekund.
 * UWAGA: Licznik dzia�a tylko w trybie debuggowania.
 * Poni�sze funkcje mog� byc zak��cone przez u�ycie debuggera
 * lib innych narz�dzi do optymalizacji kodu programu (code profilers)
 */

#include "stm32f1xx.h"

	extern uint32_t DWT_CYCCNT_OnStart;

	static inline void DWT_Init()
	{
		CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CTRL &= ~1;
		DWT->CTRL |= 1;
	}

	static inline void DWT_Delay_us(volatile uint32_t micros)
	{
		volatile uint32_t start = DWT->CYCCNT;
		micros *= (HAL_RCC_GetHCLKFreq() / 1000000);
		while ((DWT->CYCCNT - start) < micros)
			;
	}

	static inline uint32_t DWT_Counter()
	{
		return DWT->CYCCNT;
	}

	static inline void DWT_SetStart()
	{
		DWT_CYCCNT_OnStart = DWT->CYCCNT;
	}

	static inline uint32_t DWT_GetCycles()
	{
		return DWT->CYCCNT - DWT_CYCCNT_OnStart;
	}

	static inline uint32_t DWT_GetMicroseconds()
	{
		return (DWT->CYCCNT - DWT_CYCCNT_OnStart) / (HAL_RCC_GetHCLKFreq() / 1000000);
	}

#ifdef __cplusplus
}
#endif

#endif /* DWT_TOOLS_H_ */
