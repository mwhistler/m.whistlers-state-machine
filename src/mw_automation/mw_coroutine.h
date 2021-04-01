/**
 * @file    mw_coroutine.h
 * @author  marcin.swistun@gmail.com
 * @version V1.0.0
 * @date    26/03/2021
 * @brief   M.Whistler's coroutine generation macros.
 * 			Uses Duff's device like mechanism - switch-case is used to jump to prevoius return point.
 * 			mw_scheduler must be included by unit which is using mw_coroutine and must be running.
 *
 * 			USE EXAMPLE:

			create_coroutine(example_coroutine, ec_semaphore);

			uint32_t file_copy(void)
			{
				static int32_t some_result = 0;	// static or global variables recommended

				begin_coroutine()				// coroutine section opening statement

				do
				{
					function_a(ec_semaphore_setter_callback);	// call long-lasting function and give it semaphore setter as callback
					wait(fc_semaphore);							// exit coroutine

					if (fc_semaphore_value == ERROR)			// when callback is called and semaphore set routine resumes here
					{
						some_result = -1;
						finalize(ecc);							// skips the rest of routine and goes to finally{} section
					}
					wait_ms(example_coroutine, 100);			// just wait 100ms (exits and resumes from next line in 100ms)

					function_b(ec_semaphore_setter_callback);	// another long-lasting call with callback
					wait(semaphore);							// another exit

				} while(some_statement); 						// resumes here and loops

				end_coroutine()
				some_result = 0;

				finally(ecc)
				{
					fclose(...);
				}
				return 0;
			}
 *
*/


#ifndef MW_AUTOMATION_MW_COROUTINE_H_
#define MW_AUTOMATION_MW_COROUTINE_H_


#define create_coroutine(routine_name, semaphore_name)\
\
static uint32_t routine_name(void);\
\
static int32_t semaphore_name = 0;\
static int32_t semaphore_name##_value = 0;\
\
static int32_t semaphore_name##_setter_callback(int32_t value)\
{\
	semaphore_name = 1;\
	semaphore_name##_value = value;\
	schedule(routine_name, SCHEDULE_MODE_MULTI_SCHEDULE, 0);\
	return 0;\
}\
\

/* following macros are switch-case elements */

#define begin_coroutine()\
static int32_t a = 0;\
	switch (a)\
	{\
	case 0:\

#define wait_ms(routine_name, ms)\
		do {\
		a = __LINE__;\
		schedule(routine_name, SCHEDULE_MODE_MULTI_SCHEDULE, ms);\
		return 0;\
	case (__LINE__):;\
		} while(0)\

#define wait(semaphore_name)\
		do {\
		a = __LINE__;\
		semaphore_name = 0;\
		return 0;\
	case (__LINE__):\
		semaphore_name = 0;\
		} while(0)\

#define end_coroutine()\
	}\
	a = 0;\

/* the way to exit coroutine without further troubles */
#define exit_coroutine() do { a = 0; return 0;} while(0)

/* finally section - way to exit routine doing some cleaning etc. */
#define finally(prefix)\
	prefix##_finally:\
	a = 0;

#define finalize(prefix) goto prefix##_finally

#endif /* MW_AUTOMATION_MW_COROUTINE_H_ */
