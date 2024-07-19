################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Battery.c \
../Core/Src/LED.c \
../Core/Src/Motor.c \
../Core/Src/Radio.c \
../Core/Src/Servo.c \
../Core/Src/Temperature.c \
../Core/Src/main.c \
../Core/Src/system_stm32c0xx.c 

OBJS += \
./Core/Src/Battery.o \
./Core/Src/LED.o \
./Core/Src/Motor.o \
./Core/Src/Radio.o \
./Core/Src/Servo.o \
./Core/Src/Temperature.o \
./Core/Src/main.o \
./Core/Src/system_stm32c0xx.o 

C_DEPS += \
./Core/Src/Battery.d \
./Core/Src/LED.d \
./Core/Src/Motor.d \
./Core/Src/Radio.d \
./Core/Src/Servo.d \
./Core/Src/Temperature.d \
./Core/Src/main.d \
./Core/Src/system_stm32c0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Battery.cyclo ./Core/Src/Battery.d ./Core/Src/Battery.o ./Core/Src/Battery.su ./Core/Src/LED.cyclo ./Core/Src/LED.d ./Core/Src/LED.o ./Core/Src/LED.su ./Core/Src/Motor.cyclo ./Core/Src/Motor.d ./Core/Src/Motor.o ./Core/Src/Motor.su ./Core/Src/Radio.cyclo ./Core/Src/Radio.d ./Core/Src/Radio.o ./Core/Src/Radio.su ./Core/Src/Servo.cyclo ./Core/Src/Servo.d ./Core/Src/Servo.o ./Core/Src/Servo.su ./Core/Src/Temperature.cyclo ./Core/Src/Temperature.d ./Core/Src/Temperature.o ./Core/Src/Temperature.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/system_stm32c0xx.cyclo ./Core/Src/system_stm32c0xx.d ./Core/Src/system_stm32c0xx.o ./Core/Src/system_stm32c0xx.su

.PHONY: clean-Core-2f-Src

