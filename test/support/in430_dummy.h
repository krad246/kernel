/*******************************************************************************
 *  in430.h -
 *
 *  Copyright (C) 2003-2020 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/* 1.210 */

#ifndef __IN430_H__
#define __IN430_H__

#include "msp430_dummy.h"

/* Definitions for projects using the GNU C/C++ compiler */
#if !defined(__ASSEMBLER__)

/* Definitions of things which are intrinsics with IAR and CCS, but which don't 
   appear to be intrinsics with the GCC compiler */

/* The data type used to hold interrupt state */
typedef unsigned int __istate_t;

void _no_operation(void);

unsigned int _get_interrupt_state(void);
void _set_interrupt_state(unsigned int x);

void _enable_interrupts(void);
void _disable_interrupts(void);

void _bis_SR_register(unsigned int x);
void _bic_SR_register(unsigned int x);

void __bic_SR_register_on_exit(unsigned int x);
#define _bic_SR_register_on_exit(x) __bic_SR_register_on_exit(x)
void __bis_SR_register_on_exit(unsigned int x);
#define _bis_SR_register_on_exit(x) __bis_SR_register_on_exit(x)

unsigned int _get_SR_register(void);

void *_get_SP_register(void);
#define __get_SP_register() _get_SP_register()

void _set_SP_register(void *x);
#define __set_SP_register(x) _set_SP_register(x)

unsigned int _swap_bytes(unsigned int x);

unsigned short _bcd_add_short(unsigned short x, unsigned short y);
#define __bcd_add_short(x, y) _bcd_add_short(x, y)
unsigned long _bcd_add_long(unsigned long x, unsigned long y);
#define __bcd_add_long(x, y) _bcd_add_long(x, y)

void _data16_write_addr(void *addr, unsigned int src);
#define __data16_write_addr(addr, src) _data16_write_addr(addr, src)

unsigned int _data16_read_addr(void *addr, unsigned int src);
#define __data16_read_addr(addr) _data16_read_addr(addr)

void _data20_write_char(void *addr, unsigned char src);
#define __data20_write_char(addr, src) _data20_write_char(addr, src)

unsigned char _data20_read_char(void *addr);
#define __data20_read_char(addr) _data20_read_char(addr)

void _data20_write_short(void *addr, unsigned short src);
#define __data20_write_short(addr, src) _data20_write_short(addr, src)

void _data20_read_short(void *addr);
#define __data20_read_short(addr) _data20_read_short(addr)

void _data20_write_long(void *addr, unsigned long src);
#define __data20_write_long(addr, src) _data20_write_long(addr, src)

unsigned long _data20_read_long(void *addr);
#define __data20_read_long(addr) _data20_read_long(addr)

#define _low_power_mode_0() _bis_SR_register(LPM0_bits | GIE)
#define _low_power_mode_1() _bis_SR_register(LPM1_bits | GIE)
#define _low_power_mode_2() _bis_SR_register(LPM2_bits | GIE)
#define _low_power_mode_3() _bis_SR_register(LPM3_bits | GIE)
#define _low_power_mode_4() _bis_SR_register(LPM4_bits | GIE)
#define _low_power_mode_off_on_exit() _bic_SR_register_on_exit(LPM4_bits)

#define __low_power_mode_0() _low_power_mode_0()
#define __low_power_mode_1() _low_power_mode_1()
#define __low_power_mode_2() _low_power_mode_2()
#define __low_power_mode_3() _low_power_mode_3()
#define __low_power_mode_4() _low_power_mode_4()
#define __low_power_mode_off_on_exit() _low_power_mode_off_on_exit()

#define _even_in_range(x, y) (x)
#define __even_in_range(x, y) _even_in_range(x, y)

#define __no_operation() _no_operation()

#define __get_interrupt_state() _get_interrupt_state()
#define __set_interrupt_state(x) _set_interrupt_state(x)
#define __enable_interrupt() _enable_interrupts()
#define __disable_interrupt() _disable_interrupts()

#define __bic_SR_register(x) _bic_SR_register(x)
#define __bis_SR_register(x) _bis_SR_register(x)
#define __get_SR_register() _get_SR_register()

#define __swap_bytes(x) _swap_bytes(x)

#define __nop() _no_operation()

#define __eint() _enable_interrupts()
#define __dint() _disable_interrupts()

#define _NOP() _no_operation()
#define _EINT() _enable_interrupts()
#define _DINT() _disable_interrupts()

#define _BIC_SR(x) _bic_SR_register(x)
#define _BIC_SR_IRQ(x) _bic_SR_register_on_exit(x)
#define _BIS_SR(x) _bis_SR_register(x)
#define _BIS_SR_IRQ(x) _bis_SR_register_on_exit(x)
#define _BIS_NMI_IE1(x) _bis_nmi_ie1(x)

#define _SWAP_BYTES(x) _swap_bytes(x)

#define __no_init __attribute__((noinit))

#endif /* !defined _GNU_ASSEMBLER_ */

#endif /* __IN430_H__ */
