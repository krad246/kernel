import argparse
from parse_target_traps import *
from ls_parse import *

def main():
	parser = argparse.ArgumentParser(description="Autogenerate a trap table for an MSP430 CPU.")
	parser.add_argument('-s', "--symbols", metavar='S', type=str, help="Path to linker script for chip.")
	parser.add_argument('-r', "--reserve", nargs='*', metavar='R', type=str, help="List of reserved interrupt vectors to skip trap table generation on.")
	parser.add_argument('-d', "--destination", metavar='D', type=str, help="Path to write output file to.")
	args = parser.parse_args()

	autogen = open(args.destination, "w")

	header = \
"""
;-------------------------------------------------------------------------------
; includes
;-------------------------------------------------------------------------------

		#include \"msp430.h\"
		#include \"cpu_asm.S\"

;-------------------------------------------------------------------------------
; MSP430 interrupt handler installation
;-------------------------------------------------------------------------------

		EXTERN(cpu_trap)			; CPU_TRAP calls this
		EXTERN(cpu_panic)			; CPU_BAD_TRAP calls this
		
		.sect .handlers, \"ax\", @progbits
"""

		# Insert trap table between these...

	declarations = \
"""
;-------------------------------------------------------------------------------
; MSP430 interrupt declaration
;-------------------------------------------------------------------------------
"""

		# Insert trap definitions after this...

	autogen.write(header)

	vnums = []
	linker_data = get_linker_script_data(args.symbols)["sections"]

	for section in linker_data.items():

		if is_cpu_interrupt(section):
			vnum = interrupt_number(section)

			# if this interrupt vector is in the 'exclude' list skip it
			should_exclude = False
			for exclude in args.reserve:
				if should_exclude_interrupt(section, exclude):
					should_exclude = True
					break

			if should_exclude:
				continue

			# this is a meaningful CPU interrupt
			if is_valid_cpu_interrupt(section):

				vec_src = interrupt_source(section)
				vec_name = to_vector_name(vec_src)
				autogen.write("\t\t\tCPU_TRAP {} ; {}\n".format(vnum, vec_name))
			else:
				autogen.write("\t\t\tCPU_BAD_TRAP {}\n".format(vnum))

			vnums.append(vnum)

	# vnums = []
	# lines = []
	# line_to_parse = ''
	# found_section_block = False
	# for line in symbols:


	# 	if "SECTIONS" not in line and not found_section_block:
	# 		continue
	# 	elif "SECTIONS" in line and not found_section_block:
	# 		found_section_block = True
	# 		continue
	# 	elif "SECTIONS" not in line and found_section_block:
	# 		pass
	# 	else:
	# 		raise Exception()
		
	# 	print(line)

	# 	if linker_line_is_complete(line_to_parse):
	# 		line_to_parse = ' '.join(lines)
	# 		# print("fpo",line_to_parse)
	# 	else:
	# 		lines.append(line)
	# 		line_to_parse = ' '.join(lines)
	# 		continue



	# 		if linker_line_is_complete(line_to_parse):
	# 			line_to_parse = ''
	# 			lines = []

	autogen.write(declarations)

	for vnum in vnums:
		autogen.write("\t\t\tCPU_TRAP_DEFINE {}\n".format(vnum))

	cpu_vars = \
"""
;-------------------------------------------------------------------------------
; MSP430 interrupt declaration
;-------------------------------------------------------------------------------

		PUBLIC(g_cpu_trap_sp)			; programmable stack pointer for CPU
		PUBLIC(g_cpu_trap_stack)	; processor interrupt stack

		.data

g_cpu_trap_sp:
	.word 0

g_cpu_trap_stack:
	.space CPU_TRAP_STACK_SIZE
"""

	autogen.write(cpu_vars)
	autogen.write("\n\t\t\t.end\n")
	
if __name__ == "__main__":
	main()