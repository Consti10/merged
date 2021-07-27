/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Defines functions to access functions provided by the RTOS
 *  in a generic way
 *
 */

#pragma once


/* Include the actual RTOS definitions for:
 * - wiced_timed_event_t
 * - timed_event_handler_t
 */
#include "wiced_result.h"
#include "wiced_bt_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


#define WICED_NO_WAIT       0
#define WICED_WAIT_FOREVER  ((uint32_t) 0xFFFFFFFF)
#define WICED_NEVER_TIMEOUT ((uint32_t) 0xFFFFFFFF)
#define DEPRECATE_RTOS_TIMER  __attribute__ ((deprecated("wiced_rtos_timer is deprecated, please use wiced_timer instead (see wiced_timer.h).")))

//! Minimum value for the thread priority
#define THREAD_PRIORITY_MIN            0

//! Maximum value for the thread priority
#define THREAD_PRIORITY_MAX            7

/******************************************************
 * @cond            Enumerations
 ******************************************************/

/** Wiced event flag wait option */
typedef enum
{
    WAIT_FOR_ANY_EVENT,
    WAIT_FOR_ALL_EVENTS,
} wiced_event_flags_wait_option_t;

/** Wiced delay types */
typedef enum
{
    ALLOW_THREAD_TO_SLEEP, /**<  Allow the current running thread to sleep */
    KEEP_THREAD_ACTIVE,    /**<  Keep the current running thread active */
}wiced_delay_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** Callback invoked on timer expiry  */
typedef void (*timer_handler_t)( int32_t arg );

/******************************************************
 *             Structures
 ******************************************************/

typedef struct _wiced_thread_t        wiced_thread_t        ;
typedef struct _thread_monitor_info_t thread_monitor_info_t ;
typedef struct _wiced_queue_t         wiced_queue_t         ;
typedef struct _wiced_rtos_timer_t    wiced_rtos_timer_t    ;
typedef struct _wiced_semaphore_t     wiced_semaphore_t     ;
typedef struct _wiced_mutex_t         wiced_mutex_t         ;
typedef struct _wiced_event_flags_t   wiced_event_flags_t   ;
typedef struct _wiced_worker_thread_t wiced_worker_thread_t ;
typedef struct _wiced_timed_event_t   wiced_timed_event_t   ;

typedef wiced_result_t (*event_handler_t)( void* arg );

typedef void (*wiced_thread_function_t)( uint32_t arg );

/******************************************************
 *             Function declarations
 * @endcond
 ******************************************************/

extern wiced_worker_thread_t wiced_hardware_io_worker_thread;
extern wiced_worker_thread_t wiced_networking_worker_thread;

/*****************************************************************************/
/** @defgroup rtos       RTOS
 *
 *  WICED Real-Time Operating System Functions
 */
/*****************************************************************************/


/*****************************************************************************/
/** @addtogroup threads       Threads
 *  @ingroup rtos
 *
 * Thread management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of thread
 *
 * Creates a new thread (Allocates memory for a new thread instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_thread_t*  wiced_rtos_create_thread( void );

/** Checks if there is a stackoverflow
 *
 * Verifies if the stack of the thread from which this API is invoked is corrupted
 *
 * @return    TRUE  : stack overflow
 * @return    FALSE : not stack overflow
 */
wiced_bool_t wiced_rtos_check_for_stack_overflow( void );

