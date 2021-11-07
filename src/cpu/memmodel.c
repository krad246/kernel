#define MEMMODEL_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "memmodel.h"
#include "memmodel_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * public functions
 ******************************************************************************/
void cpu_init_hw_stackframe(cpu_hw_context_t *ctx, void *pc, bool trap_en)
{
	uintptr_t pc_bits = (uintptr_t)pc;

	#ifdef __MSP430X_LARGE__
		if (trap_en)
		{
			ctx->pc_plus_sr = CPU_SR_BITS(GIE) | CPU_PC_HIGH_BITS(pc_bits);
			ctx->pc_low_bits = CPU_PC_LOW_BITS(pc_bits);
		}
		else
		{
			ctx->pc_plus_sr = CPU_PC_HIGH_BITS(pc_bits);
			ctx->pc_low_bits = CPU_PC_LOW_BITS(pc_bits);
		}
	#else
		if (trap_en)
		{
			ctx->pc = CPU_PC_LOW_BITS(pc_bits);
			ctx->sr = CPU_SR_BITS(GIE);
		}
		else
		{
			ctx->pc = CPU_PC_LOW_BITS(pc_bits);
			ctx->sr = 0;
		}
	#endif
}

void cpu_init_sw_stackframe(cpu_sw_context_t *ctx, cpu_reg_t r12)
{
	memset(ctx, 0, sizeof(cpu_sw_context_t));
	ctx->r12 = r12; /* provide 1 arg for the calling convention */
}
