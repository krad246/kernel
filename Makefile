#-------------------------------------------------------------------------------
# Build parameters
#-------------------------------------------------------------------------------
CONFIG_MCU					:= msp430fr5994
CONFIG_K_TIMER				:= timer0_a0 
CONFIG_K_UART				:= usci_a0
CONFIG_K_TRAP_STACK_SIZE 	:= 128

#-------------------------------------------------------------------------------
# Toolchain definitions
#-------------------------------------------------------------------------------
CC				:= msp430-elf-gcc
CXX				:= msp430-elf-g++	
AR				:= msp430-elf-ar
NM				:= msp430-elf-nm
OBJCOPY			:= msp430-elf-objcopy
OBJDUMP			:= msp430-elf-objdump
RANLIB			:= msp430-elf-ranlib
STRIP			:= msp430-elf-strip
SIZE			:= msp430-elf-size
READELF			:= msp430-elf-readelf

GDB				:= msp430-elf-gdb
MSPDEBUG		:= mspdebug

#-------------------------------------------------------------------------------
# Build defines
#-------------------------------------------------------------------------------
SRCDIR		  	:= src
BINDIR			:= bin
OBJDIR		  	:= obj
INCDIR 			:= include
PREPROCDIR		:= preproc
AUTOGENDIR		:= $(SRCDIR)/autogen
INCDIRS			:= include $(shell find src -type d)

BINARY 			:= kernel
APP  			:= $(BINDIR)/$(BINARY)-$(CONFIG_MCU)

#-------------------------------------------------------------------------------
# Build file extensions
#-------------------------------------------------------------------------------
C_EXTS   		:= .c .cpp .cc
ASM_EXTS		:= .S

#-------------------------------------------------------------------------------
# Cross compiler flags
#-------------------------------------------------------------------------------
OPT_FLAGS		= -O2 -g -g3 -gdwarf-3 -ggdb -gstrict-dwarf
WARN_FLAGS		= -Wall -Wunused -fverbose-asm
ERRATA_FLAGS	= cpu4,cpu8,cpu11,cpu12,cpu13,cpu19
SIZE_FLAGS		= -ffreestanding -nostartfiles -mtiny-printf -msmall
STYLE_FLAGS		= -masm-hex -ffunction-sections -fdata-sections

define k_device_name
$(shell echo 
	$(shell echo $(strip $1) | cut -d '_' -f1 | sed 's/[0-9]//g')_$(shell echo $(strip $1) | cut -d '_' -f2) | 
	tr '[:lower:]' '[:upper:]')
endef

define k_vector_name
$(shell echo $(strip $1)_VECTOR | tr '[:lower:]' '[:upper:]')
endef

#TODO restrict this define to the scope of kernel code only 
CFLAGS			:= -mmcu=$(CONFIG_MCU) $(OPT_FLAGS) $(WARN_FLAGS) \
					-msilicon-errata-warn=$(ERRATA_FLAGS)  \
					$(SIZE_FLAGS) $(STYLE_FLAGS) \
					$(addprefix -I, $(INCDIRS)) -MMD -MP \
					-DCONFIG_K_TIMER=$(call k_device_name, $(CONFIG_K_TIMER)) \
					-DCONFIG_K_TIMER_VECTOR=$(call k_vector_name, $(CONFIG_K_TIMER)) \
					-DCONFIG_K_UART=$(call k_device_name, $(CONFIG_K_UART)) \
					-DCONFIG_K_TRAP_STACK_SIZE=$(CONFIG_K_TRAP_STACK_SIZE)
					
ASFLAGS			:= $(CFLAGS) -x assembler-with-cpp -Wa,-mP -Wa,-mn
LDFLAGS			:= -flto -Wl,-Map=$(APP).map -Wl,--gc-sections -Wl,--no-relax

#-------------------------------------------------------------------------------
# The magic
#-------------------------------------------------------------------------------
DIRS 			:= $(shell find $(SRCDIR) -type d -print)

