################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Sensor/doorsensor.c \
../Core/Src/Sensor/levelsensor.c \
../Core/Src/Sensor/sensor.c \
../Core/Src/Sensor/temperature.c \
../Core/Src/Sensor/vacuumsensor.c \
../Core/Src/Sensor/valvesensor.c 

OBJS += \
./Core/Src/Sensor/doorsensor.o \
./Core/Src/Sensor/levelsensor.o \
./Core/Src/Sensor/sensor.o \
./Core/Src/Sensor/temperature.o \
./Core/Src/Sensor/vacuumsensor.o \
./Core/Src/Sensor/valvesensor.o 

C_DEPS += \
./Core/Src/Sensor/doorsensor.d \
./Core/Src/Sensor/levelsensor.d \
./Core/Src/Sensor/sensor.d \
./Core/Src/Sensor/temperature.d \
./Core/Src/Sensor/vacuumsensor.d \
./Core/Src/Sensor/valvesensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Sensor/%.o Core/Src/Sensor/%.su Core/Src/Sensor/%.cyclo: ../Core/Src/Sensor/%.c Core/Src/Sensor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../FATFS/App -I../USB_HOST/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Hardware" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"C:/STMWorkSpace2/P20/Core/Src/Sensor" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Peripheral" -I../FATFS/Target -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Sensor

clean-Core-2f-Src-2f-Sensor:
	-$(RM) ./Core/Src/Sensor/doorsensor.cyclo ./Core/Src/Sensor/doorsensor.d ./Core/Src/Sensor/doorsensor.o ./Core/Src/Sensor/doorsensor.su ./Core/Src/Sensor/levelsensor.cyclo ./Core/Src/Sensor/levelsensor.d ./Core/Src/Sensor/levelsensor.o ./Core/Src/Sensor/levelsensor.su ./Core/Src/Sensor/sensor.cyclo ./Core/Src/Sensor/sensor.d ./Core/Src/Sensor/sensor.o ./Core/Src/Sensor/sensor.su ./Core/Src/Sensor/temperature.cyclo ./Core/Src/Sensor/temperature.d ./Core/Src/Sensor/temperature.o ./Core/Src/Sensor/temperature.su ./Core/Src/Sensor/vacuumsensor.cyclo ./Core/Src/Sensor/vacuumsensor.d ./Core/Src/Sensor/vacuumsensor.o ./Core/Src/Sensor/vacuumsensor.su ./Core/Src/Sensor/valvesensor.cyclo ./Core/Src/Sensor/valvesensor.d ./Core/Src/Sensor/valvesensor.o ./Core/Src/Sensor/valvesensor.su

.PHONY: clean-Core-2f-Src-2f-Sensor

