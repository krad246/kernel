/*
 * preempt.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include "c_traps.h"
#include "attributes.h"
//#include "thread.h"
#include "timer.h"

// TODO: alias this to k_require_context_switch
bool cpu_context_switch_requested(void)
{
    // TODO: if it's a timeout fault then this is always true, else use the state of the variable
    return true;
}

// TODO: alias this to k_context_switch
void cpu_context_switch(void)
{

}

void k_timer_init(unsigned int freq)
{
	//timer_init()
}
