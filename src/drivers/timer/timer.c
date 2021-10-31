/*
 * timer.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

// TODO: autogenerate the timer memory map from the symbols linker script

typedef struct
{

} timer_t;

// create an enum from all the timerAs available

//int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
//{
//    /* using fixed TIMER_BASE for now */
//    if (dev != 0) { // TODO: autogenerate the enum from the symbols
//        return -1;
//    }
//
//    /* TODO: configure time-base depending on freq value */
//    if (freq != 1000000ul) {
//        return -1;
//    }
//
//    /* reset the timer A configuration */
//    TIMER_BASE->CTL = TIMER_CTL_CLR;
//
//    /* save callback */
//    isr_cb = cb;
//    isr_arg = arg;
//
//    /* configure timer to use the SMCLK with prescaler of 8 */
//    TIMER_BASE->CTL = (TIMER_CTL_TASSEL_SMCLK | TIMER_CTL_ID_DIV8);
//
//    /* configure CC channels */
//    for (int i = 0; i < TIMER_CHAN; i++) {
//        TIMER_BASE->CCTL[i] = 0;
//    }
//    /* start the timer in continuous mode */
//    TIMER_BASE->CTL |= TIMER_CTL_MC_CONT;
//    return 0;
//}
//
//int timer_set_absolute(tim_t dev, int channel, unsigned int value)
//{
//    if (dev != 0 || channel >= TIMER_CHAN) {
//        return -1;
//    }
//    TIMER_BASE->CCR[channel] = value;
//    TIMER_BASE->CCTL[channel] &= ~(TIMER_CCTL_CCIFG);
//    TIMER_BASE->CCTL[channel] |=  (TIMER_CCTL_CCIE);
//    return 0;
//}
//
//int timer_clear(tim_t dev, int channel)
//{
//    if (dev != 0 || channel >= TIMER_CHAN) {
//        return -1;
//    }
//    TIMER_BASE->CCTL[channel] &= ~(TIMER_CCTL_CCIE);
//    return 0;
//}
//
//unsigned int timer_read(tim_t dev)
//{
//    (void)dev;
//    return (unsigned int)TIMER_BASE->R;
//}
//
//void timer_start(tim_t dev)
//{
//    (void)dev;
//    TIMER_BASE->CTL |= TIMER_CTL_MC_CONT;
//}
//
//void timer_stop(tim_t dev)
//{
//    (void)dev;
//    TIMER_BASE->CTL &= ~(TIMER_CTL_MC_MASK);
//}
