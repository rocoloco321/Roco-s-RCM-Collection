#---------------------------------------------------------------------------------
# program checks
#---------------------------------------------------------------------------------

export NUL := /dev/null

ifeq ($(OS),Windows_NT)

ifeq (, $(shell where arm-none-eabi-gcc 2>$(NUL)))
$(error "No arm-none-eabi-gcc in PATH, consider doing 'pacman -Syy' then 'pacman -S mingw-w64-x86_64-arm-none-eabi-gcc'")
endif
ECHO_FLAGS := -e

else

ifeq (, $(shell which arm-none-eabi-gcc))
$(error "No arm-none-eabi-gcc in PATH, consider doing 'apt-get install arm-none-eabi-gcc'")
endif
ECHO_FLAGS :=

endif

#---------------------------------------------------------------------------------
# the prefix on the compiler executables
#---------------------------------------------------------------------------------
PREFIX         := arm-none-eabi-

export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export OBJDUMP := $(PREFIX)objdump
export LD      := $(PREFIX)ld
export STRIP   := $(PREFIX)strip
export OBJDUMP := $(PREFIX)objdump
export READELF := $(PREFIX)readelf

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET   := code
BUILD    := build
SOURCES  := source
INCLUDES := include source ../include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH :=	-march=armv5te -mtune=arm946e-s

CFLAGS := -Wall -Os\
 		$(ARCH) -fomit-frame-pointer\
		-ffast-math -fshort-wchar -mthumb-interwork \
		-fno-asynchronous-unwind-tables -fno-short-enums \
		-DARM9 -DSDK_ARM9 -DSDK_TS \
		-std=c99 -nodefaultlibs -I. \
		-fno-builtin -Wno-unknown-pragmas -c

CFLAGS += $(INCLUDE)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS    :=    -g $(ARCH) $(INCLUDE)
LDFLAGS = --use-blx,-r,-T,../../symbols.x,-T,../linker.x,-g,-Map,newcode.map

#---------------------------------------------------------------------------------
# include SDK and NITRO-System headers if present (full paths as env vars)
#---------------------------------------------------------------------------------
ifdef NITROSDK_ROOT
	SDK_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSDK_ROOT)))/include
endif
ifdef NITROSDK3_ROOT
	SDK_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSDK3_ROOT)))/include
endif
ifdef NITROSYSTEM_ROOT
	NNS_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSYSTEM_ROOT)))/include
endif
ifdef NITROSYSTEM050901_ROOT
	NNS_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSYSTEM050901_ROOT)))/include
endif

ifdef SDK_INCLUDE
	CFLAGS += -isystem $(SDK_INCLUDE)
endif
ifdef NNS_INCLUDE
	CFLAGS += -isystem $(NNS_INCLUDE)
endif

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS := -lc

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES	:=	$(addsuffix .o,$(CPPFILES)) $(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)


.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@+make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).rcm syms.txt relocs.txt
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[34mInfo\033[1;37m] \033[0mCleaned"

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

all: $(OUTPUT).rcm

$(OUTPUT).rcm : $(OUTPUT).elf
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Relocating -> \033[33;1m\"$(notdir $@)\"\033[0m"
	@$(OBJCOPY) -O binary $< $@
	@java -cp ../../rcmreloc RCMRelocator $@ ../relocs.txt ../syms.txt > $(NUL)
	@rm -fr $< ../syms.txt ../relocs.txt
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Built"

#---------------------------------------------------------------------------------
%.elf: $(OFILES)
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Linking -> \033[33;1m\"$(notdir $@)\"\033[0m"
	@$(CC) -nostdlib -nodefaultlibs -nostartfiles $(OFILES) -Wl,$(LIBS),$(LDFLAGS) -o $@
	@$(STRIP) -g --strip-unneeded -R .comment $@
	@$(OBJDUMP) -t $@ > ../syms.txt
	@$(READELF) -r $@ > ../relocs.txt

#---------------------------------------------------------------------------------
.PRECIOUS: %.cpp.o
%.cpp.o: %.cpp
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Building C++ file \033[33;1m\"$(notdir $<)\"\033[0m"
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.cpp.d $(CXXFLAGS) -c $< -o $@

#---------------------------------------------------------------------------------
.PRECIOUS: %.c.o
%.c.o: %.c
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Building C file \033[33;1m\"$(notdir $<)\"\033[0m"
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.c.d $(CFLAGS) -c $< -o $@

#---------------------------------------------------------------------------------
.PRECIOUS: %.s.o
%.s.o: %.s
	@echo $(ECHO_FLAGS) "\033[37;1m[\033[0m\033[32;1mBuild\033[0m\033[37;1m]\033[0m Building ASM file \033[33;1m\"$(notdir $<)\"\033[0m"
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.s.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
