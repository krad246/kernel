import argparse
from parse_target_traps import *
from ls_parse import *

def main():
	parser = argparse.ArgumentParser(description="Autogenerate a trap table for an MSP430 CPU.")
	parser.add_argument('-s', "--symbols", metavar='S', type=str, help="Path to linker script for chip.")
	parser.add_argument('-r', "--reserve", nargs='*', metavar='R', type=str, help="List of reserved interrupt vectors to skip trap table generation on.")
	parser.add_argument('-d', "--destination", metavar='D', type=str, help="Path to write output file to.")
	parser.add_argument('-k', '--kernel', default=False, action='store_true', help="Whether to generate a kernel header or a system header.")
	args = parser.parse_args()

	autogen = open(args.destination, "w")

	k_prefix = "K_" if args.kernel else ""
	header = \
"""
#ifndef {}TRAP_DEFS_H_
#define {}TRAP_DEFS_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"
#ifndef TEST
	#include <msp430.h>
#else 
	#include "msp430_dummy.h"
#endif

""".format(k_prefix, k_prefix)

	defines = \
"""
/*******************************************************************************
 * defines
 ******************************************************************************/"""

	autogen.write(header)
	autogen.write(defines)

	first_valid_int = None

	vnums = []
	linker_data = get_linker_script_data(args.symbols)["sections"]

	vnums = []
	reserved = []
	for section in linker_data.items():

		if is_cpu_interrupt(section):

			vnum = interrupt_number(section)

			# this is a meaningful CPU interrupt
			if is_valid_cpu_interrupt(section):
				if first_valid_int == None:
					first_valid_int = vnum
				
				# if this interrupt vector is in the 'exclude' list skip it
				should_exclude = False
				for exclude in args.reserve:
					if should_exclude_interrupt(section, exclude):
						should_exclude = True
						break

				if should_exclude:
					reserved.append((vnum, interrupt_source(section)))
					continue

			vnums.append(vnum)

	if args.kernel:
		autogen.write("\n")

		autogen.write("#if defined(TRAP_C_)\n")

		autogen.write("\n")
		autogen.write("#define K_MIN_VALID_TRAP_NUM ({})\n".format(first_valid_int))
		autogen.write("#define K_MAX_VALID_TRAP_NUM ({})\n".format(vnums[-1]))
		autogen.write("#define K_ILL_TRAP ({})\n".format(vnums[-1] + 1))

		autogen.write("\n")
		autogen.write("STATIC const unsigned int g_kern_rsvd_traps[] = \n")
		
		autogen.write("{\n")
		for reserve in reserved:
			vnum, vsrc = reserve[0], reserve[1]
			autogen.write("\t{}, /* {} */\n".format(to_vector_name(vsrc), vnum))
			
		autogen.write("};\n")

		autogen.write("\n")

		autogen.write("#endif")
	else:
		autogen.write("\n")

		autogen.write("#if defined(C_TRAPS_C_) || defined(TRAP_C_)\n")
		autogen.write("#define CPU_N_TRAPS ({})\n".format(len(vnums)))
		autogen.write("#endif\n")

	autogen.write("\n")
	autogen.write("#endif\n")

if __name__ == "__main__":
	main()