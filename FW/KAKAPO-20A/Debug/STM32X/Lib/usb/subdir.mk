################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32X/Lib/usb/USB_CTL.c \
../STM32X/Lib/usb/USB_EP.c \
../STM32X/Lib/usb/USB_PCD.c 

OBJS += \
./STM32X/Lib/usb/USB_CTL.o \
./STM32X/Lib/usb/USB_EP.o \
./STM32X/Lib/usb/USB_PCD.o 

C_DEPS += \
./STM32X/Lib/usb/USB_CTL.d \
./STM32X/Lib/usb/USB_EP.d \
./STM32X/Lib/usb/USB_PCD.d 


# Each subdirectory must supply rules for building sources it contributes
STM32X/Lib/usb/%.o STM32X/Lib/usb/%.su STM32X/Lib/usb/%.cyclo: ../STM32X/Lib/usb/%.c STM32X/Lib/usb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C031xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I../STM32X/Lib -I../Modules/Utils/FPROM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-STM32X-2f-Lib-2f-usb

clean-STM32X-2f-Lib-2f-usb:
	-$(RM) ./STM32X/Lib/usb/USB_CTL.cyclo ./STM32X/Lib/usb/USB_CTL.d ./STM32X/Lib/usb/USB_CTL.o ./STM32X/Lib/usb/USB_CTL.su ./STM32X/Lib/usb/USB_EP.cyclo ./STM32X/Lib/usb/USB_EP.d ./STM32X/Lib/usb/USB_EP.o ./STM32X/Lib/usb/USB_EP.su ./STM32X/Lib/usb/USB_PCD.cyclo ./STM32X/Lib/usb/USB_PCD.d ./STM32X/Lib/usb/USB_PCD.o ./STM32X/Lib/usb/USB_PCD.su

.PHONY: clean-STM32X-2f-Lib-2f-usb

