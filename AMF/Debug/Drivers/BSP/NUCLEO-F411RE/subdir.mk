################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/NUCLEO-F411RE/uart_trace.c \
../Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.c 

OBJS += \
./Drivers/BSP/NUCLEO-F411RE/uart_trace.o \
./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.o 

C_DEPS += \
./Drivers/BSP/NUCLEO-F411RE/uart_trace.d \
./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/NUCLEO-F411RE/%.o Drivers/BSP/NUCLEO-F411RE/%.su Drivers/BSP/NUCLEO-F411RE/%.cyclo: ../Drivers/BSP/NUCLEO-F411RE/%.c Drivers/BSP/NUCLEO-F411RE/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"/home/juli/Documents/st/ENIB/AMF/Drivers/BSP/Components/vl53l0x" -I"/home/juli/Documents/st/ENIB/AMF/Drivers/BSP/NUCLEO-F411RE" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-NUCLEO-2d-F411RE

clean-Drivers-2f-BSP-2f-NUCLEO-2d-F411RE:
	-$(RM) ./Drivers/BSP/NUCLEO-F411RE/uart_trace.cyclo ./Drivers/BSP/NUCLEO-F411RE/uart_trace.d ./Drivers/BSP/NUCLEO-F411RE/uart_trace.o ./Drivers/BSP/NUCLEO-F411RE/uart_trace.su ./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.cyclo ./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.d ./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.o ./Drivers/BSP/NUCLEO-F411RE/vl53l0x_platform.su

.PHONY: clean-Drivers-2f-BSP-2f-NUCLEO-2d-F411RE