/** Creates a new instance of queue
 *
 * Creates a new queue (Allocates memory for a new queue instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_queue_t*  wiced_rtos_create_queue( void );

/** Creates a new instance of mutex
 *
 * Creates a new mutex (Allocates memory for a new mutex instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_mutex_t*  wiced_rtos_create_mutex( void );

/** Creates a new instance of timer
 *
 * Creates a new timer (Allocates memory for a new timer instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_rtos_timer_t*  wiced_rtos_create_timer( void ) DEPRECATE_RTOS_TIMER;

/** Creates a new instance of semaphore
 *
 * Creates a new semaphore (Allocates memory for a new semaphore instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_semaphore_t*  wiced_rtos_create_semaphore( void );

/** Creates a new instance of event flags
 *
 * Creates a new event_flag (Allocates memory for a new event flags instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_event_flags_t*  wiced_rtos_create_event_flags( void );

/** Creates a new instance of worker thread
 *
 * Creates a new worker thread (Allocates memory for a new worker thread instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_worker_thread_t* wiced_rtos_create_worker_thread(void);

/** Initializes and starts a new thread
 *
 * Initializes and starts a new thread
 *
 * @param thread     : Pointer to variable that will receive the thread handle
 * @param priority   : A priority number. Allowed values are from THREAD_PRIORITY_MIN to THREAD_PRIORITY_MAX
 * @param name       : a text name for the thread (can be null)
 * @param function   : the main thread function
 * @param stack_size : stack size for this thread
 * @param arg        : argument which will be passed to thread function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );

/** Deletes a terminated thread
 *
 * @param thread     : the handle of the thread to delete
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread );

/** Sleep for a given period of milliseconds
 *
 * Causes the current thread to sleep for AT LEAST the
 * specified number of milliseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * @param milliseconds : the time to sleep in milliseconds
 * @param delay_type : delay type to be used during the duration of the delay.
 *                     Use the delay type as ALLOW_THREAD_TO_SLEEP to allow the
 *                     current running thread to sleep or as KEEP_THREAD_ACTIVE
 *                     to keep the current running thread active
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_milliseconds( uint32_t milliseconds, wiced_delay_type_t delay_type);

/** Delay for a given period of microseconds
 *
 * Causes the current thread to block for AT LEAST the
 * specified number of microseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * NOTE: All threads with equal or lower priority than the current thread
 *       will not be able to run while the delay is occurring.
 *
 * @param microseconds : the time to delay in microseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_microseconds( uint32_t microseconds );

/** Sleeps until another thread has terminated
 *
 * Causes the current thread to sleep until the specified other thread
 * has terminated. If the processor is heavily loaded
 * with higher priority tasks, this thread may not wake until significantly
 * after the thread termination.
 *
 * @param thread : the handle of the other thread which will terminate
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread );


/** Forcibly wakes another thread
 *
 * Causes the specified thread to wake from suspension. This will usually
 * cause an error or timeout in that thread, since the task it was waiting on
 * is not complete.
 *
 * @param thread : the handle of the other thread which will be woken
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_force_awake( wiced_thread_t* thread );


/** Checks if a thread is the current thread
 *
 * Checks if a specified thread is the currently running thread
 *
 * @param thread : the handle of the other thread against which the current thread will be compared
 *
 * @return    WICED_SUCCESS : specified thread is the current thread
 * @return    WICED_ERROR   : specified thread is not currently running
 */
wiced_result_t wiced_rtos_is_current_thread( wiced_thread_t* thread );


/** Checks the stack of the current thread
 *
 * @return    WICED_SUCCESS : if the current thread stack is within limits
 * @return    WICED_ERROR   : if the current thread stack has extended beyond its limits
 */
wiced_result_t wiced_rtos_check_stack( void );

