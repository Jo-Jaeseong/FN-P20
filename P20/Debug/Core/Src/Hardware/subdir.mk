################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Hardware/doorlatch.c \
../Core/Src/Hardware/fan.c \
../Core/Src/Hardware/hardware.c \
../Core/Src/Hardware/heater.c \
../Core/Src/Hardware/peristalticpump.c \
../Core/Src/Hardware/plasma.c \
../Core/Src/Hardware/solenoidvalve.c \
../Core/Src/Hardware/vacuumpump.c 

OBJS += \
./Core/Src/Hardware/doorlatch.o \
./Core/Src/Hardware/fan.o \
./Core/Src/Hardware/hardware.o \
./Core/Src/Hardware/heater.o \
./Core/Src/Hardware/peristalticpump.o \
./Core/Src/Hardware/plasma.o \
./Core/Src/Hardware/solenoidvalve.o \
./Core/Src/Hardware/vacuumpump.o 

C_DEPS += \
./Core/Src/Hardware/doorlatch.d \
./Core/Src/Hardware/fan.d \
./Core/Src/Hardware/hardware.d \
./Core/Src/Hardware/heater.d \
./Core/Src/Hardware/peristalticpump.d \
./Core/Src/Hardware/plasma.d \
./Core/Src/Hardware/solenoidvalve.d \
./Core/Src/Hardware/vacuumpump.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Hardware/%.o Core/Src/Hardware/%.su Core/Src/Hardware/%.cyclo: ../Core/Src/Hardware/%.c Core/Src/Hardware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../FATFS/App -I../USB_HOST/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Hardware" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"C:/STMWorkSpace2/P20/Core/Src/Sensor" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Peripheral" -I../FATFS/Target -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Hardware

clean-Core-2f-Src-2f-Hardware:
	-$(RM) ./Core/Src/Hardware/doorlatch.cyclo ./Core/Src/Hardware/doorlatch.d ./Core/Src/Hardware/doorlatch.o ./Core/Src/Hardware/doorlatch.su ./Core/Src/Hardware/fan.cyclo ./Core/Src/Hardware/fan.d ./Core/Src/Hardware/fan.o ./Core/Src/Hardware/fan.su ./Core/Src/Hardware/hardware.cyclo ./Core/Src/Hardware/hardware.d ./Core/Src/Hardware/hardware.o ./Core/Src/Hardware/hardware.su ./Core/Src/Hardware/heater.cyclo ./Core/Src/Hardware/heater.d ./Core/Src/Hardware/heater.o ./Core/Src/Hardware/heater.su ./Core/Src/Hardware/peristalticpump.cyclo ./Core/Src/Hardware/peristalticpump.d ./Core/Src/Hardware/peristalticpump.o ./Core/Src/Hardware/peristalticpump.su ./Core/Src/Hardware/plasma.cyclo ./Core/Src/Hardware/plasma.d ./Core/Src/Hardware/plasma.o ./Core/Src/Hardware/plasma.su ./Core/Src/Hardware/solenoidvalve.cyclo ./Core/Src/Hardware/solenoidvalve.d ./Core/Src/Hardware/solenoidvalve.o ./Core/Src/Hardware/solenoidvalve.su ./Core/Src/Hardware/vacuumpump.cyclo ./Core/Src/Hardware/vacuumpump.d ./Core/Src/Hardware/vacuumpump.o ./Core/Src/Hardware/vacuumpump.su

.PHONY: clean-Core-2f-Src-2f-Hardware

