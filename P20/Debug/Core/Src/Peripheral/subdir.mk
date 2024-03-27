################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Peripheral/alarm.c \
../Core/Src/Peripheral/extern_flash.c \
../Core/Src/Peripheral/flash.c \
../Core/Src/Peripheral/interrupt.c \
../Core/Src/Peripheral/lcd.c \
../Core/Src/Peripheral/login_system.c \
../Core/Src/Peripheral/pm.c \
../Core/Src/Peripheral/print.c \
../Core/Src/Peripheral/process.c \
../Core/Src/Peripheral/rfid.c \
../Core/Src/Peripheral/sd.c \
../Core/Src/Peripheral/usb.c 

OBJS += \
./Core/Src/Peripheral/alarm.o \
./Core/Src/Peripheral/extern_flash.o \
./Core/Src/Peripheral/flash.o \
./Core/Src/Peripheral/interrupt.o \
./Core/Src/Peripheral/lcd.o \
./Core/Src/Peripheral/login_system.o \
./Core/Src/Peripheral/pm.o \
./Core/Src/Peripheral/print.o \
./Core/Src/Peripheral/process.o \
./Core/Src/Peripheral/rfid.o \
./Core/Src/Peripheral/sd.o \
./Core/Src/Peripheral/usb.o 

C_DEPS += \
./Core/Src/Peripheral/alarm.d \
./Core/Src/Peripheral/extern_flash.d \
./Core/Src/Peripheral/flash.d \
./Core/Src/Peripheral/interrupt.d \
./Core/Src/Peripheral/lcd.d \
./Core/Src/Peripheral/login_system.d \
./Core/Src/Peripheral/pm.d \
./Core/Src/Peripheral/print.d \
./Core/Src/Peripheral/process.d \
./Core/Src/Peripheral/rfid.d \
./Core/Src/Peripheral/sd.d \
./Core/Src/Peripheral/usb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Peripheral/%.o Core/Src/Peripheral/%.su Core/Src/Peripheral/%.cyclo: ../Core/Src/Peripheral/%.c Core/Src/Peripheral/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../FATFS/App -I../USB_HOST/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Hardware" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"C:/STMWorkSpace2/P20/Core/Src/Sensor" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I"C:/STMWorkSpace2/P20/Core/Src/Peripheral" -I../FATFS/Target -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Peripheral

clean-Core-2f-Src-2f-Peripheral:
	-$(RM) ./Core/Src/Peripheral/alarm.cyclo ./Core/Src/Peripheral/alarm.d ./Core/Src/Peripheral/alarm.o ./Core/Src/Peripheral/alarm.su ./Core/Src/Peripheral/extern_flash.cyclo ./Core/Src/Peripheral/extern_flash.d ./Core/Src/Peripheral/extern_flash.o ./Core/Src/Peripheral/extern_flash.su ./Core/Src/Peripheral/flash.cyclo ./Core/Src/Peripheral/flash.d ./Core/Src/Peripheral/flash.o ./Core/Src/Peripheral/flash.su ./Core/Src/Peripheral/interrupt.cyclo ./Core/Src/Peripheral/interrupt.d ./Core/Src/Peripheral/interrupt.o ./Core/Src/Peripheral/interrupt.su ./Core/Src/Peripheral/lcd.cyclo ./Core/Src/Peripheral/lcd.d ./Core/Src/Peripheral/lcd.o ./Core/Src/Peripheral/lcd.su ./Core/Src/Peripheral/login_system.cyclo ./Core/Src/Peripheral/login_system.d ./Core/Src/Peripheral/login_system.o ./Core/Src/Peripheral/login_system.su ./Core/Src/Peripheral/pm.cyclo ./Core/Src/Peripheral/pm.d ./Core/Src/Peripheral/pm.o ./Core/Src/Peripheral/pm.su ./Core/Src/Peripheral/print.cyclo ./Core/Src/Peripheral/print.d ./Core/Src/Peripheral/print.o ./Core/Src/Peripheral/print.su ./Core/Src/Peripheral/process.cyclo ./Core/Src/Peripheral/process.d ./Core/Src/Peripheral/process.o ./Core/Src/Peripheral/process.su ./Core/Src/Peripheral/rfid.cyclo ./Core/Src/Peripheral/rfid.d ./Core/Src/Peripheral/rfid.o ./Core/Src/Peripheral/rfid.su ./Core/Src/Peripheral/sd.cyclo ./Core/Src/Peripheral/sd.d ./Core/Src/Peripheral/sd.o ./Core/Src/Peripheral/sd.su ./Core/Src/Peripheral/usb.cyclo ./Core/Src/Peripheral/usb.d ./Core/Src/Peripheral/usb.o ./Core/Src/Peripheral/usb.su

.PHONY: clean-Core-2f-Src-2f-Peripheral

