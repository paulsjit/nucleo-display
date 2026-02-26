STM_DIR ?= /home/subhajit/tiger/STM32CubeF2
CUBE_DIR ?= /opt/st/stm32cubeide_2.0.0
SSD_DIR ?= /home/subhajit/tiger/ssd1309
PROJ_DIR ?= $(PWD)
GDB := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-gdb
CC := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-gcc
OBJDUMP := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-objdump
STRIP := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-strip

Q ?= @

DEFINES := 

# DEFINES += -DHW_I2C
# DEFINES += -DHW_I2C_INTR

IFLAGS := -I$(PROJ_DIR) \
		 -I$(STM_DIR)/Drivers/CMSIS/Device/ST/STM32F2xx/Include \
		 -I$(STM_DIR)/Drivers/CMSIS/Include \
		 -I$(STM_DIR)/Drivers/STM32F2xx_HAL_Driver/Inc \
		 -I$(STM_DIR)/Drivers/BSP/STM32F2xx_Nucleo_144 \
		 -I$(SSD_DIR)/test \
		 -I$(SSD_DIR)/interface \
		 -I$(SSD_DIR)/src \
		 -I$(SSD_DIR)/project/stm32f407/interface/inc

AFLAGS := -mcpu=cortex-m3 \
		 -g3 \
		 -x assembler-with-cpp \
		 --specs=nano.specs \
		 -mfloat-abi=soft \
		 -mthumb \
		 -MMD \
		 -MP \

CFLAGS := -mcpu=cortex-m3 \
		 -std=gnu11 \
		 -g3 \
		 -DUSE_HAL_DRIVER \
		 -DSTM32F207xx \
		 -DUSE_STM32F2XX_NUCLEO_144 \
		 --specs=nano.specs \
		 -mfloat-abi=soft \
		 -mthumb \
		 -O2 \
		 -ffunction-sections \
		 -Wall \
		 -fstack-usage \
		 -fcyclomatic-complexity \
		 -MMD \
		 -MP

LDFLAGS := -mcpu=cortex-m3 \
		  -T"STM32F207ZGTX_FLASH.ld" \
		  --specs=nosys.specs \
		  -Wl,-Map="app.map" \
		  -Wl,--gc-sections \
		  -static \
		  --specs=nano.specs \
		  -mfloat-abi=soft \
		  -mthumb \
		  -Wl,--start-group -lc -lm -Wl,--end-group


SUPPORT_DIRS := $(STM_DIR)/Drivers/STM32F2xx_HAL_Driver/Src \
	$(STM_DIR)/Drivers/BSP/STM32F2xx_Nucleo_144 \
	$(STM_DIR)/Drivers/CMSIS

EXCLUDES := $(STM_DIR)/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_timebase_rtc_alarm_template.o \
	$(STM_DIR)/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_timebase_tim_template.o \
	$(STM_DIR)/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_timebase_rtc_wakeup_template.o

SRCS := startup_stm32f207xx.s \
		system_stm32f2xx.c \
		stm32f2xx_it.c \
		main.c \
		syscalls.c \
		hooks.c \
		$(SSD_DIR)/src/driver_ssd1309.c	\
		$(SSD_DIR)/test/driver_ssd1309_display_test.c \
		$(SSD_DIR)/project/stm32f407/driver/src/stm32f407_driver_ssd1309_interface.c \
		$(SSD_DIR)/project/stm32f407/interface/src/iic.c \
		$(SSD_DIR)/project/stm32f407/interface/src/delay.c


C_SRCS := $(filter %.c, $(foreach d,$(SUPPORT_DIRS),$(wildcard $(d)/*.c)) $(SRCS))
C_OBJECTS :=  $(foreach f,$(C_SRCS),$(patsubst %c,%o,$(f)))
C_OBJECTS := $(filter-out $(EXCLUDES), $(C_OBJECTS)) 

ASM_SRCS := $(filter %.s, $(foreach d,$(SUPPORT_DIRS),$(wildcard $(d)/*.s)) $(SRCS))
ASM_OBJECTS :=  $(foreach f,$(ASM_SRCS),$(patsubst %s,%o,$(f)))

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

SU_FILES := $(foreach f,$(C_SRCS),$(patsubst %.c,%.su,$(f)))
CYCLO_FILES := $(foreach f,$(C_SRCS),$(patsubst %.c,%.cyclo,$(f)))
DEP_FILES := $(foreach f,$(C_SRCS),$(dir $(f))/$(patsubst %.c,.%.d,$(notdir $(f))))

%.o : %.s Makefile
	$(Q)([ x$(Q) == 'x@' ] && echo [ASM   ] $@) && ($(CC) $(AFLAGS) -c -o $@ $< -MF"$(dir $<)/$(patsubst %.s,.%.d,$(notdir $<))" -MT"$@")

%.o : %.c Makefile
	$(Q)([ x$(Q) == 'x@' ] && echo [CC    ] $@) && ($(CC) $(DEFINES) $(IFLAGS) $(CFLAGS) -c -o $@ $< -MF"$(dir $<)/$(patsubst %.c,.%.d,$(notdir $<))" -MT"$@")

.PHONY: app
app : app.elf app.elf.stripped app.disass Makefile STM32F207ZGTX_FLASH.ld

app.elf: $(OBJECTS)
	$(Q)([ x$(Q) == 'x@' ] && echo [LD    ] $@) && ($(CC) -o $@ $^ $(LDFLAGS))
	
app.elf.stripped: app.elf
	$(Q)([ x$(Q) == 'x@' ] && echo [STRIP ] $@) && ($(STRIP) --strip-debug -o $@ $^) 

app.disass: app.elf.stripped
	$(Q)([ x$(Q) == 'x@' ] && echo [DISASS] $@) && ($(OBJDUMP) -S $^ > $@)

.PHONY: debug
debug: app
	$(GDB) --ex 'target remote 127.0.0.1:61234' app.elf --ex 'load' \
	$(if $(wildcard test_debug.gdb),--command test_debug.gdb)

.PHONY: run 
run: app.elf
	$(GDB) --ex 'target remote 127.0.0.1:61234' app.elf --ex 'load' --ex 'detach' --batch \
	$(if $(wildcard test_run.gdb),--command test_run.gdb)

.PHONY : clean
clean: 
	rm -Rf app.elf app.elf.stripped app.map app.disass $(OBJECTS) $(SU_FILES) $(CYCLO_FILES) $(DEP_FILES)

