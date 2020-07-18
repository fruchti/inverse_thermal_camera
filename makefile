PROJECT = main
SOURCE_DIRS = src third_party/fatfs
ADDITIONAL_SOURCES = 
INCLUDE_DIRS = third_party/core third_party/device
EXCLUDE_SOURCES =
BUILD_DIR = build

DEBUG := yes

H_DEVICE = STM32F103xB
STARTUP_SOURCE_DIR = src
STARTUP_SOURCES = $(STARTUP_SOURCE_DIR)/startup.S
LD_SCRIPT = ld/stm32f103c8t6_flash.ld

ifeq ($(DEBUG),yes)
DEBUG_FLAGS = -DDEBUG -g3
endif

ifneq ($(V),1)
Q = @
endif

CFLAGS = -mcpu=cortex-m3 -mthumb \
		 -Os -fno-common -Werror \
		 -nostartfiles \
		 -Wall -Wextra -Xlinker --gc-sections \
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

CURRENT_BUILD_CONFIG := $(shell cat makefile | md5sum) DEBUG = $(DEBUG)
LAST_BUILD_CONFIG := $(shell if [ -e $(BUILD_DIR)/build-config.txt ] ; then cat $(BUILD_DIR)/build-config.txt ; fi)

SOURCES = $(filter-out $(addprefix %/,$(EXCLUDE_SOURCES)),$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))) \
		  $(ADDITIONAL_SOURCES)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))
DEPENDS = $(addprefix $(BUILD_DIR)/,$(addsuffix .d,$(basename $(notdir $(SOURCES)))))
STARTUP_OBJECTS = $(patsubst $(STARTUP_SOURCE_DIR)/%.S, $(BUILD_DIR)/%.o, $(STARTUP_SOURCES))

.DEFAULT_GOAL = all
.DELETE_ON_ERROR:

define define_compile_rules
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(filter-out $(EXCLUDE_SOURCES),$(notdir $(wildcard $(1)/*.c)))))): $(BUILD_DIR)/%.o: $(1)/%.c
	@echo " CC $$@"
	$(Q)$$(CC) $$(CFLAGS) -o $$@ -c $$<

$(addprefix $(BUILD_DIR)/,$(addsuffix .d,$(basename $(filter-out $(EXCLUDE_SOURCES),$(notdir $(wildcard $(1)/*.c)))))): $(BUILD_DIR)/%.d: $(1)/%.c
	@#echo " DP $$@"
	$(Q)set -e; rm -f $$@; $$(CC) -MM $$(CFLAGS) $$< > $$@.$$$$$$$$; sed 's,\($$*\)\.o[ :]*,build\/\1.o $$@ : ,g' < $$@.$$$$$$$$ > $$@; rm -f $$@.$$$$$$$$
endef

$(foreach directory,$(SOURCE_DIRS),$(eval $(call define_compile_rules,$(directory))))

# Additional sources
define define_compile_rule
$(addprefix $(BUILD_DIR)/,$(notdir $(1:.c=.o))): $(1)
	@echo " CC $$@"
	$(Q)$$(CC) $$(CFLAGS) -o $$@ -c $$<

$(addprefix $(BUILD_DIR)/,$(notdir $(1:.c=.d))): $(1)
	@#echo " DP $$@"
	$(Q)set -e; rm -f $$@; $$(CC) -MM $$(CFLAGS) $$< > $$@.$$$$$$$$; sed 's,\($$*\)\.o[ :]*,build\/\1.o $$@ : ,g' < $$@.$$$$$$$$ > $$@; rm -f $$@.$$$$$$$$
endef

$(foreach src,$(ADDITIONAL_SOURCES),$(eval $(call define_compile_rule,$(src))))

$(STARTUP_OBJECTS): $(BUILD_DIR)/%.o: $(STARTUP_SOURCE_DIR)/%.S
	@echo " AS $@"
	$(Q)$(CC) $< -c -o $@ $(CFLAGS)

$(DEPENDS):| $(BUILD_DIR)

include $(DEPENDS) 

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) $(STARTUP_OBJECTS) $(BUILD_NUMBER_FILE)
	@echo " LD $@"
	$(Q)$(CC) $(OBJECTS) $(STARTUP_OBJECTS) $(CFLAGS) $(BUILD_ID_FLAGS) -o $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo " OC $@"
	$(Q)$(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR)/$(PROJECT).lst: $(BUILD_DIR)/$(PROJECT).elf
	@echo " OD $@"
	$(Q)$(OBJDUMP) -h -S $< > $@

$(BUILD_DIR):
	$(Q)if [ ! -d "$(BUILD_DIR)" ]; then mkdir "$(BUILD_DIR)"; fi

$(BUILD_NUMBER_FILE): $(OBJECTS) $(STARTUP_OBJECTS)
	$(Q)if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); \
	else echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE) ; fi

# Rebuild everything in case of a makefile/configuration change
.PHONY: all
ifneq ("$(CURRENT_BUILD_CONFIG)","$(LAST_BUILD_CONFIG)")
all: clean incrementalbuild
else
all: incrementalbuild
endif

.PHONY: incrementalbuild
incrementalbuild: $(BUILD_DIR) $(OBJECTS) $(STARTUP_OBJECTS) $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).lst
	@echo "Finished build $$(cat $(BUILD_NUMBER_FILE)). Binary size:"
	@echo " SZ $(BUILD_DIR)/$(PROJECT).elf"
	$(Q)$(SIZE) $(BUILD_DIR)/$(PROJECT).elf
	@echo "$(CURRENT_BUILD_CONFIG)" > $(BUILD_DIR)/build-config.txt

.PHONY: program
program: $(BUILD_DIR)/$(PROJECT).bin
	@#if ps -e | grep openocd ; then arm-none-eabi-gdb -batch -x flash.gdb ; else st-flash --reset write $(BUILD_DIR)/$(PROJECT).bin 0x8000000 ; fi
	st-flash --reset write $(BUILD_DIR)/$(PROJECT).bin 0x8000000

.PHONY: clean
clean:
	@echo " RM $(BUILD_DIR)/*"
	$(Q)$(RM) $(BUILD_DIR)/*
