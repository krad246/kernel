
/*******************************************************************************
 * includes
 ******************************************************************************/
#include <msp430.h>
#include <stddef.h>

#include "critical.h"
#include "trap.h"

// extern int app_main(void *arg);

// thread returning from function should trigger an exit hook from a planted return address in its stack
// kernel allocates a little overhead bytes such that the request is how much you get
// stack checker is a compile time setting or something - start ram with an initial pattern - the high watermark is found with a search
// stick to weighted round robin for now
// use custom memory allocator as the basis for both kernel and userspace malloc / userspace malloc restricted per thread
// create semaphores, mutexes, other basic primitivess with priority inheritance protocol
int count = 0;

void timer0_a1_handler(unsigned int inum, void *arg)
{
    P1OUT ^= BIT0;
    __delay_cycles(100000);
    count++;
    if (count == 10)
    {
        k_trap_disconnect(TIMER0_A1_VECTOR);
        TA0CTL = 0;
    }
}

// TODO: delete trap_def_headers

int main(void)
{
    cpu_enter_critical();

    P1DIR |= BIT0;PM5CTL0 &= ~LOCKLPM5;

    k_traps_init();

    //wdog_init(PERIOD)
    //timer_init(kernel_preemption_timer)
    //uart_init(uart0)
    //logger_init

	// initialize bsp
    // set up watchdog timer with software monitoring - the kernel preemption timer will reload the watchdog instead of auto-reset
    // set up the first uart for logging

	// initialize debug utilities - spawn the logger thread or whatever

	// print out some useful stuff
	// boot kernel while printing stuff out
	// (void) app_main(0); -- should be the root task

    k_trap_connect(TIMER0_A1_VECTOR, timer0_a1_handler, NULL);

    cpu_exit_critical();

    TA0CTL |= TAIE | TAIFG;

    return 0;
}

// bsp needs flexible RAW and scheduled interrupts setup - done at compile time
// bsp
// debug
// port
