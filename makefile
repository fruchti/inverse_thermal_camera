PROJECT = main
SOURCE_DIRS = src
ADDITIONAL_SOURCES = 
INCLUDE_DIRS = 
EXCLUDE_SOURCES =
BUILD_DIR = build

DEBUG := yes

CUBE_DIR ?= /opt/stm32cube/STM32Cube_FW_F1_V1.6.0
CUBE_DEVICE = STM32F1xx
H_DEVICE = STM32F103xB
STARTUP_SOURCE_DIR = $(CUBE_DIR)/Drivers/CMSIS/Device/ST/$(CUBE_DEVICE)/Source/Templates/gcc
STARTUP_SOURCES = $(STARTUP_SOURCE_DIR)/startup_stm32f103xb.s
LD_SCRIPT = ld/STM32F103C8T6_FLASH.ld

ifeq ($(DEBUG),yes)
DEBUG_FLAGS = -DDEBUG -g3
endif

CFLAGS = -mcpu=cortex-m3 -mthumb \
		 -Os -fno-common -Werror \
		 -Wall -Xlinker --gc-sections -I$(CUBE_DIR)/Drivers/CMSIS/Include \
		 -I$(CUBE_DIR)/Drivers/CMSIS/Device/ST/$(CUBE_DEVICE)/Include \
		 -D$(H_DEVICE) -D_DEFAULT_SOURCE -T$(LD_SCRIPT) \
		 -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map -std=c99 \
		 $(addprefix -I,$(SOURCE_DIRS) $(INCLUDE_DIRS)) $(DEBUG_FLAGS)

RM = rm -f
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump

BUILD_NUMBER_FILE = build-number.txt
BUILD_ID_FLAGS = -Xlinker --defsym -Xlinker __BUILD_DATE=$$(date +'%Y%m%d') \
				 -Xlinker --defsym -Xlinker __BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))

CURRENT_BUILD_CONFIG := $(shell cat makefile | md5sum) DEBUG = $(DEBUG) CUBE_DIR = $(CUBE_DIR)
LAST_BUILD_CONFIG := $(shell if [ -e $(BUILD_DIR)/build-config.txt ] ; then cat $(BUILD_DIR)/build-config.txt ; fi)

SOURCES = $(filter-out $(addprefix %/,$(EXCLUDE_SOURCES)),$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))) \
		  $(ADDITIONAL_SOURCES)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))
DEPENDS = $(addprefix $(BUILD_DIR)/,$(addsuffix .d,$(basename $(notdir $(SOURCES)))))
STARTUP_OBJECTS = $(patsubst $(STARTUP_SOURCE_DIR)/%.s, $(BUILD_DIR)/%.o, $(STARTUP_SOURCES))

.DEFAULT_GOAL = all
.DELETE_ON_ERROR:

define define_compile_rules
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(filter-out $(EXCLUDE_SOURCES),$(notdir $(wildcard $(1)/*.c)))))): $(BUILD_DIR)/%.o: $(1)/%.c
	@echo " CC $$@"
	@$$(CC) $$(CFLAGS) -o $$@ -c $$<

$(addprefix $(BUILD_DIR)/,$(addsuffix .d,$(basename $(filter-out $(EXCLUDE_SOURCES),$(notdir $(wildcard $(1)/*.c)))))): $(BUILD_DIR)/%.d: $(1)/%.c
	@#echo " DP $$@"
	@set -e; rm -f $$@; $$(CC) -MM $$(CFLAGS) $$< > $$@.$$$$$$$$; sed 's,\($$*\)\.o[ :]*,build\/\1.o $$@ : ,g' < $$@.$$$$$$$$ > $$@; rm -f $$@.$$$$$$$$
endef

$(foreach directory,$(SOURCE_DIRS),$(eval $(call define_compile_rules,$(directory))))

# Additional sources
define define_compile_rule
$(addprefix $(BUILD_DIR)/,$(notdir $(1:.c=.o))): $(1)
	@echo " CC $$@"
	@$$(CC) $$(CFLAGS) -o $$@ -c $$<

$(addprefix $(BUILD_DIR)/,$(notdir $(1:.c=.d))): $(1)
	@#echo " DP $$@"
	@set -e; rm -f $$@; $$(CC) -MM $$(CFLAGS) $$< > $$@.$$$$$$$$; sed 's,\($$*\)\.o[ :]*,build\/\1.o $$@ : ,g' < $$@.$$$$$$$$ > $$@; rm -f $$@.$$$$$$$$
endef

$(foreach src,$(ADDITIONAL_SOURCES),$(eval $(call define_compile_rule,$(src))))

$(STARTUP_OBJECTS): $(BUILD_DIR)/%.o: $(STARTUP_SOURCE_DIR)/%.s
	@echo " AS $@"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(DEPENDS):| $(BUILD_DIR)

include $(DEPENDS) 

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) $(STARTUP_OBJECTS) $(BUILD_NUMBER_FILE)
	@echo " LD $@"
	@$(CC) $(OBJECTS) $(STARTUP_OBJECTS) $(CFLAGS) $(BUILD_ID_FLAGS) -o $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo " OC $@"
	@$(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR)/$(PROJECT).lst: $(BUILD_DIR)/$(PROJECT).elf
	@echo " OD $@"
	@$(OBJDUMP) -h -S $< > $@

$(BUILD_DIR):
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir "$(BUILD_DIR)"; fi

$(BUILD_NUMBER_FILE): $(OBJECTS) $(STARTUP_OBJECTS)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); else \
	echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE) ; fi

# Rebuild everything in case of a makefile/configuration change
.PHONY: all
ifneq ("$(CURRENT_BUILD_CONFIG)","$(LAST_BUILD_CONFIG)")
all: clean incrementalbuild
else
all: incrementalbuild
endif

.PHONY: incrementalbuild
incrementalbuild: $(BUILD_DIR) $(OBJECTS) $(STARTUP_OBJECTS) $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).lst
	@echo " SZ $(BUILD_DIR)/$(PROJECT).elf"
	@$(SIZE) $(BUILD_DIR)/$(PROJECT).elf
	@echo "$(CURRENT_BUILD_CONFIG)" > $(BUILD_DIR)/build-config.txt

.PHONY: program
program: $(BUILD_DIR)/$(PROJECT).bin
	@#if ps -e | grep openocd ; then arm-none-eabi-gdb -batch -x flash.gdb ; else st-flash --reset write $(BUILD_DIR)/$(PROJECT).bin 0x8000000 ; fi
	st-flash --reset write $(BUILD_DIR)/$(PROJECT).bin 0x8000000

.PHONY: clean
clean:
	@echo " RM $(BUILD_DIR)/*"
	@$(RM) $(BUILD_DIR)/*