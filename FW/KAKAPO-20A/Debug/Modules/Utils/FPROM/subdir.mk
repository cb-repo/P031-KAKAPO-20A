################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modules/Utils/FPROM/FPROM.c 

OBJS += \
./Modules/Utils/FPROM/FPROM.o 

C_DEPS += \
./Modules/Utils/FPROM/FPROM.d 


# Each subdirectory must supply rules for building sources it contributes
Modules/Utils/FPROM/%.o Modules/Utils/FPROM/%.su Modules/Utils/FPROM/%.cyclo: ../Modules/Utils/FPROM/%.c Modules/Utils/FPROM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Modules-2f-Utils-2f-FPROM

clean-Modules-2f-Utils-2f-FPROM:
	-$(RM) ./Modules/Utils/FPROM/FPROM.cyclo ./Modules/Utils/FPROM/FPROM.d ./Modules/Utils/FPROM/FPROM.o ./Modules/Utils/FPROM/FPROM.su

.PHONY: clean-Modules-2f-Utils-2f-FPROM

