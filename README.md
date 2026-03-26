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
git clone --recursive https://github.com/STMicroelectronics/STM32CubeF7.git ${STM_DIR}
git clone --recursive https://github.com/libdriver/ssd1309.git ${SSD_DIR}
git clone --recursive -b disco https://github.com/paulsjit/nucleo-display.git ${PROJ_DIR}
```

# 3. add a patch on ssd1309 (separately in e-mail) 

# 4. make
```
cd ${PROJ_DIR}
make
```

# 5. start ST-GDB-server
```
${CUBE_DIR}/plugins/com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.linux64_2.2.300.202509021040/tools/bin/ST-LINK_gdbserver \
    -p 61234 \
    -l 31 \
    -d \
    -s \
    -cp ${CUBE_DIR}/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.300.202508131133/tools/bin \
    -m 0 \
    -k \
    -e \
    -v
```

# 6. run
Make the H/W connections:
- CN12:2  -> VDD
- CN12:6  -> GND
- P1:08   -> SCL
- P1:01   -> SDA
```
cd ${PROJ_DIR}
make run
```

