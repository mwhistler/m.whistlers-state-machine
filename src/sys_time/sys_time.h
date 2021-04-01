/**
 * @file    sys_time.h
 * @author  marcin.swistun@gmail.com
 * @version V1.0.0
 * @date    20/09/2020
 * @brief   Plik naglowkowy modulu milisekundowego zegara systemowego
*/

#ifndef SYS_TIME_H_
#define SYS_TIME_H_

#include <stdint.h>

#define SYSTICKS_PER_MS (1)

#define MS_TO_SYSTICKS(ms) ((int64_t)(((int64_t)(ms)) * SYSTICKS_PER_MS))

/**
 * System time get.
 * @return system time in [ms]
 */
uint64_t systime_get(void);


#endif /* SYS_TIME_H_ */
