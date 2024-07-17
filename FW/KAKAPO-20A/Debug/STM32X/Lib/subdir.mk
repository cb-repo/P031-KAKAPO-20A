################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32X/Lib/ADC.c \
../STM32X/Lib/CAN.c \
../STM32X/Lib/CLK.c \
../STM32X/Lib/COMP.c \
../STM32X/Lib/CRC.c \
../STM32X/Lib/Core.c \
../STM32X/Lib/EEPROM.c \
../STM32X/Lib/FLASH.c \
../STM32X/Lib/GPIO.c \
../STM32X/Lib/I2C.c \
../STM32X/Lib/RNG.c \
../STM32X/Lib/SPI.c \
../STM32X/Lib/SUBGHZ.c \
../STM32X/Lib/TIM.c \
../STM32X/Lib/TSC.c \
../STM32X/Lib/UART.c \
../STM32X/Lib/US.c \
../STM32X/Lib/USB.c \
../STM32X/Lib/WDG.c \
../STM32X/Lib/syscalls.c 

OBJS += \
./STM32X/Lib/ADC.o \
./STM32X/Lib/CAN.o \
./STM32X/Lib/CLK.o \
./STM32X/Lib/COMP.o \
./STM32X/Lib/CRC.o \
./STM32X/Lib/Core.o \
./STM32X/Lib/EEPROM.o \
./STM32X/Lib/FLASH.o \
./STM32X/Lib/GPIO.o \
./STM32X/Lib/I2C.o \
./STM32X/Lib/RNG.o \
./STM32X/Lib/SPI.o \
./STM32X/Lib/SUBGHZ.o \
./STM32X/Lib/TIM.o \
./STM32X/Lib/TSC.o \
./STM32X/Lib/UART.o \
./STM32X/Lib/US.o \
./STM32X/Lib/USB.o \
./STM32X/Lib/WDG.o \
./STM32X/Lib/syscalls.o 

C_DEPS += \
./STM32X/Lib/ADC.d \
./STM32X/Lib/CAN.d \
./STM32X/Lib/CLK.d \
./STM32X/Lib/COMP.d \
./STM32X/Lib/CRC.d \
./STM32X/Lib/Core.d \
./STM32X/Lib/EEPROM.d \
./STM32X/Lib/FLASH.d \
./STM32X/Lib/GPIO.d \
./STM32X/Lib/I2C.d \
./STM32X/Lib/RNG.d \
./STM32X/Lib/SPI.d \
./STM32X/Lib/SUBGHZ.d \
./STM32X/Lib/TIM.d \
./STM32X/Lib/TSC.d \
./STM32X/Lib/UART.d \
./STM32X/Lib/US.d \
./STM32X/Lib/USB.d \
./STM32X/Lib/WDG.d \
./STM32X/Lib/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
STM32X/Lib/%.o STM32X/Lib/%.su STM32X/Lib/%.cyclo: ../STM32X/Lib/%.c STM32X/Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-STM32X-2f-Lib

clean-STM32X-2f-Lib:
	-$(RM) ./STM32X/Lib/ADC.cyclo ./STM32X/Lib/ADC.d ./STM32X/Lib/ADC.o ./STM32X/Lib/ADC.su ./STM32X/Lib/CAN.cyclo ./STM32X/Lib/CAN.d ./STM32X/Lib/CAN.o ./STM32X/Lib/CAN.su ./STM32X/Lib/CLK.cyclo ./STM32X/Lib/CLK.d ./STM32X/Lib/CLK.o ./STM32X/Lib/CLK.su ./STM32X/Lib/COMP.cyclo ./STM32X/Lib/COMP.d ./STM32X/Lib/COMP.o ./STM32X/Lib/COMP.su ./STM32X/Lib/CRC.cyclo ./STM32X/Lib/CRC.d ./STM32X/Lib/CRC.o ./STM32X/Lib/CRC.su ./STM32X/Lib/Core.cyclo ./STM32X/Lib/Core.d ./STM32X/Lib/Core.o ./STM32X/Lib/Core.su ./STM32X/Lib/EEPROM.cyclo ./STM32X/Lib/EEPROM.d ./STM32X/Lib/EEPROM.o ./STM32X/Lib/EEPROM.su ./STM32X/Lib/FLASH.cyclo ./STM32X/Lib/FLASH.d ./STM32X/Lib/FLASH.o ./STM32X/Lib/FLASH.su ./STM32X/Lib/GPIO.cyclo ./STM32X/Lib/GPIO.d ./STM32X/Lib/GPIO.o ./STM32X/Lib/GPIO.su ./STM32X/Lib/I2C.cyclo ./STM32X/Lib/I2C.d ./STM32X/Lib/I2C.o ./STM32X/Lib/I2C.su ./STM32X/Lib/RNG.cyclo ./STM32X/Lib/RNG.d ./STM32X/Lib/RNG.o ./STM32X/Lib/RNG.su ./STM32X/Lib/SPI.cyclo ./STM32X/Lib/SPI.d ./STM32X/Lib/SPI.o ./STM32X/Lib/SPI.su ./STM32X/Lib/SUBGHZ.cyclo ./STM32X/Lib/SUBGHZ.d ./STM32X/Lib/SUBGHZ.o ./STM32X/Lib/SUBGHZ.su ./STM32X/Lib/TIM.cyclo ./STM32X/Lib/TIM.d ./STM32X/Lib/TIM.o ./STM32X/Lib/TIM.su ./STM32X/Lib/TSC.cyclo ./STM32X/Lib/TSC.d ./STM32X/Lib/TSC.o ./STM32X/Lib/TSC.su ./STM32X/Lib/UART.cyclo ./STM32X/Lib/UART.d ./STM32X/Lib/UART.o ./STM32X/Lib/UART.su ./STM32X/Lib/US.cyclo ./STM32X/Lib/US.d ./STM32X/Lib/US.o ./STM32X/Lib/US.su ./STM32X/Lib/USB.cyclo ./STM32X/Lib/USB.d ./STM32X/Lib/USB.o ./STM32X/Lib/USB.su ./STM32X/Lib/WDG.cyclo ./STM32X/Lib/WDG.d ./STM32X/Lib/WDG.o ./STM32X/Lib/WDG.su ./STM32X/Lib/syscalls.cyclo ./STM32X/Lib/syscalls.d ./STM32X/Lib/syscalls.o ./STM32X/Lib/syscalls.su

.PHONY: clean-STM32X-2f-Lib

