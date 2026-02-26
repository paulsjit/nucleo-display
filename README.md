These boilerplate files have been copied (with minor changes) from a STM Cube project
- main.c
- startup_stm32f207xx.s
- STM32F207ZGTX_FLASH.ld
- stm32f2xx_hal_conf.h
- stm32f2xx_it.c
- syscalls.c
- system_stm32f2xx.c

And the following files have been added to interface with the ssd1309 driver
- hooks.c

`main.c` has been modified to call these functions from ssd1309 driver
```
  delay_init();
  uart_init(115200);
  ssd1309_display_test(SSD1309_INTERFACE_IIC, SSD1309_ADDR_SA0_0);
```


**This projects uses SW I2C out-of-the-box, but you can change it in Makefile by uncommenting a line **

# 1. Export the variables
```
export SSD_DIR=/where/ssd1309/will/be/cloned
export STM_DIR=/where/STM32CubeF2/will/be/cloned
export CUBE_DIR=/opt/where/stmcube/is/installed
export PROJ_DIR=/where/nucleo/display/will/be/cloned
```

# 2. clone dependent directories
```
git clone --recursive https://github.com/STMicroelectronics/STM32CubeF2.git ${STM_DIR}
git clone --recursive -b nucleo-display-stmf207zg https://github.com/paulsjit/ssd1309.git ${SSD_DIR}
git clone --recursive https://github.com/paulsjit/nucleo-display.git ${PROJ_DIR}
```

# 3. make
```
cd ${PROJ_DIR}
make
```

# 4. run
Make the H/W connections:
CN8:7   -> VDD
CN8:11  -> GND
CN10:13 -> SCL
CN7:4   -> SDA
```
cd ${PROJ_DIR}
make run
```

