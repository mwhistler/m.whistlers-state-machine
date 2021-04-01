/**
 * @file    sys_time_emu.c
 * @author  marcin.swistun@gmail.com
 * @version V1.0.0
 * @date    20/09/2020
 * @brief   System timer emulation. Returns ticks, typically 1 tick == 1ms.
*/

#include "sys_time.h"
#include <time.h>

uint64_t systime_get(void)
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return (t.tv_sec * UINT64_C(1000) + t.tv_nsec / 1000000);
}