/** @} */
/*****************************************************************************/
/** @addtogroup semaphores       Semaphores
 *  @ingroup rtos
 *
 * Semaphore management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialises a semaphore
 *
 * Initialises a counting semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be initialised
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* semaphore );


/** Set (post/put/increment) a semaphore
 *
 * Set (post/put/increment) a semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore );


/** Get (wait/decrement) a semaphore
 *
 * Attempts to get (wait/decrement) a semaphore. If semaphore is at zero already,
 * then the calling thread will be suspended until another thread sets the
 * semaphore with @ref wiced_rtos_set_semaphore
 *
 * @param semaphore : a pointer to the semaphore handle
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* semaphore, uint32_t timeout_ms );


/** De-initialise a semaphore
 *
 * Deletes a semaphore created with @ref wiced_rtos_init_semaphore
 *
 * @param semaphore : a pointer to the semaphore handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_semaphore( wiced_semaphore_t* semaphore );

/** @} */
/*****************************************************************************/
/** @addtogroup mutexes       Mutexes
 *  @ingroup rtos
 *
 * Mutex management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialises a mutex
 *
 * Initialises a mutex
 * A mutex is different to a semaphore in that a thread that already holds
 * the lock on the mutex can request the lock again (nested) without causing
 * it to be suspended.
 *
 * @param mutex : a pointer to the mutex handle to be initialised
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_mutex( wiced_mutex_t* mutex );


/** Obtains the lock on a mutex
 *
 * Attempts to obtain the lock on a mutex. If the lock is already held
 * by another thead, the calling thread will be suspended until
 * the mutex lock is released by the other thread.
 *
 * @param mutex : a pointer to the mutex handle to be locked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_lock_mutex( wiced_mutex_t* mutex );


/** Releases the lock on a mutex
 *
 * Releases a currently held lock on a mutex. If another thread
 * is waiting on the mutex lock, then it will be resumed.
 *
 * @param mutex : a pointer to the mutex handle to be unlocked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_unlock_mutex( wiced_mutex_t* mutex );


/** De-initialise a mutex
 *
 * Deletes a mutex created with @ref wiced_rtos_init_mutex
 *
 * @param mutex : a pointer to the mutex handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_mutex( wiced_mutex_t* mutex );


/** @} */
/*****************************************************************************/
/** @addtogroup queues       Queues
 *  @ingroup rtos
 *
 * Queue management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialises a queue
 *
 * Initialises a FIFO queue
 *
 * @param queue : a pointer to the queue handle to be initialised
 * @param name  : a text string name for the queue (NULL is allowed)
 * @param message_size : size in bytes of objects that will be held in the queue
 * @param number_of_messages : depth of the queue - i.e. max number of objects in the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_queue( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );


/** Pushes an object onto a queue
 *
 * Pushes an object onto a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : the object to be added to the queue. Size is assumed to be
 *                  the size specified in @ref wiced_rtos_init_queue
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error or timeout occurred
 */
wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


/** Pops an object off a queue
 *
 * Pops an object off a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : pointer to a buffer that will receive the object being
 *                  popped off the queue. Size is assumed to be
 *                  the size specified in @ref wiced_rtos_init_queue , hence
 *                  you must ensure the buffer is long enough or memory
 *                  corruption will result
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error or timeout occurred
 */
wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


/** De-initialise a queue
 *
 * Deletes a queue created with @ref wiced_rtos_init_queue
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_queue( wiced_queue_t* queue );


/** Check if a queue is empty
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is empty.
 * @return    WICED_ERROR   : queue is not empty.
 */
wiced_bool_t wiced_rtos_is_queue_empty( wiced_queue_t* queue );


/** Check if a queue is full
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is full.
 * @return    WICED_ERROR   : queue is not full.
 */
wiced_bool_t wiced_rtos_is_queue_full( wiced_queue_t* queue );


/** Get the queue occupancy
 *
 * @param queue : a pointer to the queue handle
 * @param count : pointer to integer for storing occupancy count
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_queue_occupancy( wiced_queue_t* queue, uint32_t *count );


/** @} */
/*****************************************************************************/
/** @addtogroup rtostmr       RTOS timers
 *  @ingroup rtos
 *
 * RTOS timer management functions
 * These timers are based on the RTOS time-slice scheduling, so are not
 * highly accurate. They are also affected by high loading on the processor.
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialises a RTOS timer
 *
 * Initialises a RTOS timer
 * Timer does not start running until @ref wiced_rtos_start_timer is called
 *
 * @param timer    : a pointer to the timer handle to be initialised
 * @param time_ms  : timeout value in milliseconds
 * @param function : the callback handler function that is called each
 *                   time the timer expires
 * @param arg      : an argument that will be passed to the callback
 *                   function
 *
 * @return
 */
wiced_result_t wiced_rtos_init_timer( wiced_rtos_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg ) DEPRECATE_RTOS_TIMER;


