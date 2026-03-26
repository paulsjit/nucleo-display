STM_DIR ?= /home/subhajit/tiger/STM32CubeF7
CUBE_DIR ?= /opt/st/stm32cubeide_2.0.0
SSD_DIR ?= /home/subhajit/tiger/ssd1306_clone
PROJ_DIR ?= $(PWD)
GDB := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-gdb
CC := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-gcc
OBJDUMP := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-objdump
STRIP := $(CUBE_DIR)/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.100.202509120712/tools/bin/arm-none-eabi-strip
GDB_SERVER ?= 127.0.0.1

Q ?= @

DEFINES := 

# DEFINES += -DDISPLAY_ZSTATE

IFLAGS := -I$(PROJ_DIR) \
		 -I$(STM_DIR)/Drivers/CMSIS/Device/ST/STM32F7xx/Include \
		 -I$(STM_DIR)/Drivers/CMSIS/Include \
		 -I$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Inc \
		 -I$(STM_DIR)/Drivers/BSP/STM32F723E-Discovery \
		 -I$(SSD_DIR)/test \
		 -I$(SSD_DIR)/interface \
		 -I$(SSD_DIR)/src

AFLAGS := -mcpu=cortex-m7 \
		 -g3 \
		 -x assembler-with-cpp \
		 --specs=nano.specs \
		 -mfpu=fpv5-sp-d16 \
		 -mfloat-abi=hard \
		 -mthumb \
		 -MMD \
		 -MP

CFLAGS :=	-mcpu=cortex-m7 \
			-std=gnu11 \
			-g3 \
			-DUSE_HAL_DRIVER \
			-DSTM32F723xx \
			-DUSE_STM32F723E_DISCO \
			--specs=nano.specs \
			-mfpu=fpv5-sp-d16 \
			-mfloat-abi=hard \
			-mthumb \
			-O2 \
			-ffunction-sections \
			-Wall \
			-fstack-usage \
			-fcyclomatic-complexity \
			-MMD \
			-MP

LDFLAGS := -mcpu=cortex-m7 \
		  --specs=nosys.specs \
		  -Wl,-Map="app.map" \
		  -Wl,--gc-sections \
		  -static \
		  --specs=nano.specs \
		  -mfpu=fpv5-sp-d16 \
		  -mfloat-abi=hard \
		  -mthumb \
		  -Wl,--start-group -lc -lm -Wl,--end-group

LINKER_FILE := STM32F723IEKx_FLASH.ld

SRCS := startup_stm32f723xx.s \
		system_stm32f7xx.c \
		stm32f7xx_it.c \
		main.c \
		syscalls.c \
		stm32f7xx_hal_msp.c \
		hooks.c \
		$(STM_DIR)/Drivers/BSP/STM32F723E-Discovery/stm32f723e_discovery.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c \
		$(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c \
		$(SSD_DIR)/src/driver_ssd1309.c	\
		$(SSD_DIR)/test/driver_ssd1309_display_test.c \

# SRCS += hook_i2c_sw.c
# SRCS += hook_i2c_hw.c
# SRCS += hook_i2c_hw_it.c
SRCS += hook_i2c_hw_it_dma.c

ifneq ($(filter hook_i2c_hw%.c, $(SRCS)),)
SRCS += hook_i2c_hw_common.c
SRCS += $(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
SRCS += $(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c

ifneq ($(filter hook_i2c_hw_it%.c, $(SRCS)),)
SRCS += $(STM_DIR)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
endif

DEFINES += -DI2C_TIMING=0x00C4092A
DEFINES += -DI2C_ADDRESS=0x30F
endif

C_SRCS := $(filter %.c, $(foreach d,$(SUPPORT_DIRS),$(wildcard $(d)/*.c)) $(SRCS))
C_OBJECTS :=  $(foreach f,$(C_SRCS),$(patsubst %c,%o,$(f)))
C_OBJECTS := $(filter-out $(EXCLUDES), $(C_OBJECTS)) 

ASM_SRCS := $(filter %.s, $(foreach d,$(SUPPORT_DIRS),$(wildcard $(d)/*.s)) $(SRCS))
ASM_OBJECTS :=  $(foreach f,$(ASM_SRCS),$(patsubst %s,%o,$(f)))

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

SU_FILES := $(foreach f,$(C_SRCS),$(patsubst %.c,%.su,$(f)))
CYCLO_FILES := $(foreach f,$(C_SRCS),$(patsubst %.c,%.cyclo,$(f)))
DEP_FILES := $(foreach f,$(C_SRCS),$(dir $(f))/$(patsubst %.c,.%.d,$(notdir $(f))))

-include $(DEP_FILES)

%.o : %.s Makefile
	$(if $(Q),$(Q)echo "[ASM   ] $@",)
	$(Q)$(CC) $(AFLAGS) -c -o $@ $< -MF"$(dir $<)/$(patsubst %.s,.%.d,$(notdir $<))" -MT"$@"

%.o : %.c Makefile
	$(if $(Q),$(Q)echo "[CC    ] $@",)
	$(Q)$(CC) $(DEFINES) $(IFLAGS) $(CFLAGS) -c -o $@ $< -MF"$(dir $<)/$(patsubst %.c,.%.d,$(notdir $<))" -MT"$@"

.PHONY: app
app : app.elf app.disass

app.elf: $(OBJECTS) $(LINKER_FILE) Makefile 
	$(if $(Q),$(Q)echo "[LD    ] $@",)
	$(Q)$(CC) -o $@ $(OBJECTS) $(LDFLAGS) -T"$(LINKER_FILE)"
	
app.disass: app.elf
	$(if $(Q),$(Q)echo "[DISASS] $@",)
	$(Q)$(STRIP) --strip-debug -o $^.stripped $^ && \
		$(OBJDUMP) -S $^.stripped > $@ && \
		rm $^.stripped

.PHONY: debug
debug: app
	$(GDB) --ex 'target remote $(GDB_SERVER):61234' app.elf --ex 'load' \
	$(if $(wildcard test_debug.gdb),--command test_debug.gdb)

.PHONY: run 
run: app
	$(GDB) --ex 'target remote $(GDB_SERVER):61234' app.elf --ex 'load' --ex 'detach' --batch \
	$(if $(wildcard test_run.gdb),--command test_run.gdb)

.PHONY : clean
clean: 
	rm -Rf app.elf app.elf.stripped app.map app.disass $(OBJECTS) $(SU_FILES) $(CYCLO_FILES) $(DEP_FILES)

