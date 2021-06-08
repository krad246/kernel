/*
 * preempt.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include "c_traps.h"
#include "attributes.h"
//#include "thread.h"
// #include "timer.h"
#include "utils.h"
#include <msp430.h>

#if 0
#define K_TIMER_BASE JOIN(CONFIG_K_TIMER, _BASE)

#define K_IS_ONLY_THREAD(x)         ({ ! cpu_context_switch_requested() && k_sched_jobs_avail(); })
#define K_THREAD_IS_DEAD(x)         ({ !!((x).flags & K_THREAD_DEAD); })
#define K_THREAD_IS_PAUSED(x)       ({ !!((x).flags & K_THREAD_SUSPENDED); })
#define K_THREAD_IS_PREEMPTIBLE(x)  ({ !!((x).flags & K_THREAD_PREEMPTIBLE); })

// TODO: alias this to k_require_context_switch
bool cpu_context_switch_requested(void)
{
    return g_kern_sched.curr_thread != g_kern_sched.next_thread;
}

// TODO: alias this to k_context_switch
void cpu_context_switch(void)
{
    k_thread_attr_t attr;
    k_sched_job_query(&g_kern_sched.curr_thread, &attr);

    bool must_switch = false;

    if (K_THREAD_IS_PREEMPTIBLE(attr))
    {
        must_switch = true;
    } else
    {
        /* we shouldn't even bother using interrupts here
         * so we should disable time slicing until explicitly
         * reenabled in the calling thread */
    }

    if (must_switch)
    {

        bool dead = K_THREAD_IS_DEAD(attr);
        bool paused = K_THREAD_IS_PAUSED(attr);

        if (dead)
        {
            /* clean me up */
            free(g_kern_sched.current_thread->stack_mem);
            free(g_kern_sched.current_thread);
        }

        /* check the scheduling queue for any runnable jobs */
        if (! k_sched_jobs_avail())
        {
            /*
             * the last job was deleted or suspended and
             * the run pointers MUST be pointing at the
             * same thread - this is the last thread before IDLE
             */

            if (K_IS_ONLY_THREAD(g_kern_sched) && dead)
            {
                g_kern_sched.curr_thread = g_kern_sched.idle;
                g_kern_sched.next_thread = g_kern_sched.idle;
            }

            if (K_IS_ONLY_THREAD(g_kern_sched) && paused)
            {
                g_kern_sched.curr_thread = g_kern_sched.idle;
                g_kern_sched.next_thread = g_kern_sched.idle;
            }

        } else
        {
            /* triggered ONLY when we are idling with jobs available */
            if (K_IS_ONLY_THREAD(g_kern_sched))
            {

                /* pick the first added one and restart the scheduling from that point */
                g_kern_sched.current_thread = clist_lpeek()

            } else
            {
                g_kern_sched.current_thread = g_kern_sched.next_thread;
            }

            k_sched_job_next();
        }

        cpu_context_switch_requested = false;
    }
}

void k_timer_init(unsigned int freq)
{
	//timer_init()
}
#endif
