#ifndef _EVENT_H_
#define _EVENT_H_

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

/** invalid event definition TODO: redefina esta macro a su criterio */
#define INVALID_EVENT NULL

/*==================[typedef]================================================*/

/** generic type for events TODO: redefina este tipo a su criterio */
typedef void * os_event_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

os_event_t os_event_init(void);
int32_t os_event_wait(os_event_t ev);
int32_t os_event_set(os_event_t ev);

/*==================[end of file]============================================*/
#endif /* #ifndef _EVENT_H_ */