/** Starts a RTOS timer running
 *
 * Starts a RTOS timer running. Timer must have been previously
 * initialised with @ref wiced_rtos_init_timer
 *
 * @param timer    : a pointer to the timer handle to start
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_start_timer( wiced_rtos_timer_t* timer ) DEPRECATE_RTOS_TIMER;


/** Stops a running RTOS timer
 *
 * Stops a running RTOS timer. Timer must have been previously
 * started with @ref wiced_rtos_start_timer
 *
 * @param timer    : a pointer to the timer handle to stop
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_stop_timer( wiced_rtos_timer_t* timer ) DEPRECATE_RTOS_TIMER;


/** De-initialise a RTOS timer
 *
 * Deletes a RTOS timer created with @ref wiced_rtos_init_timer
 *
 * @param timer : a pointer to the RTOS timer handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_timer( wiced_rtos_timer_t* timer ) DEPRECATE_RTOS_TIMER;


/** Check if an RTOS timer is running
 *
 * @param timer : a pointer to the RTOS timer handle
 *
 * @return    WICED_SUCCESS : if running.
 * @return    WICED_ERROR   : if not running
 */
wiced_result_t wiced_rtos_is_timer_running( wiced_rtos_timer_t* timer ) DEPRECATE_RTOS_TIMER;

/** @} */
/*****************************************************************************/
/** @addtogroup worker       Worker Threads
 *  @ingroup rtos
 *
 * Worker thread management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a worker thread
 *
 * Creates a worker thread
 * A worker thread is a thread in whose context timed and asynchronous events
 * execute.
 *
 * @param worker_thread    : a pointer to the worker thread to be created
 * @param priority         : thread priority
 * @param stack_size       : thread's stack size in number of bytes
 * @param event_queue_size : number of events can be pushed into the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );


/** Deletes a worker thread
 *
 * Deletes a worker thread
 *
 * @param worker_thread : a pointer to the worker thread to be created
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delete_worker_thread( wiced_worker_thread_t* worker_thread );

/** @} */
/*****************************************************************************/
/** @addtogroup events       Events
 *  @ingroup rtos
 *
 * Event management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Requests a function be called at a regular interval
 *
 * This function registers a function that will be called at a regular
 * interval. Since this is based on the RTOS time-slice scheduling, the
 * accuracy is not high, and is affected by processor load.
 *
 * @param event_object  : pointer to a event handle which will be initialised
 * @param worker_thread : pointer to the worker thread in whose context the
 *                        callback function runs on
 * @param function      : the callback function that is to be called regularly
 * @param time_ms       : the time period between function calls in milliseconds
 * @param arg           : an argument that will be supplied to the function when
 *                        it is called
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_register_timed_event( wiced_timed_event_t* event_object, wiced_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg );


/** Removes a request for a regular function execution
 *
 * This function de-registers a function that has previously been set-up
 * with @ref wiced_rtos_register_timed_event.
 *
 * @param event_object : the event handle used with @ref wiced_rtos_register_timed_event
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deregister_timed_event( wiced_timed_event_t* event_object );


/** Sends an asynchronous event to the associated worker thread
 *
 * Sends an asynchronous event to the associated worker thread
 *
 * @param worker_thread :the worker thread in which context the callback should execute from
 * @param function      : the callback function to be called from the worker thread
 * @param arg           : the argument to be passed to the callback function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg );

/** @} */
/*****************************************************************************/
/** @addtogroup eventflags       Event Flags
 *  @ingroup rtos
 *
 * Event flags management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialise an event flags
 *
 * @param event_flags : a pointer to the event flags handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_event_flags( wiced_event_flags_t* event_flags );


/** Wait for event flags to be set
 *
 * @param event_flags        : a pointer to the event flags handle
 * @param flags_to_wait_for  : a group of event flags (ORed bit-fields) to wait for
 * @param flags_set          : event flag(s) set
 * @param clear_set_flags    : option to clear set flag(s)
 * @param wait_option        : wait option
 * @param timeout_ms         : timeout in milliseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_wait_for_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms );


/** Set event flags
 *
 * @param event_flags  : a pointer to the event flags handle
 * @param flags_to_set : a group of event flags (ORed bit-fields) to set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_set );


/** De-initialise an event flags
 *
 * @param event_flags : a pointer to the event flags handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_event_flags( wiced_event_flags_t* event_flags );

/** @} */


#ifdef __cplusplus
} /*extern "C" */
#endif