C_SRC 			:= $(foreach DIR, $(DIRS), \
						$(foreach EXT, $(C_EXTS), \
							$(wildcard $(DIR)/*$(EXT))))

ASM_SRC			:= $(foreach DIR, $(DIRS), \
						$(foreach EXT, $(ASM_EXTS), \
							$(wildcard $(DIR)/*$(EXT))))

K_TRAP_TBL		:= $(SRCDIR)/cpu/traps.S
K_TRAP_KRNL_HDR	:= $(SRCDIR)/kernel/trap_defs.h
K_TRAP_HW_HDR	:= $(SRCDIR)/cpu/trap_defs.h 
K_TRAP_DEFS		:= $(K_TRAP_HW_HDR) $(K_TRAP_KRNL_HDR)
AUTOGEN_SRC		:= $(K_TRAP_TBL) $(K_TRAP_DEFS)

SRCS			:= $(C_SRC) $(ASM_SRC) $(AUTOGEN_SRC)

PREPROCS		:= $(addsuffix .i, $(addprefix $(PREPROCDIR)/, $(SRCS)))

DEPS 			:= $(patsubst %.o, %.d, $(OBJS))

C_OBJ 			:= $(addsuffix .o, $(addprefix $(OBJDIR)/, $(C_SRC)))
ASM_OBJ			:= $(addsuffix .o, $(addprefix $(OBJDIR)/, $(ASM_SRC)))
AUTOGEN_OBJS	:= $(addsuffix .o, \
						$(addprefix $(OBJDIR)/, \
							$(filter \
								$(addprefix %, $(C_EXTS) $(ASM_EXTS)), \
												$(AUTOGEN_SRC))))

OBJS			:= $(C_OBJ) $(ASM_OBJ) $(AUTOGEN_OBJS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

#-------------------------------------------------------------------------------
# Targets
#-------------------------------------------------------------------------------
.PHONY: all kernel strip sterilize syms preproc debug clean

all: kernel syms strip sterilize preproc

kernel: $(APP).elf $(APP).hex

strip: $(APP)-strip.elf

sterilize: $(APP)-sterile.elf

syms: $(APP).asm

preproc: $(PREPROCS)

debug: $(APP).elf
	@$(MSPDEBUG) sim gdb &
	@$(GDB) $<

clean:
	@rm -rf $(OBJDIR) $(BINDIR) $(PREPROCDIR) $(AUTOGEN_SRC) $(DEPS)

#-------------------------------------------------------------------------------
# The implementation
#-------------------------------------------------------------------------------
$(APP).elf: $(OBJS)
	@mkdir -p $(BINDIR)
	@echo [LD] $@
	@$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@$(SIZE) $@

$(APP).asm: $(APP).elf
	@$(OBJDUMP) -dS $< > $(APP).asm

$(APP)-strip.elf: $(APP).elf
	@$(STRIP) -d $< -o $(APP)-strip.elf

$(APP)-sterile.elf: $(APP).elf
	@$(STRIP) -s $< -o $(APP)-sterile.elf

$(APP).hex: $(APP).elf
	@$(OBJCOPY) -S -O ihex $< $@
	@$(OBJCOPY) -O ihex $< $@

#-------------------------------------------------------------------------------
# Compilation rules
#-------------------------------------------------------------------------------
define module_name
$(shell echo $(basename $(notdir $(1))) | tr '[:lower:]' '[:upper:]')
endef

$(OBJDIR)%/:
	@mkdir -p $@

$(OBJS): $(AUTOGEN_SRC)

$(OBJDIR)/%.c.o: %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) \
	-DK_MODULE_NAME=$(call module_name, $<) \
	-c $< -o $@ 

$(OBJDIR)/%.cc.o: %.cc
	@echo [CXX] $<
	@$(CXX) $(CFLAGS) \
	-DK_MODULE_NAME=$(call module_name, $<) \
	-c $< -o $@ 

$(OBJDIR)/%.cpp.o: %.cpp
	@echo [CXX] $<
	@$(CXX) $(CFLAGS) \
	-DK_MODULE_NAME=$(call module_name, $<) \
	-c $< -o $@ 

$(OBJDIR)/%.S.o: %.S
	@echo [AS] $<
	@$(CC) $(ASFLAGS) \
	-DK_MODULE_NAME=$(call module_name, $<) \
	-c $< -o $@ 

$(foreach OBJ, $(OBJS), $(eval $(OBJ): | $(dir $(OBJ))))

#-------------------------------------------------------------------------------
# Preprocessing rules
#-------------------------------------------------------------------------------

$(PREPROCDIR)/%.i: %
	@mkdir -p $(dir $@)
	@$(CC) $(addprefix -I, $(INCDIRS)) -E $< > $@

$(PREPROCS): $(AUTOGEN_SRC)

#-------------------------------------------------------------------------------
# Autogen rules
#-------------------------------------------------------------------------------
$(K_TRAP_TBL):
	@python3 tools/gen_traps.py \
		--symbols $(MSP430_GCC_INCLUDE_DIR)/$(CONFIG_MCU).ld \
		--reserve reset $(CONFIG_K_TIMER) \
		--destination $(K_TRAP_TBL)

$(K_TRAP_DEFS):
	@python3 tools/gen_trap_defs.py \
		--symbols $(MSP430_GCC_INCLUDE_DIR)/$(CONFIG_MCU).ld \
		--reserve reset $(CONFIG_K_TIMER) \
		--destination $(K_TRAP_HW_HDR)
	
	@python3 tools/gen_trap_defs.py \
		--symbols $(MSP430_GCC_INCLUDE_DIR)/$(CONFIG_MCU).ld \
		--reserve reset $(CONFIG_K_TIMER) \
		--destination $(K_TRAP_KRNL_HDR) \
		--kernel