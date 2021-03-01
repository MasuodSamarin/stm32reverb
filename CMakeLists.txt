cmake_minimum_required(VERSION 3.15.3)

project(nucleo-f413zh)

enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(STM32CUBEMX_GENERATED_FILES

	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c

        Core/Inc/main.h
        Core/Inc/stm32f4xx_hal_conf.h
        Core/Inc/stm32f4xx_it.h

        Core/Src/main.c
        Core/Src/stm32f4xx_hal_msp.c
        Core/Src/stm32f4xx_it.c
        Core/Src/system_stm32f4xx.c

        startup_stm32f413xx.s)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES}
	Core/Src/application.cpp
	Core/Src/application.hpp)

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F413xx
        )

target_include_directories(${EXECUTABLE} PRIVATE
        Core/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/CMSIS/Include
	Core/Src/
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections
	-fno-exceptions

        -Wall

        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/STM32F413ZHTx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nosys.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)

